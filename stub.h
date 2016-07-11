#ifndef STUB_H
#define STUB_H

#if defined(_WIN32)
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <fcntl.h>
#include <io.h>

inline void sleep(int sec)
{
	Sleep(sec * 1000);
}
#elif defined(UNIX)
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

inline void closesocket(int fd)
{
	close(fd);
}
#else
#error "Use define for platform, please!"
#endif

#include <errno.h>

#endif //STUB_H
