CC = gcc
FLAGS = -g -Wall -Wextra -pthread
OBJS_SV = server.o read_file.o terminalAdmin.o market_manager.o serverUser.o multicastServer.o
OBJS_OP = userTerminal.o multicastClient.o
.PHONY: all clean


########## GENERICO ###################
all:	prog

prog:	$(OBJS_SV) $(OBJS_OP)
		$(CC) $(FLAGS) $(OBJS_SV) -o stock_server
		$(CC) $(FLAGS) $(OBJS_OP) -o operations_terminal

debug:	$(OBJS_SV)
		$(CC) $(FLAGS) $(OBJS_SV) -DDEBUG -o stock_server
		$(CC) $(FLAGS) $(OBJS_OP) -DDEBUG -o operations_terminal 
clean:	
		rm server_header.h.gch client_header.h.gch $(OBJS_OP) $(OBJS_SV) stock_server operations_terminal

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