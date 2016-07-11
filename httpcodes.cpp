#include "httpcodes.h"

namespace {

struct
{
	int code;
	char const *msg;
} const translation[] =
{
	{httpcodes::ok,                    "OK"},
	{httpcodes::bad_request,           "Bad Request"},
	{httpcodes::forbidden,             "Forbidden"},
	{httpcodes::not_found,             "Not Found"},
	{httpcodes::uri_to_long,           "URI too long"},
	{httpcodes::internal_server_error, "Internal Server Error"},
	{httpcodes::not_implemented,       "Not Implemented"},
	{httpcodes::service_unavailable,   "Service Unavailable"},
	{0,"Unknown Error Code"}
};

} //unnamed manespace

namespace httpcodes {

char const *http_code_to_text(int code) 
{
	int i = 0;
	while(translation[i].code != code && translation[i].code != 0) ++i;
	return translation[i].msg;
}

} //namespace httpcodes
