#ifdef _MSC_VER
#include "stdafx.h"
#include "utils/util.h"
#endif

#include "cds.h"


 
tcp_client::tcp_client()
{
    sock = -1;
    port = 0;
    address = "";
}
 
/**
    Connect to a host on a certain port number
*/
bool tcp_client::conn()
{
    string address = "127.0.0.1";
    int port = 9090;
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }
         
        LogPrintf("Socket created\n");
    }
    else    {   /* OK , nothing */  }
     
    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;
         
        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            cout<<"Failed to resolve hostname\n";
             
            return false;
        }
         
        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;
 
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];
             
            LogPrintf("%s resolved to %s\n", address, inet_ntoa(*addr_list[i]));             
            break;
        }
    }
     
    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }
     
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
     
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return false;
    }
     
    LogPrintf("Connected\n");
    return true;
}

void tcp_client::disconnect() {
	if (sock!=-1) {
#ifdef _WIN32
		closesocket(sock);
#else
		close(sock);
#endif
	}

	sock=-1;
}
 
/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data)
{
	char size[4]; int l = data.length();
	size[0]=l&0xff; size[1]=(l&0xff00)>>8; size[2]=(l&0xff0000)>>16; size[3]=(l&0xff000000)>>24;
    if( send(sock , size, sizeof(size), 0) < 0)
    {
        perror("send failed");
        return false;
    }

    //Send some data
    if( send(sock , data.c_str() , data.length() , 0) < 0)
    {
        perror("send failed");
        return false;
    }
    LogPrintf("Data send\n");     
    return true;
}

 
/**
    Receive data from the connected host
*/
string tcp_client::receive()
{
	char sizeBytes[4];
    //Receive size
	if( recv(sock , sizeBytes, sizeof(sizeBytes) , 0) != sizeof(sizeBytes))
    {
        throw "Receiving size tag failed";
    }
	int size=sizeBytes[0]+(((int)sizeBytes[1])<<8)+(((int)sizeBytes[2])<<16)+(((int)sizeBytes[3])<<24);
	if (size<0 || size>1024*1024) throw "Received invalid size";
	
    char * buffer = new char[size+1];
    string reply;
     
    //Receive a reply from the server
    if( recv(sock , buffer , size , 0) != size)
    {
        throw "Receiving message body failed";
    }
	buffer[size]=0;
     
    reply = buffer;
    return reply;
}

 