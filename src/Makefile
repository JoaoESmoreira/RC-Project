CC = gcc
FLAGS = -g -Wall -Wextra -pthread
OBJS = server.o read_file.o terminalAdmin.o market_manager.o serverUser.o multicastServer.o
.PHONY: all clean


########## GENERICO ###################
all:	prog

prog:	$(OBJS) userTerminal.o multicastClient.o
		$(CC) $(FLAGS) $(OBJS) -o prog
		$(CC) $(FLAGS) userTerminal.o multicastClient.o -o clientTerminal

debug:	$(OBJS)
		$(CC) $(FLAGS) -pthread $(OBJS) -DDEBUG -o prog

clean:	
		rm $(OBJS) prog server_header.h.gch client_header.h.gch clientTerminal multicastClient.o userTerminal.o

zip:
		zip Projeto.zip -r *.c *.h *.pdf Makefile configFile.txt

%.o:	%.c
		$(CC) $(FLAGS) -pthread -c $^

######### DEPENDENCIAS ##################
server.o:			server.c terminalAdmin.c read_file.c
read_file.o:		read_file.c server_header.h
terminalAdmin.o:	terminalAdmin.c server_header.h
market_manager.o:	market_manager.c server_header.h
serverUser.o:		serverUser.c server_header.h
userTerminal.o:		userTerminal.c
multicastClient.o:	multicastClient.c client_header.h
multicastServer.o:	multicastServer.c server_header.h