/*
 * FTPclient.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: jeremy
 */

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <istream>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <strings.h>
#include <sstream>
#include "packet.h"

//This is the line that displays if there are not 2 parameters input.
#define USAGE "Usage:\r\nc [tux machine number]"
#define PORT 10038
#define PAKSIZE 256
#define BUFSIZE 249
#define ACK 0
#define NAK 1
#define FILENAME "All.txt"

using namespace std;

bool seqNum;

bool isvpack(unsigned char * p) {


	cout << endl << "=== IS VALID PACKET TESTING" << endl;

	char * sns = new char[2];
	memcpy(sns, &p[0], 1);
	sns[1] = '\0';

	char * css = new char[6];
	memcpy(css, &p[1], 6);
	css[5] = '\0';

	char * db = new char[249 + 1];
	memcpy(db, &p[7], 249);
	db[249] = '\0';

	cout << "Seq. num: " << sns << endl;
	cout << "Checksum: " << css << endl;
	cout << "Message: " << db << endl;

	int sn = atoi(sns);
	int cs = atoi(css);

	Packet pk;
	createPacket(0, db, &pk);
	setSeqNum(sn, &pk);

	if(sn != seqNum) return false;
	if(cs != generateCkSum(pk)) return false;
	return true;
}

int main(int argc, char** argv) {


	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t calen = sizeof(server);


	int rlen;
	int s;
	bool ack;
	seqNum = true;

	//If there correct amount of arguement are not input, the method will end.
	if(argc != 2) {
		cout << USAGE << endl;
		return 1;
	}

	//Uses the param (tux number) to append to the known ip of Auburn machine.
	string hs = string("131.204.14.") + argv[1];

	/* Create our socket. */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		cout << "Socket creation failed. (socket s)" << endl;
		return 0;
	}

	/*
	 * Bind our socket to an IP (whatever the computer decides)
	 * and a specified port.
	 *
	 */

	memset((char *)&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(PORT);

	if (bind(s, (struct sockaddr *)&client, sizeof(client)) < 0) {
		cout << "Socket binding failed. (socket s, address a)" << endl;
		return 0;
	}

	//Initialize the client information. the sin_addr is set from the hs string up to along with the
	//tux machine number input in the first parameter.
	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	inet_pton(AF_INET, hs.c_str(), &(server.sin_addr));

	cout << endl;

	//Prints the address of the machine it wants to send it to.
	cout << "Server address (inet mode): " << inet_ntoa(server.sin_addr) << endl;
	cout << "Port: " << ntohs(server.sin_port) << endl;

	cout << endl << endl;

	ofstream file("Corrupt70Loss30.txt");

	char filename[] = FILENAME;

	cout << endl << "Filename sent to Server" << filename << endl;
	if(sendto(s, filename, strlen(FILENAME), 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
		cout << "Package sending failed. (socket s, client address client, message m)" << endl;
		return 0;
	}


	bool isSeqNumSet = false;
	for(;;) {
		unsigned char packet[PAKSIZE + 1];
		unsigned char dataPull[PAKSIZE - 7 + 1];
		//Wait for a message from the client to see if the packet was received correctly
		rlen = recvfrom(s, packet, PAKSIZE, 0, (struct sockaddr *)&server, &calen);

		if(!isSeqNumSet) {
			isSeqNumSet = true;
			char * str = new char[1];
			memcpy(str, &packet[0], 1);
			seqNum = atoi(str);
		}
		for(int x = 0; x < rlen; x++) {
			dataPull[x] = packet[x + 7];
			if(packet[x+7] == '\0') {
				break;
			}
		}
		dataPull[rlen - 7] = '\0';
		packet[rlen] = '\0';
		if (rlen > 0) {
			char * css = new char[6];
			memcpy(css, &packet[1], 6);
			css[5] = '\0';
			cout << endl << endl << "=== RECEIPT" << endl;
			cout << "Seq. num: " << packet[0] << endl;
			cout << "Checksum: " << css << endl;
			cout << "Received message: " << dataPull << endl;
			cout << "Sent response: ";
			if(isvpack(packet)) {
				ack = ACK;
				if (seqNum) {
					seqNum = false;
				}
				else {
					seqNum = true;
				}
				file << dataPull;
			} else {
				ack = NAK;
			}
			if (ack == ACK) {
				cout << "ACK" << endl;
			}
			else {
				cout << "NAK" << endl;
			}
			Packet p;
			if (seqNum) {
				createPacket(false, reinterpret_cast<const char *>(dataPull), &p);
			} else {
				createPacket(true, reinterpret_cast<const char *>(dataPull), &p);
			}
			setCkSum(atoi(css), &p);
			setAck(ack, &p);

			if(sendto(s, str(p), strlen(str(p)), 0, (struct sockaddr *)&server, calen) < 0) {
				cout << "Acknowledgement failed. (socket s, acknowledgement message ack, client address ca, client address length calen)" << endl;
				return 0;
			}
			if(rlen < 256) {
				if(!ack) {
					delete css;
					close(s);
					file.close();
					break;
				}
			}
		}
	}
}

