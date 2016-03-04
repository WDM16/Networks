/*
 * packet.h
 *
 *  Created on: Mar 3, 2016
 *      Author: jeremy
 */

#ifndef PACKET_H_
#define PACKET_H_


#include <string.h>
#include <stdio.h>
#include <iostream>

struct Packet {
	bool seqNum = 0;
	int ckSum = 0;
	int ack = 0;
	char data[247];
	std::string info;
	std::string temp;
	char packet[256];
};

void setSeqNum(int sn, Packet *);
void setCkSum(int ck, Packet *);
void setAck(int ak, Packet *);

bool getSeqNum(Packet);
int getCkSum(Packet);
int getAck(Packet);

bool ckSum(Packet);
int generateCkSum(Packet packet);

void loadData(char* data, Packet *);
char*  getData(Packet);

void createPacket(int sn, const char data[254], Packet *);

char* str(Packet);

#endif /* PACKET_H_ */
