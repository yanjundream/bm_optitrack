/*
    Simple udp server
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include "bm_pose2d.h"
 
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

void server()
{

    //printf("Build the simple server\n");
    struct sockaddr_in si_me, si_other;
     
    int s, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
     
    //create a UDP socket

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     


        //  printf("Waiting for data...");
        fflush(stdout);
         
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:\n", inet_ntoa(si_other.sin_addr));
        DB_unpack(buf);       

	
/*	for (int ID=1; ID<=10;ID ++)
	  printf("pos of robot %d: [%3.2f,%3.2f]\n", ID, pose_opt[ID].x, pose_opt[ID].y);
	memset(buf,'\0', BUFLEN);
*/
    close(s);
    return;
}


void DB_unpack(char* pData)
{
	//printf("Begin Packet Rigid Body Data\n-------\n");
	char *ptr = pData;

	int major = 2;
	int minor = 10;


	// rigid odies**************************************************


	int nRigidBodies = 0;

	memcpy(&nRigidBodies, ptr, 4); ptr += 4;
	printf("Rigid Body Count : %d\n", nRigidBodies);
	for (int j = 0; j < nRigidBodies; j++)
	{
		// rigid body pos/ori
		int ID = 0; memcpy(&ID, ptr, 4); ptr += 4;
		float x = 0.0f; memcpy(&x, ptr, 4); ptr += 4;
	        pose_opt[ID].x=x;

		float y = 0.0f; memcpy(&y, ptr, 4); ptr += 4;
		float z = 0.0f; memcpy(&z, ptr, 4); ptr += 4;
		pose_opt[ID].y=z;

		float qx = 0; memcpy(&qx, ptr, 4); ptr += 4;
		float qy = 0; memcpy(&qy, ptr, 4); ptr += 4;
		pose_opt[ID].theta=qy;

		float qz = 0; memcpy(&qz, ptr, 4); ptr += 4;
		float qw = 0; memcpy(&qw, ptr, 4); ptr += 4;
		//printf("ID : %d\n", ID);
		//printf("pos: [%3.2f,%3.2f,%3.2f]\n", x, y, z);
		//printf("pos of robot %d: [%3.2f,%3.2f]\n", ID, pose_opt[ID].x, pose_opt[ID].y);
		//printf("ori: [%3.2f,%3.2f,%3.2f,%3.2f]\n", qx, qy, qz, qw);

		// associated marker positions
		int nRigidMarkers = 0;  memcpy(&nRigidMarkers, ptr, 4); ptr += 4;
		//printf("Marker Count: %d\n", nRigidMarkers);
		int nBytes = nRigidMarkers * 3 * sizeof(float);
		float* markerData = (float*)malloc(nBytes);
		memcpy(markerData, ptr, nBytes);
		ptr += nBytes;

		if (major >= 2)
		{

			// associated marker IDs
			nBytes = nRigidMarkers * sizeof(int);
			int* markerIDs = (int*)malloc(nBytes);
			memcpy(markerIDs, ptr, nBytes);
			ptr += nBytes;

			// associated marker sizes
			nBytes = nRigidMarkers * sizeof(float);
			float* markerSizes = (float*)malloc(nBytes);
			memcpy(markerSizes, ptr, nBytes);
			ptr += nBytes;

			if (markerIDs)
				free(markerIDs);
			if (markerSizes)
				free(markerSizes);

		}
		

		if (major >= 2)
		{

			// Mean marker error
			float fError = 0.0f; memcpy(&fError, ptr, 4); ptr += 4;
		//	printf("Mean marker error: %3.2f\n", fError);
		}

		// 2.6 and later
		if (((major == 2) && (minor >= 6)) || (major > 2) || (major == 0))
		{

			// params
			short params = 0; memcpy(&params, ptr, 2); ptr += 2;
		//	int  bTrackingValid = params & 0x01; // 
		}

	} // next rigid body

}
