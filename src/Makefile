CC = gcc
FLAGS = -g -Wall -Wextra
OBJS = server.o biblio.o
.PHONY: all clean


########## GENERICO ###################
all:	prog

prog:	$(OBJS)
		$(CC) $(FLAGS) $(OBJS) -o prog

clean:	
		rm $(OBJS) prog

%.o:	%.c
		$(CC) $(FLAGS) -c $^

######### DEPENDENCIAS ##################
server.o:	server.c biblio.c
biblio.o:	biblio.c biblio.h