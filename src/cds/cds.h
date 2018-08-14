#include<iostream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string

using namespace std;
 
#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
#else
  /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
  #include <unistd.h> /* Needed for close() */
#endif

#ifdef _MSC_VER
#pragma comment(lib, "wsock32.lib")
#endif

/**
    TCP Client class
*/
class tcp_client
{
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
     
public:
    tcp_client();
    bool conn();
    bool send_data(string data);
	void disconnect();
    string receive();
};


class CDSClient {
private:
	tcp_client tcpClient;
public:
	CDSClient() {
	}
	~CDSClient() {
		tcpClient.disconnect();
	}
	bool connect() {
		return tcpClient.conn();
	}
	string sendRequest(string str) {
		tcpClient.send_data(str);
		return tcpClient.receive();
	}
	void close() {
		tcpClient.disconnect();
	}

};