CC = gcc

OFILES = client.o \
	server.o 

CFILECLT = client.c

CFILESRV = server.c

EXESRV = server

EXECLT = client

but : ${EXESRV} ${EXECLT}

${EXESRV} :
	$(CC) $(CFLAGS) -o ${EXESRV} ${CFILESRV}

${EXECLT} :
	$(CC) $(CFLAGS) -o ${EXECLT} ${CFILECLT}

# nettoyage des fichiers crees
clean :
	-rm -f ${OFILES} ${EXESRV} ${EXECLT} *~
                 
mrproper : clean but

.PHONY : but clean mrproper




