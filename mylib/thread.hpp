#ifndef MYLIB_THREAD_HPP
#define MYLIB_THREAD_HPP

#include "noncopyable.hpp"
#include "mutex.hpp"

#include <pthread.h>

#include <list>
#include <algorithm>
#include <stdexcept>

namespace mylib {

namespace detail {
template<typename T>
void * thread_func(void *param)
{
    T* p = reinterpret_cast<T*>(param);
    (*p)();
    delete p;
	return 0;
}
} //namespace detail

class thread : private noncopyable
{
public:
	template<typename T>
    thread(T f)
    {
    	T* param = new T(f);
    	int res = pthread_create(&m_thread, 0, &detail::thread_func<T>, param);
    	if (res != 0)
    		throw std::exception();
    }
	~thread()
    {
    	pthread_detach(m_thread);
    }

	bool operator==(thread const & other) const
	{
		return pthread_equal(m_thread, other.m_thread) != 0;
	}
	bool operator!=(thread const & other) const
	{
		return !operator==(other);
	}

	void join()
	{
		pthread_join(m_thread, 0);
	}

	static void exit(void * value_ptr = 0)
	{
		pthread_exit(value_ptr);
	}

private:
	pthread_t m_thread;
};

class thread_group : private noncopyable
{
public:
    thread_group() {}
    ~thread_group()
    {
        for(std::list<thread*>::iterator it=threads.begin(),end=threads.end();
            it!=end;
            ++it)
        {
            delete *it;
        }
    }

    template<typename F>
    thread* create_thread(F threadfunc)
    {
        std::auto_ptr<thread> new_thread(new thread(threadfunc));
        threads.push_back(new_thread.get());
        return new_thread.release();
    }

    void add_thread(thread* thrd)
    {
		mutex::scoped_lock scoped_lock(m_mutex);
        if(thrd)
        {
            threads.push_back(thrd);
        }
    }

    void remove_thread(thread* thrd)
    {
	    mutex::scoped_lock scoped_lock(m_mutex);
        std::list<thread*>::iterator const it=std::find(threads.begin(),threads.end(),thrd);
        if(it!=threads.end())
        {
            threads.erase(it);
        }
    }

    void join_all()
    {
	    mutex::scoped_lock scoped_lock(m_mutex);
        for(std::list<thread*>::iterator it=threads.begin(),end=threads.end();
            it!=end;
            ++it)
        {
            (*it)->join();
        }
    }

    size_t size() const
    {
        return threads.size();
    }

private:
    std::list<thread*> threads;
	mutex m_mutex;
};

} //namespace mylib

#endif //MYLIB_THREAD_HPP
