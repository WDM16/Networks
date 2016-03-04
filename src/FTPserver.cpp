/*
 * FTPserver.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: jeremy
 */

#include <iostream>
#include "packet.h"

using namespace std;

bool seqNum;


bool gremlin(Packet * pack, int corruptProb, int lossProb);

bool gremlin(Packet *packet, int pCorr, int plost) {
	bool drop = false;
	int r = rand() % 101;

	if (r <= plost) {
		drop = true;
		cout << "Packet has been dropped." << endl;
	}
	else if(r <= pCorr) {
		cout << "Packet has been corrupted." << endl;
		int d = rand() % 101;
		if(d <= 70) {
			int c = rand() % 256;
			packet->packet[c] = NULL;
		}
		else if (d <= 90) {
			int c = rand() % 256;
			int d = rand() % 256;
			packet->packet[c] = NULL;
			while (d == c) {
				d = rand() % 256;
			}
			packet->packet[d] = NULL;
		}
		else {
			int c = rand() % 256;
			int d = rand() % 256;
			int e = rand() % 256;
			packet->packet[c] = NULL;
			while (d == c) {
				d = rand() % 256;
			}
			packet->packet[d] = NULL;
			while (e == c || e == d) {
				e = rand() % 256;
			}
			packet->packet[e] = NULL;
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


