#ifndef MY_HHTP_H
#define MY_HHTP_H

#include "mylib/thread.hpp"
#include "mysocket.h"
#include "httpcodes.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <ctime>

#include <sys/stat.h>

class http_exception : public std::runtime_error
{
public:
	explicit http_exception(std::string const & what)
		: std::runtime_error(what)
	{}
	virtual ~http_exception() throw() {}
};

class connection
{
public:
	static int get_last_http_error(int last_errno);
	static std::string make_http_date(time_t when = time(0));

	connection();
	void accept(mysocket const & server_socket);
	void process();

	int last_http_code() const { return m_last_http_code; }

private:
	void read_request();
	void parse_request();
	void read_method_uri_protocol();
	int check_method_uri_protocol();
	void make_header();
	void read_file();
	void make_error_body();
	void make_body();
	void send_header();
	void send_body();

	bool is_get() const { return "GET" == m_method; }
	bool is_head() const { return "HEAD" == m_method; }

	mysocket m_socket;

	std::vector<char> m_req_str;

	std::string m_request;
	std::string m_method;
	std::string m_uri;
	std::string m_protocol;
	std::string m_header;
	std::string m_body;

	std::string m_full_file_name;

	int m_last_http_code;
	int m_request_len;

	std::ifstream m_file;
	struct stat m_file_stat;
};

class thread_func;
class myhttp : private mylib::noncopyable
{
public:
	explicit myhttp(unsigned short port = 80);
	void run();

private:
	friend class thread_func;

	int control_func();
	int worker_func();

	unsigned short m_port;
	mysocket m_server;
	mylib::thread_group m_threads;
};

#endif //MY_HHTP_H
