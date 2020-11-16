/*
 * File : client.c
 * Aluno: Joao Paulo S Bertoncini
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

 //buffer usado para enviar o arquivo
#define BUFFERT 512

int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_socket (int port, char* ipaddr);

struct sockaddr_in sock_serv;

int main (int argc, char**argv){
	struct timeval start, stop, delta;
    int sfd,fd;
    char buf[BUFFERT];
    off_t count=0, sock_data,sz;//long
	long int fp;
    int l=sizeof(struct sockaddr_in);
	struct stat buffer;
	int n_send = 0;
    
	if (argc != 4){
		printf("Uso : %s <ip_serv> <porta_serv> <filename>\n",argv[0]);
		return EXIT_FAILURE;
	}
    
    sfd=create_socket(atoi(argv[2]), argv[1]);
    
	if ((fd = open(argv[3],O_RDONLY))==-1){
		perror("Erro ao falha ao abrir o arquivo");
		return EXIT_FAILURE;
	}
    
	//tamanho do arquivo 
	if (stat(argv[3],&buffer)==-1){
		perror("Erro ao detectar tamanho do arquivo");
		return EXIT_FAILURE;
	}
	else
		sz=buffer.st_size;
    
	bzero(&buf,BUFFERT);
    
    if(connect(sfd,(struct sockaddr*)&sock_serv,l)==-1){
        perror("Erro ao conectar\n");
        exit (3);
    }
	gettimeofday(&start,NULL);
    fp=read(fd,buf,BUFFERT);
	while(fp){
		if(fp==-1){
			perror("Erro: falha na leitura");
			return EXIT_FAILURE;
		}
		sock_data=sendto(sfd,buf,fp,0,(struct sockaddr*)&sock_serv,l);
		if(sock_data==-1){
			perror("Erro: falha no envio");
			return EXIT_FAILURE;
		}
	    printf(">> (%d) %lld bytes enviados \n",n_send+1,fp);
		count+=sock_data;
        n_send++;
		bzero(buf,BUFFERT);
        fp=read(fd,buf,BUFFERT);
	}

	sock_data=sendto(sfd,buf,0,0,(struct sockaddr*)&sock_serv,l);
	gettimeofday(&stop,NULL);
	duration(&start,&stop,&delta);
    
	printf("-----------------------------------------------------\n");	
	printf("Tamanho total : %lld (%lld Mb) \n",sz, (sz / 1000000));
	printf("Bytes transferidos : %lld\n",count);
	printf("Tempo total : %ld.%d \n",delta.tv_sec,delta.tv_usec);
	printf("Total enviados : %i \n",n_send);	
	printf("-----------------------------------------------------\n");
    
    close(sfd);
	return EXIT_SUCCESS;
}

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

int create_socket (int port, char* ipaddr){
    int l;
	int sfd;
    
	sfd = socket(PF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("Erro: Socket falhou");
        return EXIT_FAILURE;
	}
    
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
    if (inet_pton(AF_INET,ipaddr,&sock_serv.sin_addr)==0){
		printf("Erro: endereço IP inválido\n");
		return EXIT_FAILURE;
	}
    
    return sfd;
}