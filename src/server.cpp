/* Author: Connie Kim
** CS 432
** Homework 1
** 12 October 2017
**
** This is the server.cpp
** Through two arguments given in the cmd line, it opens a new socket to connect 
** with a client. Once connection is successful, socket is then changed to an async func.
** It first sends the read calls made as the ack, and then determines the total 
** time it took to read.
*/

#include <sys/types.h> // socket, bind
#include <sys/socket.h> // socket, bind, listen, inet_ntoa
#include <netinet/in.h> // htonl, htons, inet_ntoa
#include <arpa/inet.h> // inet_ntoa
#include <netdb.h> // gethostbyname
#include <unistd.h> // read, write, close
#include <string.h> // bzero
#include <netinet/tcp.h> // SO_REUSEADDR 
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h> // gettimeofday
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int BUFFSIZE = 1500;
int serverSd, newSd, reps;

long elapsedTime(timeval start, timeval lap) {
	return (((lap.tv_sec - start.tv_sec) * 1000000) + (lap.tv_usec - start.tv_usec));
}

void readFromClient(int signalInterrupt) {

		char dataBuff[BUFFSIZE];
		struct timeval start, stop;
		gettimeofday(&start, NULL);

		int r = 0, reads = 0;
		while(r < reps) {
			for(int nRead = 0; (nRead += read(newSd, dataBuff, BUFFSIZE - nRead)) < BUFFSIZE; ++reads);
			r++;
		}

		gettimeofday(&stop, NULL);

		long readTime = elapsedTime(start, stop);
		cout << "Data receiving time: " << readTime << " usec" << endl;

		write(newSd, &reads, sizeof(reads)); // send count of reads	
		
		close(newSd);
		exit(0);
}

int main(int argc, char *argv[]) {

	if(argc < 3) {
		perror("Not enough arguments.");
		return EXIT_FAILURE;
	}

	// Instantiate port and repetition with given args
	int port = atoi(argv[1]);
	reps = atoi(argv[2]); 

	sockaddr_in acceptSockAddr;
	bzero((char*)&acceptSockAddr, sizeof(acceptSockAddr));
	acceptSockAddr.sin_family = AF_INET;
	acceptSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	acceptSockAddr.sin_port = htons(port);

	serverSd = socket(AF_INET, SOCK_STREAM, 0);
	if((serverSd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error opening server.");
		return EXIT_FAILURE;
	}

	const int on = 1;
	setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int));

	int bindSuccess;
	if((bindSuccess = bind(serverSd, (sockaddr*)&acceptSockAddr, sizeof(acceptSockAddr))) < 0) {
		perror("Error binding address to server.");
		close(serverSd);
		return EXIT_FAILURE;
	}

	listen(serverSd, 5);
	sockaddr_in newSockAddr;
	socklen_t newSockAddrSize = sizeof(newSockAddr);
	
	if((newSd = accept(serverSd, (sockaddr*)&newSockAddr, &newSockAddrSize)) < 0) {
		perror("Error with connection.");
		return EXIT_FAILURE;
	}

    signal(SIGIO, readFromClient);
	fcntl(newSd, F_SETOWN, getpid());
	fcntl(newSd, F_SETFL, FASYNC);

	for(;;) pause(); // Sleep forever

	close(serverSd);
	return 0;
}