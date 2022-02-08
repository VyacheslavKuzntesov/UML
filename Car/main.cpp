#include<iostream>
#include<iomanip>
#include<thread>
#include<chrono>
#include<conio.h>
#include<Windows.h>

using namespace std;
using namespace std::literals::chrono_literals;

//#define HOMEWORK

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
	double give_fuel(double amount)
	{/*
		if (fuel_level - amount > 0)fuel_level -= amount;
		else fuel_level = 0;*/
		if (fuel_level < amount)return fuel_level = 0;
		fuel_level -= amount;
		return fuel_level;
	}
#ifdef HOMEWORK
	double fuel_consumption(double amount)
	{
		if (fuel_level - amount > 0)fuel_level -= amount;
		else fuel_level = 0;
		return fuel_level;
	}
#endif // HOMEWORK

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
	void set_consumption_per_second(double consumption)
	{
		if (consumption >= .0003 && consumption <= .003)
		{
			this->consumption_per_second = consumption;
		}
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

//enum Key
//{
//	Enter = 13,
//	Escape = 27
//};

#define Enter 13
#define Escape 27
#define Space 32
#define ArrowUp 72
#define ArrowDown 80
#define ArrowLeft 75
#define ArrowRight 77

class Car
{
	Engine engine;
	Tank tank;
	bool driver_inside;
	int speed;
	const int MAX_SPEED;
	int accelleration;

	struct Control
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
	}control;

public:
	Car(double consumption, unsigned int volume, int max_speed, int accelleration = 10) :
		engine(consumption),
		tank(volume),
		MAX_SPEED(max_speed >= 80 && max_speed <= 400 ? max_speed : 180),
		accelleration(20)
	{
		driver_inside = false;
		speed = 0;
		this->accelleration = accelleration;
		cout << "Your car is ready: " << this << endl;
	}
	~Car()
	{
		cout << "Your car is over: " << this << endl;
	}

	void fill(double fuel)
	{
		tank.fill(fuel);
	}
	void get_in()
	{
		driver_inside = true;
		control.panel_thread = thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (control.panel_thread.joinable())control.panel_thread.join();
		system("CLS");
		cout << "You are out of your Car, press Enter to get in" << endl;
	}
	void start_engine()
	{
		if (driver_inside && tank.give_fuel(engine.get_consumption_per_second()))
		{
			engine.start();
			control.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop_engine()
	{
		engine.stop();
		if (control.engine_idle_thread.joinable())control.engine_idle_thread.join();
	}
#ifdef HOMEWORK
	void free_wheeling()
	{
		while (speed > 0)
		{
			speed--;
			if (speed < 0)speed = 0;
			this_thread::sleep_for(1s);
		}
	}
	void accelerate()
	{
		if (engine.started() && speed < MAX_SPEED)
		{
			speed += accelleration;
			if (control.free_wheeling_thread.get_id() == std::thread::id())
				control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
		}
		std::this_thread::sleep_for(1s);
	}
	void slow_down()
	{
		if (speed > 0)
		{
			speed -= accelleration;
			if (speed < accelleration)
			{
				speed = 0;
				if (control.free_wheeling_thread.joinable())
					control.free_wheeling_thread.join();
			}
		}
		std::this_thread::sleep_for(1s);
	}
#endif // HOMEWORK


	void control_car()
	{
		cout << "Press Enter to get in" << endl;
		char key;
		do
		{
			key = _getch();
			switch (key)
			{
			case 'F':case 'f':
				double fuel;
				cout << "Введите объем топлива: "; cin >> fuel;
				fill(fuel);
				break;
			case 'I':case 'i':
				if (engine.started())stop_engine();
				else start_engine();
				break;

#ifdef HOMEWORK
			case 'W':case 'w':case ArrowUp:
				accelerate();
				break;
			case 'S':case 's':case ArrowDown:case Space:
				slow_down();
				break;
#endif // HOMEWORK

			case Enter:
				if (driver_inside)get_out();
				else get_in();
				break;
			case 'W':case 'w':
				if (driver_inside && engine.started())
				{
					if (speed < MAX_SPEED)speed += accelleration;
					if (speed > MAX_SPEED)speed = MAX_SPEED;
					if (!control.free_wheeling_thread.joinable())
						control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
					std::this_thread::sleep_for(1s);
				}
				break;
			case 'S':case 's':
				if (speed > 0)speed -= accelleration;
				if (speed < 0)speed = 0;
				std::this_thread::sleep_for(1s);
				break;
			case Escape:
				speed = 0;
				stop_engine();
				get_out();
			default:
				break;
			}
			if (tank.get_fuel_level() == 0)stop_engine();
			if (speed == 0 && control.free_wheeling_thread.joinable())control.free_wheeling_thread.join();
		} while (key != Escape);
	}

	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second()))
		{
			std::this_thread::sleep_for(1s);
		}
		//stop_engine();
		engine.stop();
	}

	void free_wheeling()
	{
		while (speed > 0)
		{
			speed--;
			std::this_thread::sleep_for(1s);
		}
	}



	void panel()const
	{
		while (driver_inside)
		{
			system("CLS");
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			int i = 0;
			for (; i < speed * .4; i++)
			{
				if (i > 50)SetConsoleTextAttribute(hConsole, 0x0E);
				if (i > 80)SetConsoleTextAttribute(hConsole, 0x0C);
				cout << "|";
			}
			SetConsoleTextAttribute(hConsole, 0x07);
			cout << i;
			cout << endl;
			cout << "Fuel level " << std::setprecision(4) << fixed << tank.get_fuel_level() << " liters.\t";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0x60);
				cout << "LOW FUEL";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Engine is " << (engine.started() ? "started" : "stoped") << endl;
			cout << "Speed: " << speed << " km/h\n";


#ifdef HOMEWORK
			cout << "Max speed:\t" << MAX_SPEED << " km/h" << endl;
			cout << "Current speed: " << speed << " km/h" << endl;
#endif // HOMEWORK

			std::this_thread::sleep_for(500ms);
			}
		}

	void info()const
	{
		engine.info();
		tank.info();
		cout << "Max speed:\t" << MAX_SPEED << " km/h\n";
	}
};




#ifdef HOMEWORK
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
				cout << "Сколько заправить: "; cin >> amount;
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
#endif // HOMEWORK


//#define TANK_CHECK
//#define ENGINE_CHECK

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

#ifdef ENGINE_CHECK
	Engine engine(0);
	engine.info();
#endif // ENGINE_CHECK

#ifdef HOMEWORK
	Car car(8, 40);
	car.info();
	car.control_car();
#endif // HOMEWORK

	Car bmw(11, 80, 250);
	//bmw.info();
	bmw.control_car();
}