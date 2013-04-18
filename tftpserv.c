/* TO DO:
	- Make Functions take (char *)buffers point to a local buffer declared in the function (or find better solution)

	- Fill-in other functions

	- For project 4, add concurrency (threads)
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <pthread.h>


// Global Declarations

int REQUEST_LENGTH = 516;

// Function Definitions

int sendDATA(int sockfd, struct sockaddr_in servaddr, struct sockaddr_in cliaddr, char *filename)
{

	int clientfd, numBytesRead, tftpPort = ((int)time(NULL)%65)*1000;
	struct sockaddr_in TIDaddr;

	if( (clientfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		perror("socket error");

	bzero(&TIDaddr, sizeof(struct sockaddr_in));
	TIDaddr.sin_family=AF_INET;
	TIDaddr.sin_addr.s_addr=servaddr.sin_addr.s_addr;
	TIDaddr.sin_port=htons(tftpPort);

	if( (bind(clientfd, (struct sockaddr*) &TIDaddr, sizeof(TIDaddr))) < 0)
		perror("bind error");

	char sendbuf[REQUEST_LENGTH];
	char recvACK[4];
	socklen_t clilength = sizeof(cliaddr);
	int bufferCount = 0, blockCount = 1, filefd = 0;
	const int zero = 0;
	const int three = 3;
	int tensBlockDigit;
	int onesBlockDigit;

	// Check if file exists, if not send ERROR packet
	if( (filefd = open(filename, 0)) < 0  )
		sendERROR(sockfd, cliaddr);

	for(;;)
	{
		bzero(sendbuf, sizeof(sendbuf));

		// Op Code for DATA
		memcpy( &sendbuf[bufferCount], &zero, sizeof(zero));
		bufferCount++;
		memcpy( &sendbuf[bufferCount], &three, sizeof(three));
		bufferCount++;

		memcpy( &sendbuf[bufferCount], &zero, sizeof(zero));
		bufferCount++;
		memcpy( &sendbuf[bufferCount], &blockCount, sizeof(blockCount));
		bufferCount++;


		// Read file into packet and send, break if last data packet
		if( (numBytesRead = read( filefd, &sendbuf[bufferCount], 512)) < 512)
		{
			sendto(clientfd, sendbuf, numBytesRead + 4, 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr));
			
			// Wait for ACK packet, send again if no response
			if (!recvfrom(clientfd, recvACK, sizeof(recvACK), 0, (struct sockaddr*) &cliaddr, &clilength))
				sendto(clientfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr));	
			break;
		}

		sendto(clientfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr));

		// Wait for ACK packet, send again if no response
		if (!recvfrom(clientfd, recvACK, sizeof(recvACK), 0, (struct sockaddr*) &cliaddr, &clilength))
			sendto(clientfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr));		
		// Increment block count and reset buffer count
		blockCount++;
		bufferCount = 0;
	}

}

/*int sendACK(int sockfd, struct sockaddr_in cliaddr)
{
	char sendbuf[4];
	int bufferCount = 0;
	const int zero = 0;
	const int one = 1;
	const int five = 5;
	char FileNotFound[] = "File Not Found";

	bzero(sendbuf, sizeof(sendbuf));
}
*/

int sendERROR(int sockfd, struct sockaddr_in cliaddr)
{
	char sendbuf[19];
	int bufferCount = 0;
	const int zero = 0;
	const int one = 1;
	const int five = 5;
	char FileNotFound[] = "File Not Found";

	bzero(sendbuf, sizeof(sendbuf));
	
	memcpy( &sendbuf[bufferCount], &zero, sizeof(zero));
	bufferCount++;
	memcpy( &sendbuf[bufferCount], &five, sizeof(five));
	bufferCount++;
	memcpy( &sendbuf[bufferCount], &zero, sizeof(zero));
	bufferCount++;
	memcpy( &sendbuf[bufferCount], &one, sizeof(one));
	bufferCount++;
	memcpy( &sendbuf[bufferCount], FileNotFound, sizeof(FileNotFound));
	bufferCount += strlen(FileNotFound);
	memcpy( &sendbuf[bufferCount], &zero, sizeof(zero));

	if( (sendto(sockfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr))) < 0 )
		perror("Error Sending Error Packet");
}



