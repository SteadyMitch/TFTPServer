/* TO DO:
	- Convert network address to string to write it to stdout
	- Be able to print parsedString outside of parse function
*/




#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

int REQUEST_LENGTH = 512;
int tftpPort = 69;
int errorCode = 5;

/* int sendWRQ
*/

/* int sendDATA
/

/* int sendACK
*/

/*
int sendERROR(int sockfd, char *sendbuf, size_t buflen, struct sockaddr* cliaddr)
{
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

	// Write error message to STDOUT
	write(1, sendbuf, sizeof(sendbuf));

	if( (sendto(sockfd, sendbuf, sizeof(sendbuf), 0, cliaddr, sizeof(cliaddr))) < 0 )
		perror("Error Sending Error Packet");
	return 0;
}
*/
	
/*
void ParseRequest(char *recvmesg, int numBytesReceived, struct sockaddr_in cliaddr, char *parsedmesg)
{
	char RRQ[4] = "RRQ ";
	char *filename;
	char *mode;
	char *clientIP;
	char from[5] = "from ";
	char space[] = " "; 
	char writeChar;
	int parsecount = 3;
	int recvcount;

	// Check for RRQ

	if ((recvmesg[0] == 0) && (recvmesg[1] == 1))
		memcpy(parsedmesg, RRQ, sizeof(RRQ));
	else
		perror("Not RRQ packet");

	//write(1, parsedmesg, strlen(parsedmesg));

	memcpy(&parsedmesg[parsecount], space, sizeof(space));

	// Get FileName
		parsecount++;
		recvcount = 2;
		
		while (1)
		{
			if ( recvmesg[recvcount] == 0 )
				{parsecount++; recvcount++; break;}

			parsedmesg[parsecount] = recvmesg[recvcount];
			parsecount++;
			recvcount++;
		}


		write(1, parsedmesg, sizeof(parsedmesg));
		
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

		//write(1, parsedmesg, sizeof(parsedmesg));

	// Get Address from packet
		memcpy(&parsedmesg[parsecount], from, sizeof(from));
		parsecount+=strlen(from);

		write(1, parsedmesg, sizeof(parsedmesg));

		if( (clientIP = inet_ntoa(cliaddr.sin_addr)) < 0 )
			perror("Error converting IP address to string");

		memcpy(&parsedmesg[parsecount], clientIP, sizeof(clientIP));

		write(1, parsedmesg, sizeof(parsedmesg));

}
*/
	
int main(int argc, char **argv)
{
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

	for(;;)
	{
		numBytesReceived = recvfrom(sockfd, recvmesg, REQUEST_LENGTH, 0, (struct sockaddr*) &cliaddr, &length);

		// Parse Function -- Take string as input buffer, parse from request and put into new buffer to print
	char RRQ[4] = "RRQ ";
	char WRQ[4] = "WRQ ";
	char *filename;
	char *mode;
	char *clientIP;
	char from[5] = "from ";
	char space[] = " "; 
	char writeChar;
	int parsecount = 3;
	int recvcount;

	// Check for RRQ

	if ((recvmesg[0] == 0) && (recvmesg[1] == 1))
		memcpy(parsedmesg, RRQ, sizeof(RRQ));
	else if ((recvmesg[0] == 0) && (recvmesg[1] == 1))
		memcpy(parsedmesg, WRQ, sizeof(WRQ));
	else
		perror("Not RRQ/WRQ packet");

	//write(1, parsedmesg, strlen(parsedmesg));

	memcpy(&parsedmesg[parsecount], space, sizeof(space));

	// Get FileName
		parsecount++;
		recvcount = 2;
		
		while (1)
		{
			if ( recvmesg[recvcount] == 0 )
				{parsecount++; recvcount++; break;}

			parsedmesg[parsecount] = recvmesg[recvcount];
			parsecount++;
			recvcount++;
		}


		//write(1, parsedmesg, sizeof(parsedmesg));
		
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

		//write(1, parsedmesg, sizeof(parsedmesg));

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

	// Write error message to STDOUT
	write(1, sendbuf, sizeof(sendbuf));

	if( (sendto(sockfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr))) < 0 )
		perror("Error Sending Error Packet");

	}
// ReceiveAndPrint(sockfd, (struct sockaddr*) &cliaddr, sizeof(cliaddr));

	return 0;	
}
