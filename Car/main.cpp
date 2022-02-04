#include<iostream>
#include<thread>
#include<conio.h>

using namespace std;

#define MIN_TANK_VOLUME 20
#define MAX_TANK_VOLUME 150

class Tank
{
	const unsigned int VOLUME = 60;			//Объем бака (xарактеристика)
	double fuel_level;						//Уровень топлива в баке (состояние)
public:
	unsigned int get_VOLUME()const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	void fill(double fuel)
	{
		if (fuel < 0)return;
		if (fuel_level + fuel <= VOLUME)fuel_level += fuel;
		else fuel_level = VOLUME;
	}
	double fuel_consumption(double amount)
	{
		if (fuel_level - amount > 0)fuel_level -= amount;
		else fuel_level = 0;
		return fuel_level;
	}
	Tank(double VOLUME) :VOLUME(VOLUME >= MIN_TANK_VOLUME && VOLUME <= MAX_TANK_VOLUME ? VOLUME : 80), fuel_level(0)
	{
		/*if (VOLUME >= MIN_TANK_VOLUME && VOLUME <= MAX_TANK_VOLUME)this->VOLUME = VOLUME;*/
		cout << "Tank is ready" << endl;
	}
	~Tank()
	{
		cout << "Tank is over" << endl;
	}

	void info()const
	{
		cout << "Volume:     " << VOLUME << " liters.\n";
		cout << "Fuel level: " << fuel_level << " liters.\n";
	}
};

#define MIN_ENGINE_CONSUMPTION 6
#define MAX_ENGINE_CONSUMPTION 25

class Engine
{
	double consumption;				//Расход топлива на 100 км
	double consumption_per_second;	//Расход за 1 секунду
	bool is_started;
public:
	double get_consumption()const
	{
		return consumption;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	double get_consumption_per_hour()const
	{
		return consumption_per_second * 3600;
	}
	bool started()const
	{
		return is_started;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	void set_consumption(double consumption)
	{
		if (consumption < MIN_ENGINE_CONSUMPTION)consumption = MIN_ENGINE_CONSUMPTION;
		else if (consumption > MAX_ENGINE_CONSUMPTION)consumption = MAX_ENGINE_CONSUMPTION;
		this->consumption = consumption;
		this->consumption_per_second = this->consumption * .3e-4;
	}
	Engine(double consumption)
	{
		set_consumption(consumption);
		is_started = false;
		cout << "Engine is ready" << endl;
	}
	~Engine()
	{
		cout << "Engine is over" << endl;
	}

	void info()const
	{
		cout << "Consumption per 100 km: " << consumption << " liters.\n";
		cout << "Consumption per second: " << consumption_per_second << " liters.\n";
		cout << "Consumption per hour  : " << get_consumption_per_hour() << " liters.\n";
		cout << "Engine is " << (is_started ? "started" : "stopped") << endl;
	}
};

struct Control
{
	std::thread main_thread;
	std::thread panel_thread;
	std::thread сonsumption_thread;
}control;

class Car
{
	Tank tank;
	Engine engine;
	bool driver;
public:

	void get_in()
	{
		driver = true;
		control.panel_thread = thread(&Car::panel, this);
	}
	void get_out()
	{
		driver = false;
		if (control.panel_thread.joinable())control.panel_thread.join();
	}

	Car(double consumption, unsigned int VOLUME) :engine(consumption), tank(VOLUME), driver(false)
	{
		cout << "Car is ready." << this << endl;
	}
	~Car()
	{
		cout << "Your car is over." << this << endl;
	}

	void panel() const
	{
		while (driver)
		{
			system("CLS");
			cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
			cout << "Fuel level: " << tank.get_fuel_level() << " liters";
			if (tank.get_fuel_level() < 5)cout << "Low fuel";
			cout << endl;
			cout << "Consumption per 100 km: " << engine.get_consumption() << " liters.\n";
			cout << "Consumption per second: " << engine.get_consumption_per_second() << " liters.\n";
			cout << "Consumption per hour  : " << engine.get_consumption_per_hour() << " liters.\n\n";
			cout << "Управление машиной:" << endl;
			cout << "Enter - сесть или выйти из машины " << endl;
			cout << "F - заправить машину" << endl;
			cout << "S - запустить машину" << endl;
			cout << "Escape - выйти из программы" << endl;
			std::this_thread::sleep_for(1s);
		}
		system("CLS");
		cout << "Нажмите Enter чтобы сесть в машину" << endl;
	}
	void start()
	{
		if (driver && tank.get_fuel_level() > 0)
		{
			engine.start();
			control.сonsumption_thread = thread(&Car::engine_idle, this);
		}
	}
	void stop()
	{
		engine.stop();
		if (control.сonsumption_thread.joinable())control.сonsumption_thread.join();
	}
	void engine_idle()
	{
		while (tank.fuel_consumption(engine.get_consumption_per_second()) && engine.started())
		{
			std::this_thread::sleep_for(1s);
		}
		engine.stop();
	}
	void control_car()
	{
		char key;
		do
		{
			key = _getch();
			switch (key)
			{
			case 13:
				if (driver)get_out();
				else get_in();
				break;
			case 'F':case 'f':
				double amount;
				cout << "Сколько заправить: ";cin >> amount;
				tank.fill(amount);
				break;
			case 'S':case 's':
				if (engine.started())stop();
				else start();
				break;
			case 27:
				stop();
				get_out();
			}
			if (tank.get_fuel_level() == 0 && control.сonsumption_thread.joinable())control.сonsumption_thread.join();
		} while (key != 27);
	}
	void info()const
	{
		tank.info();
		cout << endl;
		engine.info();
		cout << endl;
		cout << endl;
		cout << "Управление машиной:" << endl;
		cout << "Enter - сесть или выйти из машины " << endl;
		cout << "F - заправить машину" << endl;
		cout << "S - запустить машину" << endl;
		cout << "Escape - выйти из программы" << endl;
	}

};

//#define TANK_CHECK
#define ENGINE_CHECK

void main()
{
	setlocale(LC_ALL, "");

#ifdef TANK_CHECK
	Tank tank(40);
	tank.info();
	int fuel;
	while (true)
	{
		cout << "Введите объем топлива: "; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	}
#endif // TANK_CHECK

	/*Engine engine(12);
	engine.info();*/

	Car car(8, 40);
	car.info();
	car.control_car();
}