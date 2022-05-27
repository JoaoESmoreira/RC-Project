#ifndef client_H_
#define client_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <pthread.h>

#define CHECK(X, ...) if (X == -1) { printf(__VA_ARGS__); exit(EXIT_FAILURE); }
#define MAXLEN 20
#define PORT 6000



void* multiSub(void* );

bool impr;
bool unsc;

#endif
