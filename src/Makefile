CC = gcc
FLAGS = -g -Wall -Wextra
OBJS = server.o read_file.o terminalAdmin.o market_manager.o serverUser.o 
.PHONY: all clean


########## GENERICO ###################
all:	prog

prog:	$(OBJS) userTerminal.o
		$(CC) $(FLAGS) -pthread $(OBJS) -o prog
		$(CC) $(FLAGS) userTerminal.o -o clientTerminal

debug:	$(OBJS)
		$(CC) $(FLAGS) -pthread $(OBJS) -DDEBUG -o prog

clean:	
		rm $(OBJS) prog server_header.h.gch clientTerminal

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