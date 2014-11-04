/**This program connects to vortex.labs.overthewire.org at port 5842 in order
 * to read in four unsigned ints, add them together, and send them back. This
 * in turn allows for the client to receive the vortex credentials for ssh
 * login. Original prompt is found here 
 * http://overthewire.org/wargames/vortex/vortex0.html. This is adapted heavily
 * from Beej's network programming guide.
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>

#define SERVER "vortex.labs.overthewire.org" // server to conect to
#define PORT "5842" // port of serer
#define MAXDATASIZE 100 // max size of data to accept

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main (void) {
	struct addrinfo hints, *servinfo, *p;
	int sockfd, rv;
	char buf[MAXDATASIZE];
	char s[INET6_ADDRSTRLEN];
	unsigned int a, b, c, d, sum;

	// first, load up address structs with geraddrinfo()
	memset(&hints, 0, sizeof hints); // make sure hints is empty
	hints.ai_family = AF_UNSPEC; // don't care if IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;
	if ((rv = getaddrinfo(SERVER, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// make a socket and loop through all results, connec to 1st possible
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, 
		   p->ai_protocol)) == -1) {
			fprintf(stderr, "socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			fprintf(stderr, "connect");
			continue;
		}
		break;
	}
	
	if (p == NULL) {
		fprintf(stderr, "failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	   s, sizeof s);
	printf("connecting to %s\n", s);
	
	freeaddrinfo(servinfo); // no longer need servinfo struct

	// recieve four unsigned ints
	if(recv(sockfd, (void*)&a, sizeof(unsigned int), 0) == 0) {
		perror("server closed connection\n");
		exit(1);
	}
	if(recv(sockfd, (void*)&b, sizeof(unsigned int), 0) == 0) {
		perror("server closed connection\n");
		exit(1);
	}
	if(recv(sockfd, (void*)&c, sizeof(unsigned int), 0) == 0) {
		perror("server closed connection\n");
		exit(1);
	}
	if(recv(sockfd, (void*)&d, sizeof(unsigned int), 0) == 0) {
		perror("server closed connection\n");
		exit(1);
	}
	printf("recieved: %08x, %08x, %08x, %08x\n", a, b, c, d);
	sum = a + b + c + d;
	printf("sum of above is: %08x\n", sum);

	// send sum
	printf("sending sum...\n");
	if(send(sockfd, (void*)&sum, sizeof(unsigned int), 0) == -1) {
		perror("send failed");
		exit(1);
	}

	// recieve credentials
	recv(sockfd, buf, MAXDATASIZE-1, 0);
	printf("credentials: %s\n", buf); 

	close(sockfd);
	return 0;
	 	
}
