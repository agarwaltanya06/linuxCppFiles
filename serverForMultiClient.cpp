#include <iostream>
#include <stdio.h>  
#include <string.h>     
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h>   
#include "timestampGen.h"
     
#define TRUE   1  
#define FALSE  0  
#define PORT 8888  
     
using namespace std;

int main()   
{   
    int opt = TRUE;   
    int addrlen, new_socket, client_socket[15],  max_clients = 15, activity, valread, i, sd;   
    int max_sd;   
    struct sockaddr_in address;   
         
    char buffer[1025];  //data buffer of 1K  
         
    //set of socket descriptors  
    fd_set readfds;   
         
    //a message  
    char *message = "ECHO Daemon v1.0 \r\n";   
     
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket  
    int master_socket = socket(AF_INET , SOCK_STREAM , 0);
    if(master_socket == -1)	return -1;  
           
     
    //set master socket to allow multiple connections  
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt,  
          sizeof(opt))<0 )   	return -2; 
    	
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 8888  
    if (bind(master_socket, (sockaddr*)&address, sizeof(address))<0)    
        return -3;  

    cout<<"Listener on port "<<PORT<<"\n";   
         
    //try specifying max of 3 pending connections for the master socket  
    if (listen(master_socket,3) < 0)	return -4;  
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    cout<<"Waiting for connections ..."<<"\n";   
         
    while(TRUE)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //add child sockets to set  
        for (i=0; i < max_clients ;i++)   
        {  
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))	cout<<"select error";      
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            new_socket = accept(master_socket,  
                    (sockaddr *)&address, (socklen_t*)&addrlen);
                    
             if(new_socket < 0)	return -5;  
 
             
       //inform user of socket number - used in send and receive commands  
            cout<<"New connection , socket fd is "<<new_socket<<
            ", ip is :"<<inet_ntoa(address.sin_addr)<<
            ", port : "<<ntohs(address.sin_port) <<"\n"; 
           
            //send new connection greeting message  
            if(send(new_socket, message, strlen(message), 0) != strlen(message))   
                cout<<"Error!"; 
                 
            cout<<"Welcome message sent successfully\n";   
                 
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if(client_socket[i] == 0)   
                {   
                    client_socket[i] = new_socket;   
                    cout<<"Adding to list of sockets as "<<i<<"\n";   
                         
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET(sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                valread = read(sd, buffer, 1024);
                printTimestamp();
                if (valread == 0)   
                {   
                    //Somebody disconnected, get his details and print  
                    getpeername(sd , (sockaddr*)&address , \ 
                        (socklen_t*)&addrlen);   
                    cout<<"Host disconnected , ip"<<
                    inet_ntoa(address.sin_addr)<<", port "<<
                    ntohs(address.sin_port) << "\n";   
                         
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_socket[i] = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';   
                    send(sd, buffer, strlen(buffer), 0 );   
                }   
            }   
        }   
    }   
         
    return 0;   
}   
