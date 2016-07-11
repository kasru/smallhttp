#ifndef MY_HTTP_CODES_H
#define MY_HTTP_CODES_H

namespace httpcodes {

enum {
	ok                    = 200,
	bad_request           = 400,
	forbidden             = 403,
	not_found             = 404,
	uri_to_long           = 414,
	internal_server_error = 500,
	not_implemented       = 501,
	service_unavailable	  = 503
};

char const *http_code_to_text(int code);

} //namespace httpcodes

#endif //MY_HTTP_CODES_H
