#ifndef server_H_
#define server_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <netinet/in.h>

#define DEBUG
#define READING(X, Y, Z, ...) \
    if (X != 2 && Y != Z)     \
    {                         \
        printf(__VA_ARGS__);  \
        exit(EXIT_FAILURE);   \
    }
#define CHECK(X, ...)        \
    if (X == -1)             \
    {                        \
        printf(__VA_ARGS__); \
        exit(EXIT_FAILURE);  \
    }
#define CHECK_PTHR(X, ...)   \
    if (X == EXIT_FAILURE)   \
    {                        \
        printf(__VA_ARGS__); \
        exit(EXIT_FAILURE);  \
    }

#define PORT_BOLSA 8000
#define PORT       9876
#define BUFLEN     1024
#define SOCKADDRIN struct sockaddr_in
#define SOCKADDR   struct sockaddr

#define MAXLEN 20
#define MAXUSERS 10
#define MAXSTOCK 6

typedef struct _ADMIN
{
    char name[MAXLEN];
    char password[MAXLEN];
} ADMIN;

typedef struct _USER
{
    char name[MAXLEN];
    char password[MAXLEN];
    char markets[2][MAXLEN];
    int  budget;
    int  size; // in case of list
} USER;

typedef struct _STOCK_LIST
{
    char  market[MAXLEN];
    char  name[MAXLEN];
    float price;
    int   size; // in case of list
} STOCK_LIST;

typedef struct _ADMIN_SERVER_ARGS
{
    ADMIN admin;
    USER *users;
    STOCK_LIST *stock; 
} ADMIN_SERVER_ARGS;

typedef struct _CLIENT_SERVER_ARGS
{
    USER *users;
    STOCK_LIST *stock; 
} CLIENT_SERVER_ARGS;

typedef struct _USER_ARGS
{
    int client_fd;
    USER *users;
    STOCK_LIST *stock; 
} USER_ARGS;


FILE *check_file(const char *);
ADMIN read_admin_file(FILE *);
int  read_number_users(FILE *);
void read_user_file(FILE *, USER *, int);
void init_stock(STOCK_LIST *);
void read_stock_file(FILE *, STOCK_LIST *);
void *admin_terminal(void *);
void *market_manager(void *);
void *user_interaction(void *);

int REFRESH_TIME;
int terminal_fd;
int total_users_loged;
pthread_t admin_server, market_manager_server, user_interaction_server;
bool control;

#endif