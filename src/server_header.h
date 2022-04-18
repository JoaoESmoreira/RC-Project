#ifndef server_H_
#define server_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define DEBUG
#define READING(X,Y,Z, ...) if (X != 2 && Y != Z)  { printf(__VA_ARGS__); exit(EXIT_FAILURE); }
#define CHECK(X, ...)       if (X == -1)           { printf(__VA_ARGS__); exit(EXIT_FAILURE); }
#define CHECK_PTHR(X, ...)  if (X == EXIT_FAILURE) { printf(__VA_ARGS__); exit(EXIT_FAILURE); }


#define BUFLEN     1024
#define PORT       9876
#define SOCKADDRIN struct sockaddr_in

#define MAXLEN   20
#define MAXUSERS 10
#define MAXSTOCK 6 


typedef struct _ADMIN {
    char name[MAXLEN];
    char password[MAXLEN];
} ADMIN;

typedef struct _USER {
    char name[MAXLEN];
    char password[MAXLEN];
    char markets[2][MAXLEN];
    int  budget;
    int  size;
} USER;

typedef struct _STOCK_LIST {
    char market[MAXLEN];
    char name[MAXLEN];
    int  price;
    int  size;
} STOCK_LIST;

typedef struct _ADMIN_SERVER_ARGS {
    ADMIN admin;
    USER *users;
    STOCK_LIST *stock;
} ADMIN_SERVER_ARGS;


FILE* check_file (const char *);
ADMIN read_admin_file (FILE *);
int   read_number_users (FILE *);
void  read_user_file (FILE *, USER *, int);
void  init_stock(STOCK_LIST *);
void  read_stock_file (FILE *, STOCK_LIST *);
void* admin_usage (void *);


int REFRESH_TIME;


#endif 