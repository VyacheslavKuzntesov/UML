#include<iostream>
#include <chrono>
#include<thread>

using namespace std::chrono_literals;

bool finish = false;

void plus()
{
	while (!finish)
	{
		std::cout << "+ ";
		std::this_thread::sleep_for(1s);
	}
}

void minus()
{
	while (!finish)
	{
		std::cout << "- ";
		std::this_thread::sleep_for(1s);
	}
}

void main()
{
	std::thread plus_thread(plus);
	std::thread minus_thread(minus);

	std::cin.get();

	plus_thread.join(); //сихронизирует поток с основным потоком
	minus_thread.join();
}