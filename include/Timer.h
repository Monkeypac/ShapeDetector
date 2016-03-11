/**
 * @file Timer.h
 */
#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include "Log.h"

class Timer
{
public:
    /**
     * @function chronos
     * @brief used for evaluating any execution time
     * @param name the message to display
     * @return elapsed time since last corresponding call
     */
    template<class T>
    static double chronos(std::string name = "")
    {
		static bool show = false;
		static std::chrono::time_point<std::chrono::system_clock> start;

		std::chrono::time_point<std::chrono::system_clock> actual;
		std::chrono::duration<double> elapsed = std::chrono::duration<double>();
        actual = time_of_day();

        //On the first call, the time is registered
        if (!show){
            start = actual;
			show = true;
        }
        //On the second call, the time is printed
        else {
			elapsed = actual - start;
			Log::info("Timer") << name << elapsed.count() << "seconds";
			show = false;
        }

		return elapsed.count();
    }

    /**
     * @brief my_time_of_day
     * @return Current system time
     */
    static std::chrono::time_point<std::chrono::system_clock> time_of_day()
	{
		return std::chrono::system_clock::now();
	}

private:
    //We don't want this class to be instanciated
    Timer() {}
};

#endif // TIMER_H
