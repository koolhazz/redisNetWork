#include "ae.h"
#include "anet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void clientReceive(aeEventLoop *el, int fd, void *privdata, int mask);

int main(int argc, char *argv[])
{

	if( argc < 3)
	{
		fprintf(stderr,"Usage[%d] para\n", argc);
		exit(0);
	}
	int nPort = atoi(argv[1]);	
	aeEventLoop *el = NULL;

	el = aeCreateEventLoop();
	if( NULL == el)
	{
		fprintf(stderr,"aeCreateEventLoop err\n");
		exit(0);
	}
	
	char err[256] = {0};
	int fd = anetTcpServer(err, atoi(argv[1]), argv[2]);
	if( fd == AE_ERR)
	{
		fprintf(stderr,"Create fd fail:%s", err);
		exit(0);
	}

	if ( aeCreateFileEvent(el, fd, AE_READABLE, acceptTcpHandler, NULL) == AE_ERR)
	{
		fprintf(stderr,"aeCreateFileEvent err\n");
		exit(0);
	}

	while( 1 )
	{
		aeProcessEvents(el, AE_ALL_EVENTS);
	}
	exit(0);
	
}


void acceptTcpHandler(aeEventLoop * el,int fd,void * privdata,int mask)
{
	int cport = 0, cfd = 0;
	char cip[128] = {0};
	char err[256] = {0};
	(void )privdata;
	(void )mask;
	cfd = anetTcpAccept(err, fd, cip, &cport );
	if( cfd == AE_ERR)
	{
		fprintf(stderr,"acceptTcpHandler anetTcpAccept error:%s\n", err);	
		exit(0);
	}
	printf("Client connected ip[%s] port[%d]\n", cip, cport);
	if( aeCreateFileEvent(el, cfd, AE_READABLE, clientReceive, NULL) == AE_ERR)
	{
		fprintf(stderr,"acceptTcpHandler anetTcpAccept error:%s\n");	
		exit(0);
	}
}

void clientReceive(aeEventLoop * el,int fd,void * privdata,int mask)
{
	(void )privdata;
	(void )mask;
	(void )el;
	char buf[254] = {0};
	int nread = read(fd, buf, sizeof(buf));
	if( nread == -1)
	{
		if( errno == EAGAIN)
		{
			nread = 0;
		}
		else
		{
			fprintf(stderr,"Read from client:%s\n", strerror(errno));
			close(fd);
			return ;
		}
	}
	else if( nread == 0)
	{
		fprintf(stderr,"Client closed connection\n");
		close(fd);
		aeDeleteFileEvent(el, fd, AE_READABLE);
		return ;
	}
	if( nread )
	{
		printf("%s", buf);
		write(fd, buf, strlen(buf));		
	}
}



