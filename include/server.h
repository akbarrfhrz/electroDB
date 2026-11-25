#ifndef SERVER_H
#define SERVER_H

#include "component.h"

int server_start(int port, Component list[], int *count, int *next_id);

#endif /* SERVER_H */
