# linuxCppFiles

Using the "port.h" header file to set SERVICE_PORT value for the UDP server, which waits for UDP clients to send data to this port.

Using the "timestamp.h" header file to print current timestamp in the UDP server at the time when it receives some data.

The "udpClient1ConstRate.cpp" and "udpClient2ConstRate.cpp" files send 1K packets, each of a size of 1K bytes.
After sending every 10 packets, they wait for 1 ms before sending the next packet.
Also, they send an identifier for each of the 1K packets.
Hence packets are labelled from 0001 to 1000 along with a specification as to which client (client 1/ client 2) has sent them.

The "udpPaket1KServer.cpp" file accepts packets of a size of 1K bytes and prints the relevant bytes/ characters which indicate packet label number, and which client they have been received from.

