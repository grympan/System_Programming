#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

#define SERV_TCP_PORT 34567
#define SERV_ADDR "165.246.38.151"

struct client{
	char fname[50];
	int state;
};
struct client cli[50];

void handle_protocol(int x, fd_set * pset);
void handle_state_1(int x, fd_set *pset, char* buf);
void handle_state_2(int x, fd_set *pset, char* buf);
void handle_state_3(int x, fd_set *pset, char* buf);

void main(){
	int s1, s2, i, x, y;
	for(i=0; i<50; i++)
		cli[i].state=0;
	struct sockaddr_in serv_addr, cli_addr;
	char buf[50];
	socklen_t xx;

	printf("Hi, I am the server\n");

	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family=PF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(SERV_ADDR);
	serv_addr.sin_port=htons(SERV_TCP_PORT);

	if((s1=socket(PF_INET, SOCK_STREAM, 0))<0){
		printf("socket creation error\n");
		exit(1);
	}
	printf("socket opened successfully. socket num is %d\n", s1);

	x=bind(s1, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(x<0){
		printf("binding failed\n");
		exit(1);
	}
	printf("binding passed\n");
	listen(s1, 5);
	xx = sizeof(cli_addr);

	fd_set rset, pset;
	int maxfd=50;

	FD_ZERO(&rset);
	FD_ZERO(&pset);

	FD_SET(s1, &pset);

	for(i=0;i<40;i++){
		rset=pset;
		select(maxfd, &rset, NULL, NULL, NULL);

		for(x=0; x<maxfd; x++){
			if(FD_ISSET(x, &rset)){
				if(x==s1){
					s2=accept(s1, (struct sockaddr *)&cli_addr, &xx);
					cli[s2].state=1;
					printf("new cli at socket %d\n", s2);
					FD_SET(s2, &pset);
				}else
					handle_protocol(x, &pset);
			}
		}
	}
}

void handle_protocol(int x, fd_set * pset){
	int y; char buf[50];
	y=read(x, buf, 50);
	buf[y]=0;
	if(cli[x].state==1)
		handle_state_1(x, pset, buf);
	else if(cli[x].state==2)
		handle_state_2(x, pset, buf);
	else if(cli[x].state==3)
		handle_state_3(x, pset, buf);
}

void handle_state_1(int x, fd_set *pset, char* buf){
	if (strcmp(buf, "hello")==0){
		write(x, "file name?", 10);
		cli[x].state=2;
	}else{
		write(x, "protocol error", 14);
		close(x);
		FD_CLR(x, pset);
	}
}

void handle_state_2(int x, fd_set *pset, char* buf){
	strcpy(cli[x].fname, buf);
	write(x, "ready?", 6);
	cli[x].state=3;
}

void handle_state_3(int x, fd_set *pset, char* buf){
	if (strcmp(buf, "yes")==0){
		int a, b;
		a=open(cli[x].fname, O_RDONLY, 00777);

		if(a<0) write(x, "cannot read", 11);

		else
			for(;;){
				b=read(a, buf, 50);
				if(b==0) break;
				write(x, buf, b);
			}
	}
	else write(x, "protocol error", 14);

	close(x);
	FD_CLR(x, pset);
}
