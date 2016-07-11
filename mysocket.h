#ifndef MY_SOCKET_H
#define MY_SOCKET_H

#include "mylib/noncopyable.hpp"
#include "stub.h"

#include <vector>
#include <stdexcept>

class mysocket_exception : public std::runtime_error
{
public:
	explicit mysocket_exception(std::string const & what)
		: std::runtime_error(what)
	{}
	virtual ~mysocket_exception() throw() {}
};

class mysocket : public mylib::noncopyable
{
public:
	static int recv(int fd, char * buf, int size_buf);
	static int recv_all(int fd, char * buf, int size_buf);
	static void send_all(int fd, char const * buf, int size_buf);

	mysocket();
	~mysocket();
	void create(unsigned short const port);
	void listen(int const backlog = 16) const;
	void accept(mysocket & new_client_sock) const;
	void close();
	int recv(char * buf, int size_buf) const
	{
		return recv(m_fd, buf, size_buf);
	}
	int recv(std::vector<char> & data) const
	{
		if (data.empty())
		{
			return 0;
		}
		return recv(&data[0], data.size());
	}
	int recv_all(char * buf, int size_buf) const
	{
		return recv_all(m_fd, buf, size_buf);
	}
	void send(char const * buf, int size_buf) const
	{
		send_all(m_fd, buf, size_buf);
	}
	void send(std::string const & str) const
	{
		if (!str.empty())
		{
			send(str.c_str(), str.size());
		}
	}
private:
	unsigned short m_port;
	sockaddr_in m_sockaddr;
	int m_fd;
};

#endif //MY_SOCKET_H
