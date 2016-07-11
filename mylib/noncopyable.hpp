#ifndef MYLIB_NONCOPYABLE_HPP
#define MYLIB_NONCOPYABLE_HPP

namespace mylib {

class noncopyable
{
protected:
	noncopyable() {}
	~noncopyable() {}
private:
	noncopyable(noncopyable const &);
	noncopyable const & operator=(noncopyable const &);
};

} //namespace mylib

#endif  //MYLIB_NONCOPYABLE_HPP
