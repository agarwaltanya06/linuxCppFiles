#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "port.h"
#include "timestampGen.h"

#define BUFSIZE 2048

using namespace std;

int main()
{
				
    	int listeningSock = socket(AF_INET, SOCK_DGRAM, 0);
    	if (listeningSock == -1) return -1; 
	

	//to bind the socket to any valid IP address and a specific port 

	sockaddr_in InputSockAddress;	
	memset((char*)&InputSockAddress, 0, sizeof(InputSockAddress));
	InputSockAddress.sin_family = AF_INET;
	InputSockAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	InputSockAddress.sin_port = htons(SERVICE_PORT);

	if (bind(listeningSock, (sockaddr *)&InputSockAddress, sizeof(InputSockAddress)) == -1) return -2;
	

	
	int bytesReceived;					
	unsigned char buf[BUFSIZE];
	sockaddr_in client;	
	socklen_t clientSize = sizeof(client);
		
	for (;;) //server doesn't close. Keeps waiting on port SERVICE_PORT.
	{
		cout<<"waiting on port "<<SERVICE_PORT<<"\n";
		bytesReceived = recvfrom(listeningSock, buf, BUFSIZE, 0, (sockaddr *)&client, &clientSize);
		printTimestamp();
		cout<<"Received "<<bytesReceived<<" bytes. ";

		if(bytesReceived >=982)
		{
		       //to print "Packet number received: xxxx from client x"
			buf[bytesReceived] = 0;
			cout<<"Packet number received: ";
			string s = " ";
			for(int i=981; i<=999; i++)
			{
				s.push_back(buf[i]);
			}
			cout<<s<<"\n\n";
		}
	}
}
