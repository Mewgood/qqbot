#include <nil/thread.hpp>

#ifdef NIL_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <process.h>
#endif

#include <iostream>

namespace nil
{
	
#ifdef NIL_WINDOWS

	//::DWORD WINAPI thread_proxy(LPVOID thread_pointer)
	void thread_proxy(void * thread_pointer)
	{
		thread * self = reinterpret_cast<thread *>(thread_pointer);
		//int exit_code = self->run();
		self->run();
		self->thread_is_running = false;
		//return static_cast< ::DWORD>(exit_code);
		//return 0;
	}
	
#else

	void * thread_proxy(void * thread_pointer)
	{
		thread * self = reinterpret_cast<thread *>(thread_pointer);
		self->run();
		self->thread_is_running = false;
		return 0;
	}
	
#endif

	thread::thread():
		handle_exists(false),
		thread_is_running(false)
	{
	}

	thread::thread(function_type function):
		handle_exists(false),
		thread_is_running(false),
		function(function)
	{
	}

	thread::~thread()
	{
#ifdef NIL_WINDOWS
		if(thread_is_running == true)
			::WaitForSingleObject(thread_handle, INFINITE);
		if(handle_exists == true)
			::CloseHandle(thread_handle);
#else
		if(thread_is_running == true)
		{
			void * unused;
			pthread_join(thread_handle, &unused);
		}
#endif
	}

	void thread::start(void * function_argument)
	{
		if(thread_is_running == false)
		{
			argument = function_argument;

			close_handle();
			if(function.is_initialised() == false)
				throw exception("Tried to create a thread with an uninitialised function object");
#ifdef NIL_WINDOWS
			/*
			::DWORD thread_identifier;
			thread_handle = ::CreateThread(0, 0, &thread_proxy, this, 0, &thread_identifier);
			*/
			thread_handle = reinterpret_cast< ::HANDLE>(::_beginthread(&thread_proxy, 0, this));
			if(thread_handle == reinterpret_cast<nil::thread::thread_handle_type>(-1))
				throw exception("Failed to create Windows thread");
#else
			int result = ::pthread_create(&thread_handle, 0, &thread_proxy, this);
			if(result != 0)
				throw exception("Failed to create POSIX thread");
#endif
			else
			{
				handle_exists = true;
				thread_is_running = true;
			}
		}
	}

	void thread::start(function_type new_function, void * function_argument)
	{
		function = new_function;
		start(function_argument);
	}

	void thread::start()
	{
		start(0);
	}

	void thread::start(function_type new_function)
	{
		start(new_function, 0);
	}

	bool thread::join()
	{
		if(thread_is_running == true)
		{
#ifdef NIL_WINDOWS
			::DWORD wait_result = ::WaitForSingleObject(thread_handle, INFINITE);
			if(wait_result == WAIT_FAILED)
				return false;
#else
			void * unused;
			int result = pthread_join(thread_handle, &unused);
			if(result != 0)
				return false;
#endif
		}
		else
			return false;
		return true;
	}

/*
	int thread::return_value()
	{
		if(handle_exists == true)
		{
			::DWORD exit_code;
			::BOOL exit_code_result = ::GetExitCodeThread(thread_handle, &exit_code);
			if(exit_code_result == 0)
			{
				close_handle();
				throw exception("Failed to retrieve thread exit code");
			}
			return static_cast<int>(exit_code);
		}
		else
			throw exception("Can't retrieve thread return value after the handle has been closed");
	}
*/

	bool thread::is_running()
	{
		return thread_is_running;
	}
	
	void thread::run()
	{
		//int output = function(argument);
		function(argument);
		thread_is_running = false;
		//return output;
	}

	bool thread::terminate()
	{
#ifdef NIL_WINDOWS
			::TerminateThread(thread_handle, 0);
#endif
			return 1;
	}

	void thread::close_handle()
	{
		if(handle_exists == true)
		{
#ifdef NIL_WINDOWS
			::CloseHandle(thread_handle);
#endif
			handle_exists = false;
		}
	}

//#define USE_CRITICAL_SECTION

