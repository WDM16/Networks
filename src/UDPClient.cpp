/*
 * UDPClient.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: jeremy
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>

using namespace std;

int main(int argc, char **argv) {
	int sd;
	struct sockaddr_in server;
	struct hostent *hp;

	sd = socket(AF_INET, SOCK_DGRAM, 0);

	server.sin_family = AF_INET;
	server.sin_port = htons(12345);
	hp = gethostbyname(argv[1]);
	bcopy(hp->h_addr, &(server.sin_addr), hp->h_length);

	for(;;) {
		sendto(sd, "HI", 2, 0, (struct sockaddr *) &server, sizeof(server));
		sleep(2);
	}
	close(sd);
	return 0;
}


