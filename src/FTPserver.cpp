/*
 * FTPserver.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: jeremy
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <istream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "packet.h"

#define USAGE "Usage:\r\nc [tux machine number] [probability of packet corruption in int form] [probability of packet loss in int form]\r\n"
#define BUFSIZE 249
#define FILENAME "testfile.txt"

using namespace std;

bool seqNum;


bool gremlin(Packet * pack, int corruptProb, int lossProb);


int main(int argc, char** argv) {

	int s = 0;

	if(argc != 4) {
		cout << USAGE << endl;
		return 1;
	}

	char * probCorruptStr = argv[2];
	int probCorrupt = atoi(probCorruptStr);
	char * probLossStr = argv[3];
	int probLoss = atoi(probLossStr);

	string hs = string("131.204.14.") + argv[1];
	short int port = 10038; /* Can be any port within 10038-10041, inclusive. */

	ifstream is(FILENAME, ios::out);

	unsigned char b[BUFSIZE];
	char * file;
	int length;

	if(is) {
	    is.seekg(0, is.end);
	    length = is.tellg();
	    is.seekg(0, is.beg);

	    file = new char[length];

	    cout << "Reading " << length << " characters..." << endl;
	    is.read(file, length);

	    if(!is) cout << "File reading failed. (filename " << FILENAME << "). Only " << is.gcount() << " could be read.";
	    is.close();
	}

	struct sockaddr_in client;
	struct sockaddr_in server;
	socklen_t salen = sizeof(server);
	struct hostent *h;

	string m = string("Hello, server world! I'm gonna talk for a long long time and see if this works. Maybe \r\n")
	      + "it'll properly tell me there are more bytes; maybe it won't. Either way I'll be proud of the work I've done. \r\n"
	      + "Also, Patrick is gay.";

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		cout << "Socket creation failed. (socket " << s << ")" << endl;
	    return 0;
	}

	memset((char *)&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(10038); //why does this always give us 0?
	client.sin_port = htons(10038);

	if (bind(s, (struct sockaddr *)&client, sizeof(client)) < 0){
	    cout << "Socket binding failed. (socket s, address client )" << endl;
	    return 0;
	}

	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, hs.c_str(), &(server.sin_addr));

	cout << endl;

	cout << "Server address (inet mode): " << inet_ntoa(server.sin_addr) << endl;
	cout << "Port: " << ntohs(server.sin_port) << endl;

	cout << endl << endl;
	string fstr = string(file);

	cout << "File: " << endl << fstr << endl << endl;

	  seqNum = true;
	  bool dropPck = false;
	  for(int x = 0; x <= length / BUFSIZE; x++) {
	    cout << endl;
	    cout << "=== TRANSMISSION START" << endl;
	    string mstr = fstr.substr(x * BUFSIZE, BUFSIZE);
	    if(x * BUFSIZE + BUFSIZE > length) {
	      mstr[length - (x * BUFSIZE)] = '\0';
	    }
	    Packet p;
	    createPacket(seqNum, mstr.c_str(), &p);

	    if((dropPck = gremlin(&p, probCorrupt, probLoss)) == false){
	      if(sendto(s, str(p), BUFSIZE + 7, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
	        cout << "Package sending failed. (socket s, server address sa, message m)" << endl;
	        return 0;
	      }
	    } else continue;

	    recvfrom(s, b, BUFSIZE + 7, 0, (struct sockaddr *)&server, &salen);
	    char * ack = new char[3];

	    cout << endl << "=== SERVER RESPONSE TEST" << endl;
	    cout << "Data: " << b << endl;
	    if(b[6] == '0') ack = (char *)"ACK";
	    else ack = (char *)"NAK";
	    cout << "Response: " << ack << endl;

	    if(ack == "NAK") { //if NAK
	      /* should say: if chksm(). chksm should be a function both client and server
	       * can see and use that returns a boolean: true if the checksum "checks out"
	       * (no bytes have been tampered with).
	      */

	      char * sns = new char[2];
	      memcpy(sns, &b[0], 1);
	      sns[1] = '\0';

	      char * css = new char[5];
	      memcpy(css, &b[1], 5);

	      char * db = new char[BUFSIZE + 1];
	      memcpy(db, &b[2], BUFSIZE);
	      db[BUFSIZE] = '\0';

	      cout << "Sequence number: " << sns << endl;
	      cout << "Checksum: " << css << endl;

	      Packet pk;
	      createPacket(0, db, &pk);
	      setSeqNum(atoi(sns), &pk);
	      setCkSum(atoi(css), &pk);

	      if(ckSum(pk)) {
	          x--;
	      }
	      else {
	          if(x - 2 > 0) {
	              x = x - 2;
	          }
	          else {
	              x = 0;
	          }
	      }
	    }

	    memset(b, 0, BUFSIZE);
	  }
	return 0;
}


bool gremlin(Packet *packet, int pCorr, int plost) {
	srand(time(NULL));
	bool drop = false;
	int r = rand() % 100 + 1;

	if (r <= plost) {
		drop = true;
		cout << "Packet has been dropped." << endl;
	}
	else if(r <= pCorr) {
		cout << "Packet has been corrupted." << endl;
		int d = rand() % 101;
		if(d <= 70) {
			int c = rand() % 256;
			packet->packet[c] = '\n';
		}
		else if (d <= 90) {
			int c = rand() % 256;
			int d = rand() % 256;
			packet->packet[c] = '\0';
			while (d == c) {
				d = rand() % 256;
			}
			packet->packet[d] = '\0';
		}
		else {
			int c = rand() % 256;
			int d = rand() % 256;
			int e = rand() % 256;
			packet->packet[c] = '\0';
			while (d == c) {
				d = rand() % 256;
			}
			packet->packet[d] = '\0';
			while (e == c || e == d) {
				e = rand() % 256;
			}
			packet->packet[e] = '\0';
		}
	}
	else {
		if(seqNum) {
			seqNum = false;
		}
		else {
			seqNum = true;
		}
		cout << "Sequence number: " << getSeqNum(*packet) << endl;
		cout << "Checksum: " << getCkSum(*packet) << endl;
		cout << "Message: "  << getData(*packet) << endl;
	}
	return drop;
}