	void mutex::manual_init() {
#ifdef NIL_WINDOWS
#ifdef USE_CRITICAL_SECTION
		::InitializeCriticalSection(&critical_section);
#else
		mutex_handle = ::CreateMutex(NULL, FALSE, NULL);
		if(mutex_handle == NULL)
			throw nil::exception("Failed to create Windows mutex");
#endif
#else
		int result = pthread_mutex_init(&mutex_handle, 0);
		if(result == 0)
			throw nil::exception("Failed to create POSIX mutex");
#endif
	}

	mutex::mutex()
	{
		manual_init();
	}

	mutex::~mutex()
	{
#ifdef NIL_WINDOWS
#ifdef USE_CRITICAL_SECTION
		::DeleteCriticalSection(&critical_section);
#else
		::CloseHandle(mutex_handle);
#endif
#else
		pthread_mutex_destroy(&mutex_handle);
#endif
	}

	void mutex::lock()
	{
#ifdef NIL_WINDOWS
#ifdef USE_CRITICAL_SECTION
		::EnterCriticalSection(&critical_section);
#else
		::WaitForSingleObject(mutex_handle, INFINITE);
#endif
#else
		pthread_mutex_lock(&mutex_handle);
#endif
	}

	void mutex::unlock()
	{
#ifdef NIL_WINDOWS
#ifdef USE_CRITICAL_SECTION
		::LeaveCriticalSection(&critical_section);
#else
		::ReleaseMutex(mutex_handle);
#endif
#else
		pthread_mutex_unlock(&mutex_handle);
#endif
	}

	void mutex::info()
	{
		unsigned char * hack = reinterpret_cast<unsigned char*>(this);
		std::cout << std::hex;
		for(std::size_t i = 0; i < sizeof(mutex); i++)
		{
			std::cout.width(2);
			std::cout.fill('0');
			std::cout << (unsigned long)(unsigned char)hack[i] << " ";
		}
		std::cout << std::dec << std::endl;
	}

	event::event()
	{
#ifdef NIL_WINDOWS
		event_handle = ::CreateEvent(0, TRUE, FALSE, 0);
		if(event_handle == 0)
			throw exception("Failed to create Windows event");
#else
		int result = pthread_cond_init(&event_handle, 0);
		if(result != 0)
			throw exception("Failed to initialise POSIX thread condition variable");
#endif
	}

	event::~event()
	{
#ifdef NIL_WINDOWS
		::CloseHandle(event_handle);
#else
		pthread_cond_destroy(&event_handle);
#endif
	}

	void event::set()
	{
#ifdef NIL_WINDOWS
		if(::SetEvent(event_handle) == 0)
			throw exception("Failed to set Windows event");
#else
		//broadcast or signal?
		if(pthread_cond_broadcast(&event_handle) != 0)
			throw exception("Failed to broadcast event signal to POSIX threads");
#endif	
	}

	void event::wait()
	{
#ifdef NIL_WINDOWS
		if(::WaitForSingleObject(event_handle, INFINITE) == WAIT_FAILED)
			throw exception("Failed to wait for Windows event");
		if(::ResetEvent(event_handle) == 0)
			throw exception("Failed to reset Windows event");
#else
		//shit this is fucked, it is bound to a mutex...
		//if(pthread_cond_wait(&event_handle, 
#endif
	}

	void event::wait(ulong milliseconds)
	{
#ifdef NIL_WINDOWS
		if(::WaitForSingleObject(event_handle, static_cast< ::DWORD>(milliseconds)) == WAIT_FAILED)
			throw exception("Failed to wait for event");
#endif
	}

	void sleep(ulong milliseconds)
	{
#ifdef NIL_WINDOWS
		::Sleep(static_cast< ::DWORD>(milliseconds));
#else
		::usleep(static_cast<useconds_t>(milliseconds * 1000));
#endif
	}

	void sleep()
	{
#ifdef NIL_WINDOWS
		::Sleep(INFINITE);
#else
		while(true)
			sleep(1000);
#endif
	}
}
