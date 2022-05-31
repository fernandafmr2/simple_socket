#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#include <arpa/inet.h>
#include <unistd.h>

// http port
#define SERVER_PORT 80

void err_die(const char *fmt, ...);

int main(int argc, char **argv)
{
	static const char http_payload[] = "GET / HTTP/1.1\r\n\r\n";
	int  		sockfd,err;
	size_t  	sendbytes;
	ssize_t		ret;
	struct sockaddr_in  	serveraddr;
	const char * 	sendline;
	char 		recvline[4096];
	
	if (argc != 2)
		//einval
		err_die("usage: %s <server addr>", argv[0]);

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

	sendline=http_payload;
	sendbytes = strlen(http_payload);

	// send req
write_:
	ret = write(sockfd, sendline, sendbytes);
	if(ret<=0){
		if(ret == 0) {
			puts("server error while send data");
			close(sockfd);
			return ENETDOWN;
		}

		err = errno;
		if(err==EINTR) // write interupt
			goto write_;

		err_die("write err");
		close(sockfd);
		return err;
	}

	// short write handling
	sendbytes -= (size_t)ret;
	if(sendbytes>0){
		// short write handle
		sendline += (size_t)ret;
		goto write_;
	}

read_:
	ret = read(sockfd, recvline, sizeof(recvline)-1);
	if (ret < 0) {
		err_die("read");
		close(sockfd);
		return ret;
	}

	if (ret == 0) {
		/*
		 * EOF selesai
		 */
		close(sockfd);
		return 0;
	}

	/* set null terminating for safe print*/
	recvline[ret] = '\0';
	printf("%s", recvline);
	goto read_;
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
