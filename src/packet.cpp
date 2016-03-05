/*
 * packet.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: jeremy
 */

#include "packet.h"
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

void setSeqNum(int sn, Packet * packet) {
	packet->seqNum = sn;
}

void setCkSum(int ck, Packet * packet) {
	packet->ckSum = ck;
}

void setAck(int ak, Packet * packet) {
	packet->ack = ak;
}

bool getSeqNum(Packet packet)  {
	return packet.seqNum;
}

int getCkSum(Packet packet) {
	return packet.ckSum;
}

int getAck(Packet packet) {
	return packet.ack;
}

bool ckSum(Packet packet) {
	return packet.ckSum == generateCkSum(packet);
}

int generateCkSum(Packet packet) {
	int cs = 0;
	if(packet.data == NULL) {
		return -1;
	}

	for (int i = 0; i < sizeof(packet.data); i++) {
		if (packet.data[i] == '\0') {
			i == sizeof(packet.data);
			break;
		}
		cs +=  packet.data[i];
	}
	if(cs <= 9999) {
		cs *= 10;
	}

	if(cs > 0)	{
		return cs;
	}
	return -1;

}

void loadData(char* data, Packet* packet) {
	strcpy(packet->data, data);
}

char*  getData(Packet packet) {
	return packet.data;
}

void createPacket(int sn, const char data[249], Packet * packet) {
	packet->seqNum = (sn + 1) % 2;
	strcpy(packet->data, data);
	packet->ckSum = generateCkSum(*packet);
	packet->ack = 0;
}

char* str(Packet packet) {
	std::string tempStr(packet.data);
	std::string packetString;
	std::string csStr;
	
	std::stringstream stream;
	stream << (long long int) packet.ckSum;
	csStr = stream.str();

	while (csStr.length() < 5) csStr += '0';

	std::stringstream snhold;
	
	snhold << (long long int) packet.seqNum;

	std::stringstream ackhold;
	ackhold << (long long int) packet.ack;


	packetString = snhold.str() + csStr + ackhold.str() + tempStr;
	strcpy(packet.packet, packetString.c_str());
	
	cout << endl << "This is the value sent from the str() method" << packet.packet << endl;
	return packet.packet;
}


