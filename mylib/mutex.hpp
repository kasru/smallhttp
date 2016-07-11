#ifndef MYLIB_MUTEX_HPP
#define MYLIB_MUTEX_HPP

#include "noncopyable.hpp"

#include <pthread.h>

#include <stdexcept>

namespace mylib {

class condition;

template <typename Mutex>
class lock_ops : private noncopyable
{
private:
	lock_ops() { }

public:
	typedef typename Mutex::cv_state lock_state;

	static void lock(Mutex& m)
	{
		m.do_lock();
	}
	static bool trylock(Mutex& m)
	{
		return m.do_trylock();
	}
	static void unlock(Mutex& m)
	{
		m.do_unlock();
	}
	static void lock(Mutex& m, lock_state& state)
	{
		m.do_lock(state);
	}
	static void unlock(Mutex& m, lock_state& state)
	{
		m.do_unlock(state);
	}
};

template <typename Mutex>
class scoped_lock : private noncopyable
{
public:
	typedef Mutex mutex_type;

	explicit scoped_lock(Mutex& mx, bool initially_locked=true)
		: m_mutex(mx), m_locked(false)
	{
		if (initially_locked) lock();
	}
	~scoped_lock()
	{
		if (m_locked) unlock();
	}

	void lock()
	{
		if (m_locked) throw std::exception();
		lock_ops<Mutex>::lock(m_mutex);
		m_locked = true;
	}
	void unlock()
	{
		if (!m_locked) throw std::exception();
		lock_ops<Mutex>::unlock(m_mutex);
		m_locked = false;
	}

	bool locked() const { return m_locked; }
	operator const void*() const { return m_locked ? this : 0; }

private:
	friend class condition;

	Mutex& m_mutex;
	bool m_locked;
};

class mutex: private noncopyable
{
public:
	friend class lock_ops<mutex>;
	typedef scoped_lock<mutex> scoped_lock;

	mutex()
	{
		int res = pthread_mutex_init(&m_mutex, 0);
		if (res != 0)
			throw std::exception();
	}
	~mutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}

private:
	struct cv_state
	{
		pthread_mutex_t* pmutex;
	};
	void do_lock()
	{
		int res = pthread_mutex_lock(&m_mutex);
		if (res == EDEADLK) throw std::exception();
	}
	void do_unlock()
	{
		int res = pthread_mutex_unlock(&m_mutex);
		if (res == EPERM) throw std::exception();
	}
	void do_lock(cv_state& state)
	{
	}
	void do_unlock(cv_state& state)
	{
	    state.pmutex = &m_mutex;
	}

	pthread_mutex_t m_mutex;
};

} // namespace mylib

#endif // MYLIB_MUTEX_HPP
