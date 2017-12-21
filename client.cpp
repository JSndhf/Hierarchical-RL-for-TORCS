/***************************************************************************

    file                 : client.cpp
    copyright            : (C) 2007 Daniele Loiacono

		Adjusted by:	Joschka Sondhof, Hamburg University of Applied Sciences, 2017

 ***************************************************************************/
#ifdef WIN32
		#include <WinSock.h>
#else
		#include <netdb.h>
		#include <netinet/in.h>
		#include <unistd.h>
#endif

/*** Experiment config file ***/
#include "hrl_config.h"
/******************************/
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <chrono>
#include __DRIVER_INCLUDE__

/*** defines for UDP *****/
#define UDP_MSGLEN 1000
#define UDP_CLIENT_TIMEUOT 1000000
//#define __UDP_CLIENT_VERBOSE__
//#define __COMMUNICATION_VERBOSE__
/************************/

#ifdef WIN32
		typedef sockaddr_in tSockAddrIn;
		#define CLOSE(x) closesocket(x)
		#define INVALID(x) x == INVALID_SOCKET
		#else
		typedef int SOCKET;
		typedef struct sockaddr_in tSockAddrIn;
		#define CLOSE(x) close(x)
		#define INVALID(x) x < 0
#endif

class __DRIVER_CLASS__;
typedef __DRIVER_CLASS__ tDriver;

using namespace std;

// Parsing the command line arguments
void parse_args(int argc, char *argv[], char *hostName, unsigned int &serverPort, char *id, unsigned int &maxEpisodes,
		  unsigned int &maxSteps, char *trackName, BaseDriver::tstage &stage, unsigned int &mode, char *expFilePath);

