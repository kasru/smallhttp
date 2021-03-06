#include "stub.h"
#include "myhttp.h"

#include "mylib/cast_to.hpp"

#include <iostream>
#include <strstream>
#include <functional>
#include <algorithm>

#include <limits.h>

#include "win_stub.inl"

#define BUFLEN 512000
#define DEFAULT_FILE_NAME "index.html"

/*static*/ int connection::get_last_http_error(int last_errno)
{
	switch(last_errno)
	{
	case EACCES:
		return httpcodes::forbidden;
	case EFAULT:
	case ENOTDIR:
	case ENOENT:
		return httpcodes::not_found;
	case EIO:
	case EMFILE:
	case ENOMEM:
	case EAGAIN:
		return httpcodes::service_unavailable;
	case ENAMETOOLONG:
	default:
		return httpcodes::internal_server_error;
	}
}

connection::connection()
	: m_req_str(BUFLEN)
	, m_last_http_code(httpcodes::ok)
	, m_request_len(0)
{
}

void connection::accept(mysocket const & server_socket)
{
	server_socket.accept(m_socket);
}

void connection::process()
{
	read_request();
	parse_request();
	make_body();
	make_header();
	send_header();
	if (is_get())
	{
		send_body();
	}
	m_socket.close();
}

void connection::read_request()
{
	m_request_len = m_socket.recv(m_req_str);
	m_request.assign(m_req_str.begin(),m_req_str.begin() + m_request_len);
}

void connection::parse_request()
{
	read_method_uri_protocol();
	m_last_http_code = check_method_uri_protocol();
}

void connection::read_method_uri_protocol()
{
	char const rn[]  = "\r\n";
	char const space[] = " \t";

	std::string line;
	//read first line
	std::string::iterator it = std::search(m_request.begin(), m_request.end(), rn, rn+sizeof(rn)-1);
	if (it != m_request.end())
	{
		line.assign(m_request.begin(), it);
	}
	if (!line.empty())
	{
		//read method
		it = std::find_first_of(line.begin(), line.end(), space, space+sizeof(space)-1);
		if (it != line.end())
		{
			m_method.assign(line.begin(), it);
			//read uri
			std::string::iterator it_next = it+1;
			it = std::find_first_of(it_next, line.end(), space, space+sizeof(space)-1);
			if (it != line.end())
			{
				m_uri.assign(it_next, it);
				//read protocol
				it_next = it+1;
				it = std::find_first_of(it_next, line.end(), space, space+sizeof(space)-1);
				m_protocol.assign(it_next, it);
				//TODO: parse protocol version
			}
		}
	}
}

//URL Encoding not emplemented RFC1738
int connection::check_method_uri_protocol()
{
	if (m_method.empty() || m_uri.empty() || m_protocol.empty())
	{
		return httpcodes::forbidden;
	}
	char const bad_sym[] = "<>\"%{}|^~[]`\\;:/?@#=&";
	std::string::iterator it = std::search(m_uri.begin(), m_uri.end(), bad_sym, bad_sym+sizeof(bad_sym)-1);
	if (it != m_uri.end())
	{
		return httpcodes::forbidden;
	}
	if (m_uri.find("..") != std::string::npos)
	{
		//URI contains ..
		return httpcodes::forbidden;
	}
	if (!is_get() && !is_head())
	{
		//Unsupported method
		return httpcodes::not_implemented;
	}
	if (m_uri.size() > MAX_PATH)
	{
		return httpcodes::uri_to_long;
	}
	if ('/' == *m_uri.begin())
	{
		m_full_file_name.assign(m_uri.begin()+1, m_uri.end());
	}
	if (m_full_file_name.empty())
	{
		m_full_file_name = DEFAULT_FILE_NAME;
	}
	int res = ::stat(m_full_file_name.c_str(), &m_file_stat);
	if (res < 0)
	{
		int last_errno = errno;
		return get_last_http_error(last_errno);
	}
	m_file.open(m_full_file_name.c_str(), std::ifstream::in);
	if (!m_file)
	{
		return httpcodes::not_found;
	}
	if (m_file.fail())
	{
		return httpcodes::forbidden;
	}
	if (m_file.bad())
	{
		return httpcodes::service_unavailable;
	}
	return httpcodes::ok;
}

