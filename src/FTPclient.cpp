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

#define PORT 10038
#define PAKSIZE 256
#define ACK 0
#define NAK 1

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

  std::stringstream s;
  s << sn;
  s << " ";
  s << seqNum;

  std::stringstream c;
  c << cs << " " << generateCkSum;
  // change to validate based on checksum and sequence number
  cout << endl << "The sn and seq Num are " << s.str() <<  endl;
  if(sn == seqNum) return false;
  cout << endl << "The cs and ckSum are "<< c << endl;
  if(cs != generateCkSum(pk)) return false;
  return true;
}

int main() {
  struct sockaddr_in a;
  struct sockaddr_in ca;
  socklen_t calen = sizeof(ca);
  int rlen;
  int s;
  bool ack;
  seqNum = true;

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

  memset((char *)&a, 0, sizeof(a));
  a.sin_family = AF_INET;
  a.sin_addr.s_addr = htonl(INADDR_ANY);
  a.sin_port = htons(PORT);

  if (bind(s, (struct sockaddr *)&a, sizeof(a)) < 0) {
    cout << "Socket binding failed. (socket s, address a)" << endl;
    return 0;
  }

  /* Loop forever, waiting for messages from a client. */

  cout << "Waiting on port " << PORT << "..." << endl;

  ofstream file("Dumpfile");

  bool isSeqNumSet = false;
  for (;;) {
    unsigned char packet[PAKSIZE + 1];
    unsigned char dataPull[PAKSIZE - 7 + 1];
    rlen = recvfrom(s, packet, PAKSIZE, 0, (struct sockaddr *)&ca, &calen);
    if(!isSeqNumSet) {
      isSeqNumSet = true;
      char * str = new char[1];
      memcpy(str, &packet[0], 1);
      seqNum = atoi(str);
    }
    for(int x = 0; x < PAKSIZE - 7; x++) {
      dataPull[x] = packet[x + 7];
    }
    dataPull[PAKSIZE - 7] = '\0';
    packet[PAKSIZE] = '\0';
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
      cout << ((ack == ACK) ? "ACK" : "NAK") << endl;
      Packet p;
      if (seqNum) {
          createPacket(false, reinterpret_cast<const char *>(dataPull), &p);
      } else {
          createPacket(true, reinterpret_cast<const char *>(dataPull), &p);
      }
      setCkSum(atoi(css), &p);
      setAck(ack, &p);
	
      cout << "dataPull right before being sent through str method" << dataPull << endl;
      if(sendto(s, str(p), PAKSIZE, 0, (struct sockaddr *)&ca, calen) < 0) {
        cout << "Acknowledgement failed. (socket s, acknowledgement message ack, client address ca, client address length calen)" << endl;
        return 0;
      }
      delete css;
    }
  }
  file.close();
}