void ParseRequest(char *recvmesg, int numBytesReceived, struct sockaddr_in cliaddr, int *requestBit, char* Filename, char* Mode, char* ClientIP)
{
	char RRQ[4] = "RRQ ";
	char WRQ[4] = "WRQ ";
	char filename[50];
	char mode[10];
	char clientIP[16];
	char from[5] = "from ";
	char space[] = " "; 
	char parsedmesg[REQUEST_LENGTH];
	int parsecount = 3;
	int recvcount;

	bzero(parsedmesg, sizeof(parsedmesg));
	bzero(filename, sizeof(filename));
	bzero(mode, sizeof(mode));
	bzero(clientIP, sizeof(clientIP));

	// Check for RRQ/WRQ

	if ((recvmesg[0] == 0) && (recvmesg[1] == 1))
		{
			memcpy(parsedmesg, RRQ, sizeof(RRQ));
			*requestBit = 1;
		}
	else if ((recvmesg[0] == 0) && (recvmesg[1] == 2))
		{
			memcpy(parsedmesg, WRQ, sizeof(WRQ));
			*requestBit = 2;
		}
	else
		perror("Not RRQ/WRQ packet");

	memcpy(&parsedmesg[parsecount], space, sizeof(space));

	// Get FileName
		parsecount++;
		recvcount = 2;

		memcpy(filename, &recvmesg[recvcount], sizeof(filename));
	
		while (1)
		{
			if ( recvmesg[recvcount] == 0 )
			{
				bzero(&filename[recvcount - 2], sizeof(filename));
				parsecount++; 
				recvcount++; 
				break;
			}

			parsedmesg[parsecount] = recvmesg[recvcount];
			parsecount++;
			recvcount++;
		}

		memcpy(&parsedmesg[parsecount], space, sizeof(space));
		parsecount++;

	// Get Mode
		memcpy(mode, &recvmesg[recvcount], sizeof(mode));

		while (1)
		{
			if ( recvmesg[recvcount] == 0 )
			{
				parsecount++; 
				recvcount++;
				break;
			}

			parsedmesg[parsecount] = recvmesg[recvcount];
			parsecount++;
			recvcount++;
		}
		memcpy(&parsedmesg[parsecount], space, sizeof(space));
		parsecount++;

	// Get Address from packet
		memcpy(&parsedmesg[parsecount], from, sizeof(from));
		parsecount+=strlen(from);

		int a, b, c, d;
		unsigned long host;
		host = ntohl(cliaddr.sin_addr.s_addr);
		a = host >> 24;
		b = (host >> 16) & 0xff;
		c = (host >> 8) & 0xff;
		d = host & 0xff;

		sprintf(&parsedmesg[parsecount], "%d.%d.%d.%d\n", a, b, c, d);
		sprintf(clientIP, "%d.%d.%d.%d\n", a, b, c, d);

		// Write Parsed Request String to STD_OUT
		write(1, parsedmesg, sizeof(parsedmesg));

		// Write Extractions to input arrays
		memcpy(Filename, filename, sizeof(filename));
		memcpy(Mode, mode, sizeof(mode));
		memcpy(ClientIP, clientIP, sizeof(clientIP));
}

	
int main(int argc, char **argv)
{
	int sockfd, tftpPort;
	struct sockaddr_in servaddr;

	if( argc != 2 )
		perror("Usage: ./tftpserv <port number>\n");

	tftpPort = atoi(argv[1]);

	if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		perror("socket error");

	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(tftpPort);

	if( (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr))) < 0)
		perror("bind error");

	// Receive and write packets

	int numBytesReceived = 0; 
	char recvmesg[REQUEST_LENGTH];
	char parsedmesg[REQUEST_LENGTH];
	struct sockaddr_in cliaddr;
	socklen_t length = sizeof(cliaddr);
	bzero(recvmesg, sizeof(recvmesg));
	bzero(parsedmesg, sizeof(parsedmesg));

	for(;;)
	{
		numBytesReceived = recvfrom(sockfd, recvmesg, REQUEST_LENGTH, 0, (struct sockaddr*) &cliaddr, &length);

		// Parse Function -- Take string as input buffer, parse request and put into new buffer to print.  Extracts filename, TFTP mode, client IP address
		int requestBit;
		char filename[50];
		char mode[10];
		char clientIP[16];
		ParseRequest(recvmesg, numBytesReceived, cliaddr, &requestBit, filename, mode, clientIP);


		switch ( requestBit )
		{
			case 1:	// RRQ
				sendDATA(sockfd, servaddr, cliaddr, filename);
				break;

			case 2:	// WRQ
				// write
				break;

			default://Send ERROR packet
				sendERROR(sockfd, cliaddr);
				break;
		
		}
	}	
    return 0;
}
