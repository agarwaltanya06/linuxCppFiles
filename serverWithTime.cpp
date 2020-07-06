#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include "timestampGen.h"
 
using namespace std;
 
int main()
{
    
    // Creating a socket. Return if can't create socket.   
    int listeningSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSock == -1) return -1; 
 
 
 
    /*Binding (ip address + port) to the socket. 
    sockaddr_in is a predefined STRUCT data type with sin_family,
    sin_port and sin_addr parameters.*/   
    sockaddr_in InputSocketAddress; 
     
    InputSocketAddress.sin_family = AF_INET;
    
    InputSocketAddress.sin_port = htons(54000); 
    //htons: host to network short-> converts 54000 to occupy less bits.
    
    inet_pton(AF_INET, "0.0.0.0", &InputSocketAddress.sin_addr); 
    //inet_pton: an internet function, pointer to string (repr-ing a num?) 
 
    if(bind(listeningSock, (sockaddr*)&InputSocketAddress,sizeof(InputSocketAddress)) == -1) return -2; 
    //2nd arg: ptr to input socket address.

 
    // Use socket for listening.
    if(listen(listeningSock, SOMAXCONN)==-1) return -3;
	
	
 
    // Wait for a connection
    sockaddr_in client;  
    socklen_t clientSize = sizeof(client);
	
 
    int clientSocket = accept(listeningSock, (sockaddr*)&client, &clientSize);
 
    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   //Service(i.e.port) client is connected on
 
    memset(host, 0, NI_MAXHOST); 
    memset(service, 0, NI_MAXSERV);
 
    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
        cout << host << " connected on port " << service << endl;
    
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        //ntop-> networkto string ptr,"have to do manually is else condition"
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }
 
 
 
    // Close listening socket. i.e immediately after creating 1 connection.
    close(listeningSock);
 
 
 
    // While loop: accept and echo message back to client
    char buf[4096];
 
    while (true)
    {
        memset(buf, 0, 4096);
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
	
        
	//printing timestamp after receiving data
        printTimestamp();  
 	
        
        
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }
 
        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            break;
        }


        cout << string(buf, 0, bytesReceived) << endl;

 
        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0); 
        //bytesReceived+1 is 'due to trailing 0 in C'        
    }
 
 
 
    // Close the socket, i.e the till-now connected-to-client socket.
    close(clientSocket);
 
    return 0;
}
