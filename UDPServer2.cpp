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
#include <chrono>
#include <ctime>

#define BUFSIZE 2048
#define start 981
#define readSize 19

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
	InputSockAddress.sin_port = htons(SERVICE_PORT_2);

	if (bind(listeningSock, (sockaddr *)&InputSockAddress, sizeof(InputSockAddress)) == -1) return -2;
	

	
	int bytesReceived;					
	unsigned char buf[BUFSIZE];
	sockaddr_in client;	
	socklen_t clientSize = sizeof(client);
	
	//to store time delay between every 10th and 11th packet
	double timeDiff = 0;
	double totalTimeDiff = 0;
	int packetNum;
	
	int reorderingNum = 0;
	int lastPacketNum = 0;


		
	for (;;) //server doesn't close. Keeps waiting on port SERVICE_PORT_2.
	{
		cout<<"waiting on port "<<SERVICE_PORT_2<<"\n";
		auto t1 = std::chrono::high_resolution_clock::now();
		
		bytesReceived = recvfrom(listeningSock, buf, BUFSIZE, 0, (sockaddr *)&client, &clientSize);
		
		auto t2 = std::chrono::high_resolution_clock::now();
		
		printTimestamp();
		
		cout<<"Received "<<bytesReceived<<" bytes. ";

		if(bytesReceived >=start+1)
		{
		       //to print "Packet number received: xxxx from client x"
			buf[bytesReceived] = 0;
			cout<<"Packet number received: ";
			string s = "";
			string s2 = "";
			for(int i=start; i<=(start+readSize-1); i++)
			{
				s.push_back(buf[i]);
				if((i-start)<=3) s2.push_back(buf[i]);
			}
			cout<<s<<"\n\n";
			
			packetNum = stoi(s2);

			if(packetNum!=(lastPacketNum+1)) reorderingNum++;
			lastPacketNum = packetNum;

			auto duration  = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
			if(packetNum!=1) totalTimeDiff+=duration;
			
			if(packetNum!=1 && (packetNum-1)%10==0) 
			{
				timeDiff+=duration;
				
			}
			
			if(packetNum == 1000)
			{
				cout<<"Time parameters: \n\n";
				cout<<"Total delay (due to sleep function calls) = "<<timeDiff<<" ns ("<<timeDiff/1000000<<" ms)\n";
				cout<<"Average delay (in a 1 ms sleep function call) = "<<timeDiff/99<<" ns ("<<timeDiff/99000000<<" ms)\n";
								
				cout<<"Overall total transmission time = "<<totalTimeDiff<<" ns ("<<totalTimeDiff/1000000<<" ms)\n";
				cout<<"Overall average transmission time = "<<totalTimeDiff/999<<" ns ("<<totalTimeDiff/999000000<<" ms)\n\n";
				
				cout<<"Number of packets reordered = "<<reorderingNum<<"\n\n";
				
				
				timeDiff = 0;
				totalTimeDiff = 0;
				lastPacketNum = 0;
				reorderingNum = 0;
			}
		}
	}
}
