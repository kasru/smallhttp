#include "stub.h"
#include "mysocket.h"

#include <stdexcept>
#include <strstream>

mysocket::mysocket() : m_port(0), m_fd(-1)
{
	::memset(&m_sockaddr,0,sizeof(m_sockaddr));
}

mysocket::~mysocket()
{
	if (-1  != m_fd)
	{
		close();
	}
}

void mysocket::create(unsigned short const port)
{
	int most_recent_errno = 0;

	m_port = port;
	::memset(&m_sockaddr,0,sizeof(m_sockaddr));

	// Listen on our designated port#
	m_sockaddr.sin_port = ::htons(port);
	// Fill in the rest of the address structure
	m_sockaddr.sin_family = PF_INET;
	m_sockaddr.sin_addr.s_addr = INADDR_ANY;

	m_fd = ::socket(PF_INET, SOCK_STREAM, 0);
	if(m_fd < 0)
	{
		most_recent_errno = errno;
	}
	if (!most_recent_errno)
	{
		//reuse address
		int sockopt = 1;
		if (::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR,
			reinterpret_cast<char*>(&sockopt), sizeof(sockopt)) < 0)
		{
			most_recent_errno = errno;
			close();
			m_fd = -1;
		}
	}
	if (!most_recent_errno)
	{
		// bind our name to the socket
		if (::bind(m_fd, reinterpret_cast<sockaddr *>(&m_sockaddr), sizeof(m_sockaddr)) < 0)
		{
			most_recent_errno = errno;
			close();
			m_fd = -1;
		}
	}
	if(m_fd < 0)
	{
		std::ostrstream ostr;
		ostr << "Can't create socket " << port << ".";
		if(most_recent_errno)
			ostr << " " << ::strerror(most_recent_errno);
		throw mysocket_exception(ostr.str());
	}
}

void mysocket::listen(int const backlog /*= 16*/) const
{
	// Set the socket to listen
	if (::listen(m_fd, backlog) < 0)
	{
		throw mysocket_exception(::strerror(errno));
	}
}

void mysocket::accept(mysocket & new_client_sock) const
{
	while(true)
	{
		socklen_t sin_size = static_cast<socklen_t>(sizeof(new_client_sock.m_sockaddr));
		new_client_sock.m_fd = ::accept(m_fd, reinterpret_cast<sockaddr *>(&new_client_sock.m_sockaddr), &sin_size);
		if (-1 == new_client_sock.m_fd)
		{
			if (errno == EINTR)
			{
				continue; //interrupted by signal
			}
			else
			{
				throw mysocket_exception(::strerror(errno));
			}
		}
		break;
	} //while
	new_client_sock.m_port = ::ntohs(new_client_sock.m_sockaddr.sin_port);
	int val = 1;
	if (::setsockopt(new_client_sock.m_fd, SOL_SOCKET, SO_KEEPALIVE,
					reinterpret_cast<char*>(&val), sizeof(val)) < 0)
	{
		int most_recent_errno(errno);
		new_client_sock.close();
		throw mysocket_exception(::strerror(most_recent_errno));
	}
}

void mysocket::close()
{
	::closesocket(m_fd);
	m_fd = -1;
}

/*static*/ int mysocket::recv(int fd, char * buf, int size_buf)
{
	int sz = 0;
	//write to descriptor
	while (size_buf)
	{
		sz = ::recv(fd, buf, size_buf, 0);
		if (-1 == sz)
		{
			if (EINTR == errno)
			{
				continue; //interrupted by signal
			}
			else
			{
				throw std::runtime_error(::strerror(errno));
			}
		}
		break;
	}
	return sz;
}

/*static*/ int mysocket::recv_all(int fd, char * buf, int size_buf)
{
	int ret = size_buf;
	//write to descriptor
	while (size_buf)
	{
		int sz = recv(fd, buf, size_buf);
		size_buf -= sz;
		buf += sz;
	}
	return ret;
}

/*static*/ void mysocket::send_all(int fd, char const * buf, int size_buf)
{
	//write to descriptor
	while (size_buf)
	{
		int sz = ::send(fd, buf, size_buf, 0);
		if (-1 == sz)
		{
			if (EINTR == errno)
			{
				continue; //interrupted by signal
			}
			else
			{
				throw std::runtime_error(::strerror(errno));
			}
		}
		size_buf -= sz;
		buf += sz;
	}
}
