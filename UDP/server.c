/*
 * File : server.c
 * Aluno: Joao Paulo S Bertoncini
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFERT 512

int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_socket (int port);

struct sockaddr_in sock_serv,clt;

int main (int argc, char**argv){
	int fd, sfd;
	char buf[BUFFERT];
	off_t count=0, fp,sock_data;
	char filename[256];
    unsigned int l=sizeof(struct sockaddr_in);
	int n_recive = 0;
	time_t intps;
	struct tm* tmi;
    
	if (argc != 2){
		printf("Uso : %s <porta_serv> \n",argv[0]);
		return EXIT_FAILURE;
	}
    
    sfd = create_socket(atoi(argv[1]));
    
	intps = time(NULL);
	tmi = localtime(&intps);
	bzero(filename,256);
	sprintf(filename,"filename-%d.%d.%d.%d.%d.%d.bin",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
	printf("Criando o arquivo de saída : %s\n",filename);
    
	if((fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600))==-1){
        perror("Erro ao criar arquivo");
		return EXIT_FAILURE;
	}
    
	bzero(&buf,BUFFERT);
    sock_data=recvfrom(sfd,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	while(sock_data){ 
		n_recive++;
		if(sock_data==-1){
		    perror("Recebimento falhou");
			return EXIT_FAILURE;
		}
       printf(">> %ld bytes recebidos \n", sock_data);
       if((fp=write(fd,buf,sock_data))==-1){
           perror("Falha na gravação");
           exit (6);
        }
		//sendto(sfd, "OK", strlen("OK"), 0, (struct sockaddr *)&clt, sizeof(clt));
		count+=fp;			
		bzero(buf,BUFFERT);
        sock_data=recvfrom(sfd,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	}
    printf("-----------------------------------------------------\n");
    printf("Bytes recebidos : %ld \n",count);   
    printf("Total recebidos : %d \n",n_recive); 
    printf("-----------------------------------------------------\n");	
    close(sfd);
    close(fd);
	return EXIT_SUCCESS;
}

/* Fonction permettant le calcul de la durée de l'envoie */
int duration (struct timeval *start,struct timeval *stop,struct timeval *delta)
{
    suseconds_t microstart, microstop, microdelta;
    
    microstart = (suseconds_t) (100000*(start->tv_sec))+ start->tv_usec;
    microstop = (suseconds_t) (100000*(stop->tv_sec))+ stop->tv_usec;
    microdelta = microstop - microstart;
    
    delta->tv_usec = microdelta%100000;
    delta->tv_sec = (time_t)(microdelta/100000);
    
    if((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
        return -1;
    else
        return 0;
}

int create_socket (int port){
    int l;
	int sfd;
    
	sfd = socket(AF_INET,SOCK_DGRAM,0);
	if (sfd == -1){
        perror("socket fail");
        return EXIT_FAILURE;
	}
    
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
	sock_serv.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(sfd,(struct sockaddr*)&sock_serv,l)==-1){
		perror("bind fail");
		return EXIT_FAILURE;
	}
    
    return sfd;
}