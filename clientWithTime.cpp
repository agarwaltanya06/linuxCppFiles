#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <chrono>
#include <ctime>

using namespace std;

int main()
{
    //	Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) return -1;
    
    
    
    //	Create a hint structure for the server we're connecting with
    int port = 54000;
    string ipAddress = "127.0.0.1"; //loopback IP because using same machine

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &serverAddress.sin_addr);



    //Connect created client socket to the server(defined by serverAddress)
    int connectRes = connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connectRes == -1) return -1;



    //	While loop:
    char buf[4096];
    string userInput;


    do {

        //Enter lines of text
        cout << "> ";
        getline(cin, userInput);


        //Send to server
	auto t1 = std::chrono::high_resolution_clock::now();
        int sendRes = send(sock,userInput.c_str(), userInput.size() + 1, 0);
        auto t2 = std::chrono::high_resolution_clock::now();
        if (sendRes == -1)
        {
            cout << "Could not send to server! Whoops!\r\n";
            continue;
        }
        

        //Wait for response
        memset(buf, 0, 4096);
        auto t3 = std::chrono::high_resolution_clock::now();
        int bytesReceived = recv(sock, buf, 4096, 0);
	auto t4 = std::chrono::high_resolution_clock::now();
        if (bytesReceived == -1)
        {
            cout << "There was an error getting response from server\r\n";
        }
        
        else
        {
            //Display response
            cout << "SERVER> " << string(buf, bytesReceived) <<"\r\n";
        }
        
        
        auto duration1 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
        
        auto duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t4-t3).count();
        
        cout<<"Time taken to send: ";
        std::cout<<duration1<<"\r\n";
        cout<<"Time taken to recv: ";
        std::cout<<duration2<<"\r\n";
    } while(true);



    //	Close the socket
    close(sock);

    return 0;
}
