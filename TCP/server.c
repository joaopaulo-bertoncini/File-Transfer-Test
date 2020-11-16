/*
 * File : server.c
 *
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
#include <strings.h>

#define BUFFERT 512
#define BACKLOG 1

/* Declaration des fonctions*/
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_socket (int port);

struct sockaddr_in sock_serv,sock_clt;


int main(int argc,char** argv){
    int sfd,fd;
    unsigned int length=sizeof(struct sockaddr_in);
    long int fp, sock_data,count=0;
    unsigned int nsid;
    ushort clt_port;
    char buffer[BUFFERT],filename[256];
    char dst[INET_ADDRSTRLEN];
    int n_recive = 0;
	time_t intps;
	struct tm* tmi;
    
    if(argc!=2) {
		printf("Uso : %s <porta_serv> \n",argv[0]);
        exit(3);
    }
    
    sfd = create_socket(atoi(argv[1]));
    
    bzero(buffer,BUFFERT);
    listen(sfd,BACKLOG);

    nsid=accept(sfd,(struct sockaddr*)&sock_clt,&length);
    if(nsid==-1){
        perror("accept fail");
        return EXIT_FAILURE;
    }
    else {
        if(inet_ntop(AF_INET,&sock_clt.sin_addr,dst,INET_ADDRSTRLEN)==NULL){
            perror("erreur socket");
            exit (4);
        }
        clt_port=ntohs(sock_clt.sin_port);
        printf("Início da conexão para : %s:%d\n",dst,clt_port);
        
        bzero(filename,256);
        intps = time(NULL);
        tmi = localtime(&intps);
        bzero(filename,256);
        sprintf(filename,"filename-%d.%d.%d.%d.%d.%d.bin",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
        printf("Criando o arquivo de saída : %s\n",filename);
        
        if ((fd=open(filename,O_CREAT|O_WRONLY,0600))==-1)
        {
            perror("Erro ao criar arquivo");
            exit (3);
        }
        bzero(buffer,BUFFERT);
        
        sock_data=recv(nsid,buffer,BUFFERT,0);
        while(sock_data) {
            printf(">> %lld bytes recebidos \n",sock_data);
            if(sock_data==-1){
                perror("Recebimento falhou");
                exit(5);
            }
            if((fp=write(fd,buffer,sock_data))==-1){
                perror("Falha na gravação");
                exit (6);
            }
            count=count+fp;
            n_recive++;
            bzero(buffer,BUFFERT);
            sock_data=recv(nsid,buffer,BUFFERT,0);
        }
        close(sfd);
        close(fd);
    }
    close(nsid);
	printf("-----------------------------------------------------\n");	    
    printf("Início da conexão para : %s:%d\n",dst,clt_port);
    printf("Fim da transmissão : %s.%d\n",dst,clt_port);
    printf("Número de bytes recebidos : %ld \n",count);
    printf("Total recebidos : %i\n",n_recive); 
 	printf("-----------------------------------------------------\n");	   
    return EXIT_SUCCESS;
}

int create_socket (int port){
    int l;
	int sfd;
    int yes=1;
    
	sfd = socket(PF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("Falha no socket");
        return EXIT_SUCCESS;
	}
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,&yes,sizeof(int)) == -1 ) {
        perror("setsockopt erreur");
        exit(5);
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