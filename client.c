#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<netdb.h>
#include<unistd.h>
#include<strings.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>
#include<errno.h>

// http port
#define SERVER_PORT 80

// buffer data
#define MAXLINE 4096
#define SA struct sockaddr

void err_die(const char *fmt, ...);

int main(int argc, char **argv)
{
	int  		sockfd, n;
	int  		sendbytes;
	struct sockaddr_in  	serveraddr;
	char 		sendline[MAXLINE];
	char 		recvline[MAXLINE];

	if (argc != 2)
		err_die("usage: %s <server addr>", argv[0]);

	// AF_INET = address family-internet
	// 0 = use tcp
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_die("Error when create socket");

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family 	= AF_INET;
	serveraddr.sin_port		= htons(SERVER_PORT); // host network, short
	
	if(inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) <= 0)
		err_die("inet_pton err for %s ", argv[1]);

	// connect server
	if(connect(sockfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
		err_die("connect failed");

	sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
	sendbytes = strlen(sendline);

	// send req
	if(write(sockfd, sendline, sendbytes) != sendbytes)
		err_die("write err");

	// receive response
	memset(recvline, 0, MAXLINE);
	while((n = read(sockfd, recvline, MAXLINE-1)) > 0)
	{
		printf("%s", recvline);
		memset(recvline, 0, MAXLINE);
	}
	if(n<0)
		err_die("read error");

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
		fprintf(stdout, "(errno = %d) : %s\n", errno_save,
		strerror(errno_save));
		fprintf(stdout, "\n");
		fflush(stdout);
	}
	va_end(ap);

	// terminate with an err
	exit(1);
}
