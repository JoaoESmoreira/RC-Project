CC = gcc
FLAGS = -g -Wall -Wextra
OBJS = server.o


########## GENERICO ###################
all:	prog

prog:	$(OBJS)
		$(CC) $(FLAGS) $(OBJS) -o prog

clean:	
		rm $(OBJS) prog

%.o:	%.c
		$(CC) $(FLAGS) -c $^

######### DEPENDENCIAS ##################
server.o:	server.c