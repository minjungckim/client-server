/* Author: Connie Kim
** CS 432
** Homework 1
** 12 October 2017
**
** This is the client.cpp
** Through arguments given in the cmd line, it opens a new socket to connect 
** with a server. Depending on the "Type" of write given in the 
** argument, it determines the write time and the roundtrip time after receiving
** ack from the server to determine how many times server called read.
*/

#include <sys/types.h> // socket, bind
#include <sys/socket.h> // socket, bind, listen, inet_ntoa
#include <netinet/in.h> // htonl, htons, inet_ntoa
#include <arpa/inet.h> // inet_ntoa
#include <netdb.h> // gethostbyname
#include <unistd.h> // read, write, close
#include <string.h> // bzero
#include <netinet/tcp.h> // SO_REUSEADDR 
#include <sys/uio.h> // writev
#include <sys/time.h> // gettimeofday
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

// Calculate total elapsed time
long elapsedTime(timeval start, timeval lap) {
	return (((lap.tv_sec - start.tv_sec) * 1000000) + (lap.tv_usec - start.tv_usec));
}

int main(int argc, char *argv[]) {

	// Checking number of arguments
	if(argc < 7) {
		perror("Not enough arguments");
		return EXIT_FAILURE;
	}

	// Declare/instantiate with given arguments
	int port = atoi(argv[1]);
	int reps = atoi(argv[2]);
	int nbufs = atoi(argv[3]);
	int bufsize = atoi(argv[4]);
	char *serverIp = argv[5];
	int type = atoi(argv[6]);

	// Checking arguments are correct
	if(nbufs * bufsize != 1500) {
		perror("Make sure nbufs * bufsize equals 1500.");
		return EXIT_FAILURE;
	}
	if(type != 1 && type != 2 && type != 3) {
		perror("Type has to be a number from 1 - 3.");
		return EXIT_FAILURE;
	}

	// Retrieve hostent structure corresponding to this IP name by calling gethostname()
	struct hostent *host = gethostbyname(serverIp);

	// Declare sockaddr_in structure
	sockaddr_in sendSockAddr;
	bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
	sendSockAddr.sin_family = AF_INET;
	sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*) (*host->h_addr_list)));
	sendSockAddr.sin_port = htons(port);

	// Open a stream-oriented socket with the Internet Address family
	int clientSd;
	if((clientSd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error opening client.");
		return EXIT_FAILURE;
	}

	// Connect socket to server by calling connect
	int connectSuccess;
	if((connectSuccess = connect(clientSd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr))) < 0) {
		perror("Couldn't connect with server.");
		return EXIT_FAILURE;
	}

	char databuf[nbufs][bufsize];

	struct timeval start, lap;
	long oneLap, total;
	gettimeofday(&start, NULL); // timer start
	int r = 0;
	while(r < reps) { // type determines write
		if(type == 1) {
			for(int j = 0; j < nbufs; ++j) {
				write(clientSd, databuf[j], bufsize);
			}
		} 
		else if(type == 2) {
			struct iovec vector[nbufs];
			for(int j = 0; j < nbufs; ++j) {
				vector[j].iov_base = databuf[j];
				vector[j].iov_len = bufsize;
			}
			writev(clientSd, vector, nbufs);
		} 
		else write(clientSd, databuf, nbufs * bufsize);
		r++;
	}
	
	gettimeofday(&lap, NULL); // data send
	oneLap = elapsedTime(start, lap);

	int reads = 0;
	read(clientSd, &reads, sizeof(reads));

	gettimeofday(&lap, NULL); // roundtrip
	total = elapsedTime(start, lap);

	cout << "Test: " << type << 
	", Data sending time: " << oneLap << 
	" usec; Total time: " << total <<
	" usec; Reads: " << reads << endl;

	close(clientSd);
	exit(0); // Success
}