int main(int argc, char *argv[]){
    SOCKET socketDescriptor;
    int numRead;

    char hostName[1000];
    unsigned int serverPort;
    char id[1000];
    unsigned int maxEpisodes;
    unsigned int maxSteps;
    char trackName[1000];
    BaseDriver::tstage stage;
		unsigned int mode;
		char expFilePath[1000];

    tSockAddrIn serverAddress;
    struct hostent *hostInfo;
    struct timeval timeVal;
    fd_set readSet;
    char buf[UDP_MSGLEN];


		#ifdef WIN32
		  	/* WinSock Startup */
				WSADATA wsaData={0};
				WORD wVer = MAKEWORD(2,2);
				int nRet = WSAStartup(wVer,&wsaData);

				if(nRet == SOCKET_ERROR){
						std::cout << "Failed to init WinSock library" << std::endl;
						exit(1);
		    }
		#endif

//    parse_args(argc,argv,hostName,serverPort,id,maxEpisodes,maxSteps,trackName,stage);

    parse_args(argc,argv,hostName,serverPort,id,maxEpisodes,maxSteps,trackName,stage, mode, expFilePath);

    hostInfo = gethostbyname(hostName);
    if (hostInfo == NULL){
        cout << "Error: problem interpreting host: " << hostName << "\n";
        exit(1);
    }

		// Print command line option used
		#ifdef HRL_DEBUG
		    cout << "***********************************" << endl;
		    cout << "HOST: "   << hostName    << endl;
		    cout << "PORT: " << serverPort  << endl;
		    cout << "ID: "   << id     << endl;
		    cout << "MAX_STEPS: " << maxSteps << endl;
		    cout << "MAX_EPISODES: " << maxEpisodes << endl;
		    cout << "TRACKNAME: " << trackName << endl;
		    if (stage == BaseDriver::WARMUP)
						cout << "STAGE: WARMUP" << endl;
				else if (stage == BaseDriver::QUALIFYING)
						cout << "STAGE:QUALIFYING" << endl;
				else if (stage == BaseDriver::RACE)
						cout << "STAGE: RACE" << endl;
				else
						cout << "STAGE: UNKNOWN" << endl;
				cout << "TRAINING MODE: " << mode << endl;
				cout << "***********************************" << endl;
		#endif
    // Create a socket (UDP on IPv4 protocol)
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID(socketDescriptor)){
        cerr << "cannot create socket\n";
        exit(1);
    }

		/**** Driver agent gets declared here ***/																	// <----
    tDriver d;
    strcpy(d.trackName,trackName);
    d.stage = stage;

    unsigned long curEpisode=0;

		/**** Initialization routine of driver agent ***/														// <----
		float angles[19];
		d.init(angles, mode, expFilePath);
		if(mode){
				cout << endl << "********** Begin driving **********" << endl;
		} else {
				cout << endl << "********** Begin learning *********" << endl;
		}
		/************** ONLY FOR LEARNING MODE **********************************************/
		if(mode){
				/************** Make sure, all torcs instances currently running are closed *********/
				stringstream ssCmd;
				// First force a server shutdown for the current server
				ssCmd << "pkill torcs";
				system(ssCmd.str().c_str());
				/************** Create as much instances of the torcs game as needed ****************/
				for(int server = HRL_DEFAULT_SERVERPORT; server <= HRL_MAX_SERVERPORT; server++){
						// Build system call for the torcs server
						stringstream ss;
						ss << "torcs -nofuel -nodamage -nolaptime -t 1000000  -r " << HRL_RACECONFIG_BASE << "-" << server << ".xml ";
						#ifndef __COMMUNICATION_VERBOSE__
								ss << ">/dev/null 2>&1 ";		// Surpress output from torcs
						#endif
						ss << "&";	// Immediate return
						system(ss.str().c_str());
				}
				// Give the servers a little head start for this first startup
				usleep(2000000);
		}

		// Mark the beginning of the experiments
		auto experimentStart = std::chrono::system_clock::now();
		// MAIN LOOP while shutdownClient==false && ( (++curEpisode) != maxEpisodes)
    do {
				/*************************** Server definition **********************************/
				// Make sure, the serverPort is inbetween 3001 and 3009
				serverPort = (serverPort > HRL_MAX_SERVERPORT || serverPort < HRL_DEFAULT_SERVERPORT) ? HRL_DEFAULT_SERVERPORT : serverPort;
				// Set some fields in the serverAddress structure.
		    serverAddress.sin_family = hostInfo->h_addrtype;
		    memcpy((char *) &serverAddress.sin_addr.s_addr,
		           hostInfo->h_addr_list[0], hostInfo->h_length);
		    serverAddress.sin_port = htons(serverPort);
				#ifdef __COMMUNICATION_VERBOSE__
						cout << "Start interaction with serverPort: " << serverPort << endl;
				#endif
        /************************ UDP client identification *****************************/
        while(1) {
        		string initString = SimpleParser::stringify(string("init"),angles,19);
						#ifdef __UDP_CLIENT_VERBOSE__
            		cout << "Sending id to server: " << id << endl;
						#endif
            initString.insert(0,id);
						#ifdef __UDP_CLIENT_VERBOSE__
            		cout << "Sending init string to the server: " << initString << endl;
						#endif
            if (sendto(socketDescriptor, initString.c_str(), initString.length(), 0,
                       (struct sockaddr *) &serverAddress,
                       sizeof(serverAddress)) < 0){
                cerr << "cannot send data ";
                CLOSE(socketDescriptor);
                exit(1);
            }

            // wait until answer comes back, for up to UDP_CLIENT_TIMEUOT micro sec
            FD_ZERO(&readSet);
            FD_SET(socketDescriptor, &readSet);
            timeVal.tv_sec = 0;
            timeVal.tv_usec = UDP_CLIENT_TIMEUOT;

            if (select(socketDescriptor+1, &readSet, NULL, NULL, &timeVal)){
                // Read data sent by the solorace server
                memset(buf, 0x0, UDP_MSGLEN);  // Zero out the buffer.
                numRead = recv(socketDescriptor, buf, UDP_MSGLEN, 0);
                if (numRead < 0){
                    cerr << "didn't get response from server...";
                } else {
										#ifdef __UDP_CLIENT_VERBOSE__
                				cout << "Received: " << buf << endl;
										#endif
										if (strcmp(buf,"***identified***")==0)
                    		break;
            		}
	      		}
				// END WHILE: UDP client identification
        }
				#ifdef __COMMUNICATION_VERBOSE__
						cout << "identification done." << endl;
				#endif
				/************************ Continuous incoming data processing *****************/
				int noRespCnt = 0;
        while(1){
            // wait until answer comes back, for up to UDP_CLIENT_TIMEUOT micro sec
            FD_ZERO(&readSet);
            FD_SET(socketDescriptor, &readSet);
            timeVal.tv_sec = 0;
            timeVal.tv_usec = UDP_CLIENT_TIMEUOT;

            if (select(socketDescriptor+1, &readSet, NULL, NULL, &timeVal)){
                // Read data sent by the solorace server
                memset(buf, 0x0, UDP_MSGLEN);  // Zero out the buffer.
                numRead = recv(socketDescriptor, buf, UDP_MSGLEN, 0);
                if (numRead < 0){
                    cerr << "didn't get response from server?";
                    CLOSE(socketDescriptor);
                    exit(1);
                }
								#ifdef __UDP_CLIENT_VERBOSE__
										cout << "Received: " << buf << endl;
								#endif
                /* Compute the action to send to the server or restart if
									 the maximum of simulation steps is reached. 						*/
								/**** Main drivining routine ***/															// <----
								string action = d.drive(string(buf));
								memset(buf, 0x0, UDP_MSGLEN);
								sprintf(buf,"%s",action.c_str());
								/************* SERVER RESTART AND SWITCHING ************************/
								/* A restart is requested from the agent, so restart the current server
									 and switch to the next available one. */
								if(mode && (action.find("(meta 1)") != string::npos)){
										stringstream ss1;
										// First force a server shutdown for the current server
										ss1 << "lsof -i udp:" << serverPort << " | awk 'NR!=1 {print $2}' | xargs kill";
										#ifndef __COMMUNICATION_VERBOSE__
												ss1 << ">/dev/null 2>&1 ";		// Surpress output from torcs
										#endif
										system(ss1.str().c_str());
										// Now open it up again...
										stringstream ss2;
										ss2 << "torcs -nofuel -nodamage -nolaptime -t 1000000 -r " << HRL_RACECONFIG_BASE << "-" << serverPort << ".xml ";
										#ifndef __COMMUNICATION_VERBOSE__
												ss2 << ">/dev/null 2>&1 ";		// Surpress output from torcs
										#endif
										ss2 << "&";	// Immediate return
										system(ss2.str().c_str());
										// ... and don't wait for it but go to the next server
										serverPort++;
										d.onRestart();
										#ifdef __COMMUNICATION_VERBOSE__
												cout << "Client Restart" << endl;
										#endif
										break;
								} else {
										if (sendto(socketDescriptor, buf, strlen(buf)+1, 0,
															 (struct sockaddr *) &serverAddress,
															 sizeof(serverAddress)) < 0){
												cerr << "cannot send data ";
												CLOSE(socketDescriptor);
												exit(1);
										}
										#ifdef __UDP_CLIENT_VERBOSE__
								    else
								    		cout << "Sending " << buf << endl;
										#endif
								}
            } else {
								if(noRespCnt > 2) break;
								#ifdef __COMMUNICATION_VERBOSE__
                		cout << "** Server did not respond in 1 second.\n";
								#endif
								noRespCnt++;
            }
				// END WHILE: Continuous incoming data processing
        }
		// END DO: MAIN LOOP
		} while((++curEpisode) != maxEpisodes);
		// Now calculate and print out the experiment duration
		auto experimentEnd = std::chrono::system_clock::now();
		auto experimentDur = std::chrono::duration_cast<std::chrono::duration<float>>(experimentEnd - experimentStart);
		cout << endl << "********** End of run *************" << endl;
		double secs = experimentDur.count();
		short hh = (short)(secs/3600.0);
		short mm = (short)((secs-hh*3600.0)/60.0);
 		secs = secs - hh*3600.0 - mm*60.0;
		cout << "*** Total time: " << hh << "h " << mm << "m " << setprecision(2) << secs << " sec. ***" << endl;
		cout << "***********************************" << endl << endl;
		// Clean up before leaving
    d.onShutdown();
    CLOSE(socketDescriptor);
		/************** ONLY FOR LEARNING MODE **********************************************/
		if(mode){
				/************** Close all running instances of torcs *******************************/
				system("pkill torcs");
		}
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}

