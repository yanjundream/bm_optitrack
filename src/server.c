/*
    Simple udp server
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<math.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include "bm_pose2d.h"
//#include "server.h"
 
#define BUFLEN 3000  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data


void die(char *s)
{
    perror(s);
    exit(1);
}

void DB_unpack(char *pData);
void close(int s);

pose2d pose_opt[20];
thread_stop=0;

void server()
{

	//printf("Build the server\n");
	struct sockaddr_in si_me, si_other;

	int s, slen = sizeof(si_other) , recv_len;
	char buf[BUFLEN];

	//create a UDP socket

	socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	bind(s , (struct sockaddr*)&si_me, sizeof(si_me) );
	



	//  printf("Waiting for data...");
	fflush(stdout);
	 
	//try to receive some data
	recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
	 
	//print details of the client/peer and the data received
	//printf("Received packet from %s:\n", inet_ntoa(si_other.sin_addr));
	DB_unpack(buf);       

    close(s);
    return;
}

void DB_unpack(char* pData)
{
	//printf("Begin Packet Rigid Body Data\n-------\n");

	char* ptr = pData;
	int framenumber = 0;
	memcpy(&framenumber,ptr,4);
	//printf("@unpack_dataTrans framenumber is : \n", framenumber);
	ptr += 4;

	int nMarkerSets = 0;
	memcpy(&nMarkerSets, ptr, 4);
	ptr += 4;
	//printf("@unpack_dataTransMarker Set Count : %d\n", nMarkerSets);

	// rigid bodies
	int nRigidBodies = 0;
	memcpy(&nRigidBodies, ptr, 4);
	ptr += 4;
	//printf("@unpack_dataTransRigid Body Count : %d\n", nRigidBodies);


	for (int j = 0; j < nRigidBodies; j++)
	{
		char szName[256];
		strcpy(szName, ptr);
		int nDataBytes = (int)strlen(szName) + 1;
		ptr += nDataBytes;
		//printf("@unpack_dataTransModel Name1: %s\n", szName);
	

		// rigid body pos/ori
		int ID = 0;
		ID= strtol(szName+2,NULL,10);// Decided by the name gived to the regidbody
		//memcpy(&ID, ptr, 4);
		ptr += 4;
		float x = 0.0f;
		memcpy(&x, ptr, 4);
		pose_opt[ID].x=x;
		ptr += 4;
		float y = 0.0f;
		memcpy(&y, ptr, 4);
		ptr += 4;
		float z = 0.0f;
		memcpy(&z, ptr, 4);
		pose_opt[ID].y=z;
		ptr += 4;
		float qx = 0;
		memcpy(&qx, ptr, 4);
		ptr += 4;
		float qy = 0;
		memcpy(&qy, ptr, 4);
		
		ptr += 4;
		float qz = 0;
		memcpy(&qz, ptr, 4);
		ptr += 4;
		float qw = 0;
		memcpy(&qw, ptr, 4);
		ptr += 4;

		float fRoll = atan2(2 * (qw * qz + qx * qy), 1 - 2 * (qz * qz + qx * qx));
  		//float fPitch = asin(CLAMP(2 * (qw * qx - qy * qz), -1.0f, 1.0f));
  		float fYaw = atan2(2 * (qw * qy + qz * qx), 1 - 2 * (qx * qx + qy * qy));
		pose_opt[ID].theta=fYaw;

		//printf("@unpack_dataTransID : %d\n", ID);
		//printf("@unpack_dataTranspos: [%3.2f,%3.2f,%3.2f]\n", x, y, z);
		//printf("@unpack_dataTransori: [%3.2f,%3.2f,%3.2f,%3.2f]\n", qx, qy, qz, qw);
		//printf("@unpack_dataTransori: [%3.2f,%3.2f]\n", fYaw, fRoll);
	}

}

void *thrd_func(void *arg){
	while(thread_stop==0) {
		server();
	}
}