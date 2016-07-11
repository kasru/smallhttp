#ifndef MYLIB_FUNCTION_HPP
#define MYLIB_FUNCTION_HPP

namespace mylib {

template<typename T>
struct result_type;

template<typename R>
struct result_type<R()>
{
	typedef R type;
};

template<typename R, typename P1>
struct result_type<R(P1)>
{
	typedef R type;
};

template<typename R, typename P1, typename P2>
struct result_type<R(P1, P2)>
{
	typedef R type;
};

template<typename R, typename P1, typename P2, typename P3>
struct result_type<R(P1, P2, P3)>
{
	typedef R type;
};

template<typename FunctionType>
class function
{
	FunctionType * f;
	typedef typename result_type<FunctionType>::type R;

public:
	template<typename T>
	function(T ff) : f(ff) {}
	R operator ()()
	{
		return f();
	}
	template<typename P1>
	R operator ()(P1 p1)
	{
		return f(p1);
	}
	template<typename P1, typename P2>
	R operator ()(P1 p1, P2 p2)
	{
		return f(p1, p2);
	}
	template<typename P1, typename P2, typename P3>
	R operator ()(P1 p1, P2 p2, P3 p3)
	{
		return f(p1, p2, p3);
	}
};

} //namespace mylib

#endif //MYLIB_FUNCTION_HPP
