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

void main(){
	int x, y, a, i;
	struct sockaddr_in serv_addr;
	char buf[50], fname[50];
	printf("Hi, I am the client\n");

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family=PF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(SERV_ADDR);
	serv_addr.sin_port=htons(SERV_TCP_PORT);

	if((x=socket(PF_INET, SOCK_STREAM, 0))<0){
		printf("socket creation error\n");
		exit(1);
	}
	printf("socket opened successfully. socket num is %d\n", x);

	if(connect(x, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("can't connect to the server\n");
		exit(1);
	}

	printf("enter hello\n");
	gets(buf);			//send hello
	write(x, buf, strlen(buf));
	y=read(x, buf, 50);		//rcv fname?
	write(1, buf, y);
	printf("\n");

	gets(buf);			//send fname
	write(x, buf, strlen(buf));
	strcpy(fname, buf);

	y=read(x, buf, 50);		//rcv ready?
	write(1, buf, y);
	printf("\n");

	gets(buf);
	write(x, buf, strlen(buf));	//send yes

	y=read(x, buf, 50);
	if(strcmp(buf, "protocol error")!=0 && strcmp(buf, "cannot read")!=0){
		a=open(fname, O_RDWR | O_CREAT, 00777);
		write(a, buf, y);
		for(;;){
			y=read(x, buf, 50);
			if(y==0) break;
			write(a, buf, y);
		}
		printf("ftp complete. check %s", fname);
	}

	else write(1, buf, y);		// print "error"

	printf("\n");
	close(x);
}
