/*
 * UDPServer.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: jeremy
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

//int main(int argc, char **argv) {
//	int n, sd;
//	struct sockaddr_in server;
//	char buf[512];
//
//	server.sin_family = AF_INET;
//	server.sin_addr.s_addr = htonl(INADDR_ANY);
//	server.sin_port = htons(12345);
//
//	sd = socket(AF_INET, SOCK_DGRAM, 0);
//
//	bind(sd, (struct sockaddr *)&server, sizeof(server));
//
//	for(;;) {
//		n = recv (sd, buf, sizeof(buf), 0);
//		buf[n] = '\0';
//		printf("Received: %s\n", buf);
//	}
//	close(sd);
//	return 0;
//}


