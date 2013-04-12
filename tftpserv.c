#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

int REQUEST_LENGTH = 512;
int tftpPort = 69;
	
int main(int argc, char **argv)
{

	// Initialize Socket
	int sockfd, port;
	struct sockaddr_in servaddr, cliaddr;

	if( argc != 1 )
		printf("Usage: ./tftpserv\n");

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
	socklen_t length = sizeof(cliaddr);
	bzero(recvmesg, sizeof(recvmesg));
	bzero(parsedmesg, sizeof(parsedmesg));

	numBytesReceived = recvfrom(sockfd, recvmesg, REQUEST_LENGTH, 0, (struct sockaddr*) &cliaddr, &length);

	// Parse 
	char RRQ[4] = "RRQ ";
	char WRQ[4] = "WRQ ";
	char *clientIP;
	char from[5] = "from ";
	char space[] = " "; 
	int parsecount = 3;
	int recvcount;

	// Check for RRQ/WRQ

	if ((recvmesg[0] == 0) && (recvmesg[1] == 1))
		memcpy(parsedmesg, RRQ, sizeof(RRQ));
	else if ((recvmesg[0] == 0) && (recvmesg[1] == 2))
		memcpy(parsedmesg, WRQ, sizeof(WRQ));
	else
		perror("Not RRQ/WRQ packet");

	memcpy(&parsedmesg[parsecount], space, sizeof(space));
	parsecount++;

	// Get FileName
	recvcount = 2;
	
	while (1)
	{
		if ( recvmesg[recvcount] == 0 )
			{parsecount++; recvcount++; break;}

		parsedmesg[parsecount] = recvmesg[recvcount];
		parsecount++;
		recvcount++;
	}
	memcpy(&parsedmesg[parsecount], space, sizeof(space));
	parsecount++;

	// Get Mode
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

	write(1, parsedmesg, sizeof(parsedmesg));

	int a, b, c, d;
	unsigned long host;
	host = ntohl(cliaddr.sin_addr.s_addr);
	a = host >> 24;
	b = (host >> 16) & 0xff;
	c = (host >> 8) & 0xff;
	d = host & 0xff;

	printf("%d.%d.%d.%d\n", a, b, c, d);

	// Send Error Code
	// Make Error Packet
	char sendbuf[REQUEST_LENGTH];
	int bufferCount = 0;
	const int zero = 0;
	const int one = 1;
	const int five = 5;
	char FileNotFound[] = "File Not Found";
	
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

	// Sending
	if( (sendto(sockfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr))) < 0 )
		perror("Error Sending Error Packet");

	return 0;	
}
