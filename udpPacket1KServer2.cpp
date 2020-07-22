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
#define totalPacketNum 1000

using namespace std;

int main()
{
	
	/*
			This server:
			1. Measures packet reordering within 2 streams
			2. Measures total time taken in receiving each stream's packets, and corresponding average time
			3. Measures total time delay due to sleep function in each stream, and corresponding average time
			4. The function to measure packet reordering across streams was written, but I had calculated something other than what we require. 
	
	*/
	
	
	
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
	
	double timeDiff1 = 0;            //measures time lag due to sleep function in client 1
	double totalTimeDiff1 = 0;	 //measures overall time taken to receive client 1 packets
	double timeDiff2 = 0;		 //measures time lag due to sleep function in client 2
	double totalTimeDiff2 = 0;       //measures overall time taken to receive client 2 packets
	
	int packetNum;
	int clientNum;
//	int lastClient = -1;  //last client (1 or 2) from which data has been received
//	int overallReorder=0; 
//	int client1Over = -1; //is '1' when all client 1 data has been received
//	int client2Over = -1; //is '1' when all client 2 data has been received 
	
	
	int reorder1 = 0;    //reordering in client 1 packets
	int reorder2 = 0;    //reordering in client 2 packets
	int lastPacket1 = 0;  //last packet received from client 1
	int lastPacket2 = 0;  //last packet received from client 2


		
	for (;;) //server doesn't close. Keeps waiting on port SERVICE_PORT.
	{
		cout<<"waiting on port "<<SERVICE_PORT<<"\n";
		
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
			string s3 = "";
			
			for(int i=start; i<=(start + readSize -1 ); i++)    //reading data received, from starting index of relevant data to end
			{
				s.push_back(buf[i]);
				if((i-start)<=3) s2.push_back(buf[i]);      //first 4 bytes contain packet number in xxxx format
				if(i==start+readSize-2) s3.push_back(buf[i]);  //second-last byte contains client number
			}
			cout<<s<<"\n\n";
			
			packetNum = stoi(s2);
			clientNum = stoi(s3);
			
/*			
			//Measuring overall reordering between flows
			if(lastClient == -1) lastClient = clientNum;
			else if(lastClient!=clientNum) overallReorder++;
			lastClient = clientNum; 
*/			
			
			//Measuring reordering within each flow
			if(clientNum == 1){
				if(lastPacket1!=(packetNum-1)) reorder1++;
				lastPacket1 = packetNum;
			}
					
			else if(clientNum==2){
				if(lastPacket2!=(packetNum-1)) reorder2++;
				lastPacket2 = packetNum;
			}
			

			
			//measure of time taken to receive this packet
			auto duration  = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
			
			
			//adding to total time taken to receive all packets
			if(packetNum!=1) 
			{
				if(clientNum == 1) totalTimeDiff1+=duration;
				else totalTimeDiff2+=duration;
			}
			
			
			//adding to total time delay due to sleep function
			if(packetNum!=1 && (packetNum-1)%10==0) 
			{
				if(clientNum == 1) timeDiff1+=duration;
				else timeDiff2+=duration;
				
			}
			
			
			//if all packets have been received from either client 1 or client 2
			if(packetNum == totalPacketNum)
			{
				cout<<"Time parameters ";
				
				
				//Printing data for client 1 if all its data has been received
				if(clientNum == 1) {
				cout<<"(for client 1):\n";
				cout<<"Total delay (due to sleep function calls) = "<<timeDiff1<<" ns ("<<timeDiff1/1000000<<" ms)\n";
				cout<<"Average delay (in a 1 ms sleep function call) = "<<timeDiff1/99<<" ns ("<<timeDiff1/99000000<<" ms)\n";
								
				cout<<"Overall total transmission time = "<<totalTimeDiff1<<" ns ("<<totalTimeDiff1/1000000<<" ms)\n";
				cout<<"Overall average transmission time = "<<totalTimeDiff1/999<<" ns ("<<totalTimeDiff1/999000000<<" ms)\n\n";
					
				cout<<"Reordering in client 1: "<<reorder1<<"\n\n";
					
				
				//Resetting relevant variables
				timeDiff1 = 0;
				totalTimeDiff1 = 0;
				reorder1 = 0;
			//	client1Over = 1;
				
				}
				
				
				
				//Printing data for client 2 if all its data has been received
				else if(clientNum == 2) {
				cout<<"(for client 2): \n";
				
				cout<<"Total delay (due to sleep function calls) = "<<timeDiff2<<" ns ("<<timeDiff2/1000000<<" ms)\n";
				cout<<"Average delay (in a 1 ms sleep function call) = "<<timeDiff2/99<<" ns ("<<timeDiff2/99000000<<" ms)\n";
								
				cout<<"Overall total transmission time = "<<totalTimeDiff2<<" ns ("<<totalTimeDiff2/1000000<<" ms)\n";
				cout<<"Overall average transmission time = "<<totalTimeDiff2/999<<" ns ("<<totalTimeDiff2/999000000<<" ms)\n\n";
				
				cout<<"Reordering in client 2: "<<reorder2<<"\n\n";
					
				
				//Resetting relevant variables	
				timeDiff2 = 0;
				totalTimeDiff2 = 0;
				reorder2 = 0;
			//	client2Over = 1;
				}
				
				
				//Printing overall reordering if all data has been received from both clients
			/*	if(client1Over==1 && client2Over==1){
				cout<<"\nOverall reordering: "<<overallReorder<<"\n";
				overallReorder = 0;
				client1Over = -1;
				client2Over = -1;
				
				} */

			}
		}
	}
}
