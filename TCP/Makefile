# 
# File : Makefile
#
#

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

clean :
	-rm -f ${OFILES} ${EXESRV} ${EXECLT} *~
                 
mrproper : clean but

.PHONY : but clean mrproper

# fin du Makefile



