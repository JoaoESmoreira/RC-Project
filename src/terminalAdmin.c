#include "server_header.h"

static int number_spaces(const char *string) {
    int count = 0;

    for (int i = 0; string[i] != '\0'; ++i) {
        if (string[i] == ' ')
            count++;
    }
    return count;
}

bool user_in_list (const USER *users, const char *username) {
    for (int i = 0; i < users->pos; ++i) {
        if (strcmp(users[i].name, username) == 0)
            return true;
    }
    return false;
}

static void add_user (USER *users, const char * command_line, int terminal_fd, SOCKADDRIN admin_addr) {
    char command[MAXLEN], username[MAXLEN], password[MAXLEN];

    if (users->pos < MAXUSERS) {
        char stock1[MAXLEN], stock2[MAXLEN];
        int  spaces = number_spaces(command_line);
        int  number_events, budget;

        if (spaces == 5) {
            number_events = sscanf(command_line, "%s %s %s %s %s %d", command, username, password, stock1, stock2, &budget);

            if (number_events  == 6) {
                if (!user_in_list(users, username)) {
                    strcpy(users[users->pos].name, username);
                    strcpy(users[users->pos].password, password);
                    strcpy(users[users->pos].markets[0], stock1);
                    strcpy(users[users->pos].markets[1], stock2);
                    users[users->pos].budget = budget;
                    users->pos++;
                } else {
                    for (int i = 0; i < users->pos; ++i) {
                        if (strcmp(users[i].name, username) == 0) {
                            strcpy(users[i].name, username);
                            strcpy(users[i].password, password);
                            strcpy(users[i].markets[0], stock1);
                            strcpy(users[i].markets[1], stock2);
                            users[i].budget = budget;
                        }
                    }
                }
            } else {
	            CHECK(sendto(terminal_fd, (void *) "Erro a ler os parametros\n", strlen("Erro a ler os parametros\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            }
        } else if (spaces == 4) {
            number_events = sscanf(command_line, "%s %s %s %s %d", command, username, password, stock1, &budget);

            if (number_events  == 5) {
                if (!user_in_list(users, username)) {
                    strcpy(users[users->pos].name, username);
                    strcpy(users[users->pos].password, password);
                    strcpy(users[users->pos].markets[0], stock1);
                    strcpy(users[users->pos].markets[1], "-");
                    users[users->pos].budget = budget;
                    users->pos++;
                } else {
                    for (int i = 0; i < users->pos; ++i) {
                        if (strcmp(users[i].name, username) == 0) {
                            strcpy(users[i].name, username);
                            strcpy(users[i].password, password);
                            strcpy(users[i].markets[0], stock1);
                            strcpy(users[i].markets[1], "-");
                            users[i].budget = budget;
                        }
                    }
                }
            } else {
	            CHECK(sendto(terminal_fd, (void *) "Erro a ler os parametros\n", strlen("Erro a ler os parametros\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            }
        } else {
	        CHECK(sendto(terminal_fd, (void *) "Numero de parametros errado\n", strlen("Numero de parametros errado\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
        }
    } else {
	    CHECK(sendto(terminal_fd, (void *) "Maximo de users atingido\n", strlen("Maximo de users atingido\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
    }
}

static void list (USER *users, char *string) {
    char aux[MAXLEN];

    for (int i = 0; i < users->pos; ++i) {
        strcat(string, users[i].name); strcat(string, " ");
        strcat(string, users[i].password); strcat(string, " ");
        strcat(string, users[i].markets[0]); strcat(string, " ");
        strcat(string, users[i].markets[1]); strcat(string, " ");
        sprintf(aux, "%d\n", users[i].budget);
        strcat(string, aux);
    }
}

static void delete (USER *users, USER *new_user, const char *name) {
    new_user->pos = 0;

    for (int i = 0; i < users->pos; ++i) {
        if (strcmp(name, users[i].name) != 0) {
            #ifdef DEBUG
            printf("%s - %s\n", users[i].name, name);
            printf("DELETE: %d\n", strcmp(name, users[i].name));
            #endif
            new_user[new_user->pos] = users[i];
            new_user->pos++;
        }
    }
}

void* admin_usage (void *args) {
    // read args
    ADMIN_SERVER_ARGS argumento = *((ADMIN_SERVER_ARGS *) args);
    ADMIN admin  = argumento.admin;
    USER  *users = argumento.users;

    SOCKADDRIN terminal_addr, admin_addr;
    socklen_t  t_len = sizeof(admin_addr);
    char       command_line[BUFLEN * 5];
    char       command[BUFLEN];
    int        terminal_fd;


    while (strcmp(command, "QUIT_SERVER")) {

        CHECK((terminal_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)), "Erro no socket\n");
	    terminal_addr.sin_family 	  = AF_INET;
	    terminal_addr.sin_port   	  = htons(PORT);
	    terminal_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	    CHECK(bind(terminal_fd, (struct sockaddr*) &terminal_addr, sizeof(terminal_addr)), "Erro no bind");

        char buf[BUFLEN], username[MAXLEN], password[MAXLEN];
        char log_men[] = "Introduza o seu nick: \n";
        char pas_men[] = "Introduza a sua password: \n";

        CHECK(recvfrom(terminal_fd, buf, BUFLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");

        // login
        while (true) {

	        CHECK(sendto(terminal_fd, (void *) log_men, strlen(log_men), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            CHECK(recvfrom(terminal_fd, username, MAXLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
            #ifdef DEBUG
            printf("Admin said: %s - %d\n", username, strcmp(username, admin.name));
            #endif

	        CHECK(sendto(terminal_fd, (void *) pas_men, strlen(pas_men), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            CHECK(recvfrom(terminal_fd, password, MAXLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
            #ifdef DEBUG
            printf("Admin said: %s - %d\n", password, strcmp(password, admin.password));
            #endif

            if (strcmp(username, admin.name) == 10 && strcmp(password, admin.password) == 10)
                break;

            printf("Credencias erradas\n");
        }
	    CHECK(sendto(terminal_fd, (void *) "Logged in.\n\n", strlen("Logged in.\n\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");


        // comandos terminal
        while (true) {
            CHECK(recvfrom(terminal_fd, command_line, BUFLEN * 5, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");

            sscanf(command_line, "%s", command);

            if (strcmp(command, "QUIT") == 0 || strcmp(command, "QUIT_SERVER") == 0) {
	            CHECK(sendto(terminal_fd, (void *) "Logged out.\n\n", strlen("Logged out.\n\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");

                close(terminal_fd);
                break;
            } else if (strcmp(command, "ADD_USER") == 0) {

                add_user(users, command_line, terminal_fd, admin_addr);

            } else if (strcmp(command, "LIST") == 0) {
                char string[BUFLEN] = "";
                
                list(users, string);

                CHECK(sendto(terminal_fd, (void *) string, strlen(string), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");

            } else if (strcmp(command, "REFRESH") == 0) {
                int number_events = sscanf(command_line, "%s %d", command, &REFRESH_TIME);
                if (number_events != 2) {
                    CHECK(sendto(terminal_fd, (void *) "Erro no comando\n", strlen("Erro no comando\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                }

                #ifdef DEBUG
                printf("%d\n", REFRESH_TIME);
                #endif
            } else if (strcmp(command, "DEL") == 0) {
                char name[MAXLEN];
                int  number_events = sscanf(command_line, "%s %s", command, name);

                if (number_events != 2) {
                    CHECK(sendto(terminal_fd, (void *) "Erro no comando\n", strlen("Erro no comando\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                } else {
                    USER new_user[MAXUSERS];
                    delete(users, new_user, name);
                    users = new_user;
                }
            }
        }
        close(terminal_fd);
        // dar tempo para o admin sair do processo dele
        sleep(2);
    }
    pthread_exit(NULL);
}