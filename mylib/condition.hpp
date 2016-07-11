#ifndef MYLIB_CONDITION_HPP
#define MYLIB_CONDITION_HPP

#include <pthread.h>

namespace mylib {

class condition : private noncopyable
{
public:
	condition()
	{
		int res = pthread_cond_init(&m_condition, 0);
		if (res != 0)
			throw std::exception();
	}
	~condition()
	{
    	pthread_cond_destroy(&m_condition);
	}

	void notify_one()
	{
		pthread_cond_signal(&m_condition);
	}
	void notify_all()
	{
		pthread_cond_broadcast(&m_condition);
	}

	template <typename L>
	void wait(L& lock)
	{
		if (!lock)
			throw lock_error();

		pthread_cond_wait(&m_condition, lock.m_mutex);
	}

	template <typename L, typename Pr>
	void wait(L& lock, Pr pred)
	{
		if (!lock)
			throw lock_error();

		while (!pred())
			pthread_cond_wait(&m_condition, lock.m_mutex);
	}

private:
    pthread_cond_t m_condition;

	template <typename M>
	void do_wait(M& mutex)
	{
		typedef lock_ops<M> lock_ops;
		typename lock_ops::lock_state state;
		lock_ops::unlock(mutex, state);
        pthread_cond_wait(&m_condition, state.pmutex);
		lock_ops::lock(mutex, state);
	}
};

} // namespace mylib

#endif //MYLIB_CONDITION_HPP