//void parse_args(int argc, char *argv[], char *hostName, unsigned int &serverPort, char *id, unsigned int &maxEpisodes,
//		  unsigned int &maxSteps,bool &noise, char *trackName, BaseDriver::tstage &stage)
void parse_args(int argc, char *argv[], char *hostName, unsigned int &serverPort, char *id, unsigned int &maxEpisodes,
		  unsigned int &maxSteps, char *trackName, BaseDriver::tstage &stage, unsigned int &mode, char *expFilePath){
    int		i;

    // Set default values
    maxEpisodes=HRL_MAX_EPISODES;
    maxSteps=0;
    serverPort=HRL_DEFAULT_SERVERPORT;
    strcpy(hostName,"localhost");
    strcpy(id,"SCR");
    strcpy(trackName,"unknown");
    stage=BaseDriver::UNKNOWN;
		mode=1;
		strcpy(expFilePath, "");

    i = 1;
    while (i < argc){
				if (strncmp(argv[i], "host:", 5) == 0){
		    		sprintf(hostName, "%s", argv[i]+5);
		    		i++;
    		} else if (strncmp(argv[i], "port:", 5) == 0){
		    		sscanf(argv[i],"port:%d",&serverPort);
		    		i++;
    		} else if (strncmp(argv[i], "id:", 3) == 0){
			  		sprintf(id, "%s", argv[i]+3);
			  		i++;
	    	}	else if (strncmp(argv[i], "maxEpisodes:", 12) == 0){
    				sscanf(argv[i],"maxEpisodes:%ud",&maxEpisodes);
    	    	i++;
    		} else if (strncmp(argv[i], "maxSteps:", 9) == 0){
    				sscanf(argv[i],"maxSteps:%ud",&maxSteps);
    				i++;
    		} else if (strncmp(argv[i], "track:", 6) == 0){
    	    	sscanf(argv[i],"track:%s",trackName);
    	    	i++;
    		} else if (strncmp(argv[i], "stage:", 6) == 0){
						int temp;
    		   	sscanf(argv[i],"stage:%d",&temp);
    		   	stage = (BaseDriver::tstage) temp;
    	    	i++;
    	    	if (stage<BaseDriver::WARMUP || stage > BaseDriver::RACE)
								stage = BaseDriver::UNKNOWN;
				} else if (strncmp(argv[i], "mode:",5) == 0){
						sscanf(argv[i],"mode:%ud",&mode);
						i++;
				} else if(strncmp(argv[i], "expFilePath:",12)){
						sprintf(expFilePath, "%s", argv[i]+12);
						i++;
				} else {
    				i++;		/* ignore bad args */
    		}
    }
}
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
