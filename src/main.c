#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/component.h"
#include "../include/storage.h"
#include "../include/cli.h"
#include "../include/server.h"

int main(int argc, char *argv[]) {
    Component components[MAX_COMPONENTS];
    int count = 0;
    int next_id = 1;

    if (!storage_load(components, &count, &next_id)) {
        fprintf(stderr, "Gagal memuat file database %s\n", DB_FILE_PATH);
        return 1;
    }

    if (argc > 1 && strcmp(argv[1], "--cli") == 0) {
        cli_run(components, &count, &next_id);
        return 0;
    }

    int port = 8080;
    if (argc > 1 && atoi(argv[1]) > 0) {
        port = atoi(argv[1]);
    }

    return server_start(port, components, &count, &next_id);
}
