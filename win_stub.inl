#ifndef WIN_STUB_H
#define WIN_STUB_H

#ifdef _WIN32
namespace {

class wsa_init
{
public:
	wsa_init()
	{
		WSADATA wsaData;
		::WSAStartup(MAKEWORD(2,2), &wsaData);
	}
};
wsa_init wsa;

} //unnamed namespace
#endif //_WIN32

#endif //WIN_STUB_H
