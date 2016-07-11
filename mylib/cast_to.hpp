#ifndef CAST_TO_HPP
#define CAST_TO_HPP

#include <iostream>
#include <strstream>

namespace mylib {

template<typename To, typename From>
To cast_to(From const& v)
{
	To result;
	std::strstream ss;
	ss << v;
	ss >> result;
	return result;
}

} // namespace mylib

#endif //CAST_TO_HPP