//RFC1123 date
/*static*/ std::string connection::make_http_date(time_t const when /*= time(0)*/)
{
	time_t when_copy = when;
	char buffer[30]; // strlen("Fri, 28 Feb 2003 00:02:08 GMT")+1
	::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&when_copy));
	return buffer;
}

void connection::read_file()
{
	m_file.unsetf(std::ios_base::skipws);
	m_body.assign(std::istreambuf_iterator<char>(m_file), std::istreambuf_iterator<char>());
}

void connection::make_error_body()
{
	m_file_stat.st_mtime = time(0);
	m_body = "<html><head><title>";
	m_body += mylib::cast_to<std::string>(m_last_http_code);
	m_body += " ";
	m_body += httpcodes::http_code_to_text(m_last_http_code);
	m_body += "</title></head>\n<body>\n<h1>";
	m_body += httpcodes::http_code_to_text(m_last_http_code);
	m_body += "</h1>\n<hr>Generated by smallhttp on ";
	m_body += make_http_date(m_file_stat.st_mtime);
	m_body += "\n</body></html>\n";
}

void connection::make_body()
{
	if (httpcodes::ok != m_last_http_code)
	{
		make_error_body();
	}
	else
	{
		read_file();
	}
}

void connection::make_header()
{
	m_header = "HTTP/1.1 200 OK\r\n";
	m_header += "Server: smallhttp/1.0\r\n";
	m_header += "Date: ";
	m_header += make_http_date();
	m_header += "\r\n";
	m_header += "Last-Modified: ";
	m_header += make_http_date(m_file_stat.st_mtime);
	m_header += "\r\n";
	m_header += "Content-Type: text/html\r\n";
	m_header += "Content-Length: ";
	m_header += mylib::cast_to<std::string>(m_body.size());
	m_header += "\r\n";
	m_header += "Connection: close\r\n";
	m_header += "\r\n";
}

void connection::send_header()
{
	m_socket.send(m_header);
}

void connection::send_body()
{
	m_socket.send(m_body);
}

class thread_func
{
public:
	enum thread_type {control, worker};
	thread_func(myhttp * http, thread_type tt) : m_http(http), m_type(tt)
	{
	}
	int operator ()() const
	{
		int ret = 0;
		if (control == m_type)
		{
			ret = m_http->control_func();
		}
		else if (worker == m_type)
		{
			ret = m_http->worker_func();
		}
		return ret;
	}
private:
	myhttp * m_http;
	thread_type m_type;
};

myhttp::myhttp(unsigned short port /*= 80*/)
	: m_port(port)
{
	m_server.create(port);
	m_server.listen();
}

#define WORKER_THREADS 5
void myhttp::run()
{
	thread_func control(this, thread_func::control);
	m_threads.create_thread(control);
	for (int i = 0; i != WORKER_THREADS; ++i)
	{
		thread_func worker(this, thread_func::worker);
		m_threads.create_thread(worker);
	}
	m_threads.join_all();
}

int myhttp::control_func() 
{
	while(true)
	{
		try
		{
			//TODO: stop all workers and any control for workers
			//control signal: SIGPIPE, SIGINT, SIGTERM
			::sleep(1);
		}
		catch (std::exception const & ex)
		{
			std::cerr << "myhttp listen_func error: " << ex.what() << std::endl;
		}
	} //while(true)
	return 0;
}

int myhttp::worker_func()
{
	while(true)
	{
		try
		{
			connection con;
			con.accept(m_server);
			con.process();
		}
		catch (std::exception const & ex)
		{
			std::cerr << "myhttp worker_func error: " << ex.what() << std::endl;
		}
	} //while
	return 0;
}
