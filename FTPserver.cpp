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
#include <ctime>
#include "packet.h"


//This is the line that displays if there are not 2 parameters input.
#define USAGE "Usage:\r\n [probability of packet corruption in int form] [probability of packet loss in int form]\r\n"
//This is the size of the Packet minus the size of the header.
#define BUFSIZE 249
#define PAKSIZE 256
//The port we will be communicating through
#define PORT 10038
#define ACK 0
#define TIMEOUT 10
#define NAK 1

using namespace std;

bool seqNum;

//The gremlin function is declared in this scope, at the bottom of the page.
bool gremlin(Packet * pack, int corruptProb, int lossProb);

//This method takes 3 arguments. The first is the number of the tux machine, the second is the prob
// of corruption to go in the gremlin function, and the final is the probability of a dropped packet
//for the gremlin function.
int main(int argc, char** argv) {

    //This variable will hold the socket descriptor
    int s = 0;
    int rlen;

    //If there correct amount of arguement are not input, the method will end.
    if (argc != 3) {
        cout << USAGE << endl;
        return 1;
    }

    //Takes the 2 arguments and converts them into integers.
    char * probCorruptStr = argv[1];
    int probCorrupt = atoi(probCorruptStr);
    char * probLossStr = argv[2];
    int probLoss = atoi(probLossStr);

    //Create socket address variable for the client and server.
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t salen = sizeof(client);

    //This may be used for the alternative to the hard coded Auburn addresses.
//    struct hostent *h;

    //If socket is not successful created the program ends.
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cout << "Socket creation failed. (socket " << s << ")" << endl;
        return 0;
    }


    //This initializes the server (computer running this program) information.
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    //INADDR_ANY is meant to get the address of the local machine.
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    //binds this program to the socket.
    if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
        cout << "Socket binding failed. (socket s, address server)" << endl;
        return 0;
    }

    cout << endl;

    /* Loop forever, waiting for messages from a client. */

    cout << "Waiting on port " << PORT << "..." << endl;

    for (;;) {
        char filename[256];
        rlen = recvfrom(s, filename, PAKSIZE, 0, (struct sockaddr *) &client,
                &salen);

        filename[rlen] = '\0';

	cout << endl << "This is the name of the file received" << filename << endl;

        const char * fn = reinterpret_cast<const char *>(filename);
        //Open specific file for reading.
        ifstream is(fn, ios::out);

        //Create a buffer of the packet size without the header to use for reading.
        unsigned char b[BUFSIZE];
        char * file;
        int length;

        //if the file exist, read it into the file parameter for later use.
        if (is) {
            is.seekg(0, is.end);
            length = is.tellg();
            is.seekg(0, is.beg);

            file = new char[length];

            cout << "Reading " << length << " characters..." << endl;
            is.read(file, length);

            if (!is)
                cout << "File reading failed. (filename " << filename
                        << "). Only " << is.gcount() << " could be read.";
            is.close();
        }

        //Makes a string from the file that was read in earlier.
        string fstr = string(file);

        //Prints out that file.
        cout << "File: " << endl << fstr << endl << endl;

        //Initalize the seqNum and dropPacket variables before getting ready to send packets.
        seqNum = true;
        bool dropPck = false;

        //This for loop does the segmentation of the packet. It runs the size of the file divided by the total size
        //of the buffer (249 bytes)
        for (int x = 0; x <= length / BUFSIZE; x++) {
            cout << endl;
            cout << "=== TRANSMISSION START" << endl;
	    

            //gets the next 249 bytes of the file.
            string mstr = fstr.substr(x * BUFSIZE, BUFSIZE);

            //Marks where the file ends with the null character because if the last read went beyond the length of the
            //file length - total already read will give you how much was left to be read.
            if (x * BUFSIZE + BUFSIZE > length) {
                mstr[length - (x * BUFSIZE)] = '\0';
            }

            //Create a packet with the string we just read from our file. The seqNum will alternate
            //between 1 and 2 depending on if the packet gets dropped.
            int startTime = clock();
	    Packet p;
            createPacket(seqNum, mstr.c_str(), &p);
	
	     
            //Send the gremlin the probablities. If the packet gets dropped go to the next packet.
            if ((dropPck = gremlin(&p, probCorrupt, probLoss)) == false) {

                //If the packet does not get dropped send it.
                if (sendto(s, str(p), BUFSIZE + 7, 0,
                        (struct sockaddr *) &client, sizeof(client)) < 0) {
                    cout
                            << "Package sending failed. (socket s, client address client, message m)"
                            << endl;
                    return 0;
                }

            } 
		
	   
	   int curTime = clock();
            //Wait for a message from the client to see if the packet was received correctly
	    
            
	    	    
	   double timeElapsed = (curTime - startTime)/ (double)CLOCKS_PER_SEC*1000;
	   cout << "The current time is: " << (double) timeElapsed << endl; 
            
	    rlen = 0;
	    

	    while(timeElapsed < .001 && rlen == 0){
            rlen = recvfrom(s, b, BUFSIZE + 7, 0, (struct sockaddr *) &client, &salen);
            	timeElapsed = (clock() - startTime) / CLOCKS_PER_SEC;
		cout << "This is in the while loop" << endl; 
	    }
		
	    cout << "The current time is: " << (double) timeElapsed << endl;
	    if(timeElapsed >= 10){
	    	cout << "timeout" << endl;
	    	//Send the gremlin the probablities. If the packet gets dropped go to the next packet.
	    	if ((dropPck = gremlin(&p, probCorrupt, probLoss)) == false) {

	    		//If the packet does not get dropped send it.
	    		if (sendto(s, str(p), BUFSIZE + 7, 0,
	    				(struct sockaddr *) &client, sizeof(client)) < 0) {
	    	                    cout << "Package sending failed. (socket s, client address client, message m)"<< endl;
	    	                    return 0;
	    	        }

	    	 } //else continue;
	    }
            
	    char * ack = new char[3];
            
            b[rlen] = '\0';

            cout << endl << "=== SERVER RESPONSE TEST" << endl;
            cout << "Data: " << b << endl;

            //Check the last spot in the header for the ack or nak.
            if (b[6] == '0')
                ack = (char *) "ACK";
            else
                ack = (char *) "NAK";
            cout << "Response: " << ack << endl;

            //If the file was corrupted.
            if (b[6] != '0') { //if NAK
                /* should say: if chksm(). chksm should be a function both client and server
                 * can see and use that returns a boolean: true if the checksum "checks out"
                 * (no bytes have been tampered with).
                 */

                //Get the sequence number from the message
                char * sns = new char[2];
                memcpy(sns, &b[0], 1);
                sns[1] = '\0';

                //Get the checksum from the message
                char * css = new char[6];
                memcpy(css, &b[1], 6);
		css[5] = '\0';

                //get the data from the received message.
                char * db = new char[BUFSIZE + 1];
                memcpy(db, &b[7], BUFSIZE);
                db[BUFSIZE] = '\0';

                //Print out sequence number and checksum
                cout << "Sequence number: " << sns << endl;
                cout << "Checksum: " << css << endl;

                //Create new packet with old information.
                Packet pk;
                createPacket(0, db, &pk);
                setSeqNum(atoi(sns), &pk);
                setCkSum(atoi(css), &pk);

                //If calculated checksums are the same resend the last packet.
                if (ckSum(pk)) {
                    x--;
                }
                //If they are not the same send last two packets, unless this is the less than the 3rd packet, then
                //start from 0.
                else {
                    if (x - 2 > 0) {
                        x = x - 2;
                    } else {
                        x = 0;
                    }
                }
                delete css;
            }

            //reset the value of b.
            memset(b, 0, BUFSIZE);
        }

    }

    return 0;
}

bool gremlin(Packet *packet, int pCorr, int plost) {
    bool drop = false;
    int r = rand() % 100 + 1;

    if (r <= plost) {
        drop = true;
        cout << "Packet has been dropped." << endl;
    } else if (r <= pCorr) {
        cout << "Packet has been corrupted." << endl;
        int d = rand() % 101;
        if (d <= 70) {
            int c = rand() % 256;
            packet->packet[c] = '\0';
        } else if (d <= 90) {
            int c = rand() % 256;
            int d = rand() % 256;
            packet->packet[c] = '\0';
            while (d == c) {
                d = rand() % 256;
            }
            packet->packet[d] = '\0';
        } else {
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
    } else {
        if (seqNum) {
            seqNum = false;
        } else {
            seqNum = true;
        }
        cout << "Sequence number: " << packet->seqNum << endl;
        cout << "Checksum: " << getCkSum(*packet) << endl;
        cout << "Message: " << getData(*packet) << endl;
    }
    return drop;
}
