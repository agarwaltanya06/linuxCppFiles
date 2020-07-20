#include <iostream>
#include <sys/types.h>
#include <sys/time.h>
#include <iomanip>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <sstream>
#include <sys/socket.h>
#include "port.h"

#define BUFLEN 2048
#define totalCycles 100
#define packetsPerCycle 10
#define sleepDur 1000

using namespace std;

int main(void)
{
	
	/*
		This client:
		1. Sends 1000 packets to the common server ("udpPacket1KClient2")
		2. Sleeps for 1 ms after sending every 10 packets
	
	*/
	
	
	struct sockaddr_in myaddr, remaddr;
	int fd, slen=sizeof(remaddr);
	char *server = "127.0.0.1";	

	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1) return -1;

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd,(sockaddr *)&myaddr,sizeof(myaddr))<0) return -1;     



	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	if (inet_aton(server, &remaddr.sin_addr)==0) return -1;
	
	
	
	/* creating a 1000 byte packet to send
	Will use last 15 characters/bytes for client number, and the 4 characters/bytes before that for identification of packet number. */
	ostringstream oss;
	for(int i=1; i<=981; i++) oss<<" "; 
	string s1 = oss.str();
	
	
	
	int nextPacket=1; 
	int i;
		
	int  iter = 1; //to count 'totalCycles' number of iterations
		
	
	while(iter<=totalCycles)
	{		
		for(i = nextPacket; i<nextPacket+(packetsPerCycle); i++) //to send 'packetsPerCycle' packets in each iteration
    		{
    			char buf[BUFLEN];
    			
    			string s2;
			s2 = s1;
			
			ostringstream oss2;
			oss2<<setw(4)<<setfill('0')<<i;
	
			s2.append(oss2.str());
			s2.append(" from client 2.");
			s2.resize(BUFLEN-1);
			strcpy(buf, s2.c_str());
			
			cout<<"Sending packet "<<i<<" from client 2 to "<<server<<" port "<<SERVICE_PORT<<"\n";

			if(sendto(fd, buf, strlen(buf),0,(sockaddr *)&remaddr, slen)==-1)
			{
				cout<<"sendto error.";
				return -1;
			}
    		}
    		
    		nextPacket = i;
		iter++;
		
		usleep(sleepDur); //to sleep for 'sleepDur' milliseconds after sending 10 packets
	}
	
	close(fd);
	return 0;
}

	

