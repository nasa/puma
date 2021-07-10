#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <iostream>
#include <iomanip>

namespace TexGen
{
	/// Class used to meaure the amount of time it takes to perform a certain task
	class CLASS_DECLSPEC CTimer
	{
		friend std::ostream& operator<<(std::ostream& os, CTimer& t);

		private:
			bool running;
			clock_t start_clock;
			time_t start_time;
			double acc_time;

			/** Return the total time that the timer has been in the "running"
			state since it was first "started" or last "restarted".  For
			"short" time periods (less than an hour), the actual cpu time
			used is reported instead of the elapsed time.*/
			double elapsed_time();

		public:
			// 'running' is initially false.  A timer needs to be explicitly started
			// using 'start' or 'restart'
			CTimer() : running(false), start_clock(0), start_time(0), acc_time(0) { }

			/** Start a timer.  If it is already running, let it continue running.
			Print an optional message.*/
			void start(const char* msg = 0);
			/// Turn the timer off and start it again from 0.  Print an optional message.
			void restart(const char* msg = 0);
			/// Stop the timer and print an optional message.
			void stop(const char* msg = 0);
			/**
			Allow timers to be printed to ostreams using the syntax 'os << t'
			for an ostream 'os' and a timer 't'.  For example, "cout << t" will
			print out the total amount of time 't' has been "running".*/
			void check(const char* msg = 0);

	}; // class timer

	inline double CTimer::elapsed_time()
	{
		time_t acc_sec = time(0) - start_time;
		if (acc_sec < 3600)
			return (clock() - start_clock) / (1.0 * CLOCKS_PER_SEC);
		else
			return (1.0 * acc_sec);

	} // timer::elapsed_time

	inline void CTimer::start(const char* msg)
	{
		// Print an optional message, something like "Starting timer t";
		if (msg) TGLOG(msg);

		// Return immediately if the timer is already running
		if (running) return;

		// Set timer status to running and set the start time
		running = true;
		start_clock = clock();
		start_time = time(0);

	} // timer::start


	inline void CTimer::restart(const char* msg)
	{
		// Print an optional message, something like "Restarting timer t";
		if (msg) TGLOG(msg);

		// Set timer status to running, reset accumulated time, and set start time
		running = true;
		acc_time = 0;
		start_clock = clock();
		start_time = time(0);

	} // timer::restart

	inline void CTimer::stop(const char* msg)
	{
		// Print an optional message, something like "Stopping timer t";
		if (msg) TGLOG(msg);

		// Compute accumulated running time and set timer status to not running
		if (running) acc_time += elapsed_time();
		running = false;
	} // timer::stop

	inline void CTimer::check(const char* msg)
	{
		std::stringstream String;
		// Print an optional message, something like "Checking timer t";
		if (msg) String << msg << " : ";

		String << "Elapsed time [" << std::setiosflags(std::ios::fixed)
				<< std::setprecision(2)
				<< acc_time + (running ? elapsed_time() : 0) << "] seconds\n";

		TGLOG(String.str());

	} // timer::check

	inline std::ostream& operator<<(std::ostream& os, CTimer& t)
	{
		os << std::setprecision(2) << std::setiosflags(std::ios::fixed)
			<< t.acc_time + (t.running ? t.elapsed_time() : 0);
		return os;
	}

	//===========================================================================
}// namespace TexGen;

#endif // TIMER_H

