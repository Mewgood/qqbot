#ifndef NIL_THREAD_HPP
#define NIL_THREAD_HPP

#include <nil/function.hpp>
#include <nil/exception.hpp>
#include <nil/environment.hpp>
#include <nil/types.hpp>

#ifdef NIL_WINDOWS
#include <nil/windows.hpp>
#else
#include <pthread.h>
#endif

namespace nil
{
	class thread
	{
	public:
		//typedef unary_function<int, void *> function_type;
		typedef unary_function<void, void *> function_type;

#ifdef NIL_WINDOWS
		//friend ::DWORD WINAPI thread_proxy(LPVOID thread_pointer);
		friend void thread_proxy(void * thread_pointer);
		typedef HANDLE thread_handle_type;
#else
		friend void * thread_proxy(void * thread_pointer);
		typedef pthread_t thread_handle_type;
#endif

		thread();
		thread(function_type function);
		~thread();
		void start(void * function_argument);
		void start(function_type new_function, void * function_argument);
		void start();
		void start(function_type new_function);
		bool join();
		bool terminate();

		//int return_value();
		bool is_running();

	private:
		bool thread_is_running;
		bool handle_exists;
		thread_handle_type thread_handle;
		function_type function;
		void * argument;

		thread(thread const &);
		thread & operator=(thread const &);

		void run();
		void close_handle();
	};

	class mutex
	{
	public:
		mutex();
		~mutex();
		void lock();
		void unlock();
		void manual_init();
		void info();

	private:
#ifdef NIL_WINDOWS
		CRITICAL_SECTION critical_section;
		HANDLE mutex_handle;
#else
		pthread_mutex_t mutex_handle;
#endif
		mutex(mutex const &);
		mutex & operator=(mutex const &);
	};

	class event
	{
	public:
		event();
		~event();

		void set();
		void wait();
		void wait(ulong milliseconds);

	private:
#ifdef NIL_WINDOWS
		HANDLE event_handle;
#else
		pthread_cond_t event_handle;
#endif

		event(event const &);
		event & operator=(event const &);
	};

	void sleep(ulong milliseconds);
	void sleep();
}

#endif
