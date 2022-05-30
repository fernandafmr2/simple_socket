#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#include <arpa/inet.h>
#include <unistd.h>

// http port
#define SERVER_PORT 80

// buffer data
#define MAXLINE 4096

void err_die(const char *fmt, ...);

int main(int argc, char **argv)
{
	if (argc != 2)
		err_die("usage: %s <server addr>", argv[0]);

	int  		sockfd, n;
	size_t  	sendbytes;
	struct sockaddr_in  	serveraddr;
	char 		sendline[MAXLINE];
	char 		recvline[MAXLINE];

	// SOCK_STREAM = use tcp
	// SOCK_DGRAM = use udp
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_die("Error when create socket");

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family 	= AF_INET;
	serveraddr.sin_port		= htons(SERVER_PORT); // host network, short
	
	if(inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) <= 0){
		close(sockfd);
		err_die("inet_pton err for %s ", argv[1]);
	}

	// connect server
	if(connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0){
		close(sockfd);
		err_die("connect failed");		
	}

	sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
	sendbytes = strlen(sendline);

	// send req
	if(write(sockfd, sendline, sendbytes) < 0)
		err_die("write err");

	// receive response
	memset(recvline, 0, MAXLINE);
	while((n = read(sockfd, recvline, MAXLINE-1)) > 0)
	{
		recvline[n] = 0x00;
		printf("block read: \n<%s>\n", recvline);
	}
	if(n<0)
		err_die("read error");

	close(sockfd);
	exit(0);
}

void err_die(const char *fmt, ...)
{
	int errno_save;
	va_list		ap;

	errno_save = errno;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "\n");
	fflush(stdout);

	if (errno_save != 0)
	{
		fprintf(stdout, "(errno = %d) : %s\n\n", errno_save,
		strerror(errno_save));
		fflush(stdout);
	}
	va_end(ap);

	// terminate with an err
	exit(1);
}
