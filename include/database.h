#ifndef DATABASE_H
#define DATABASE_H

#include "component.h"

int db_find_index_by_id(const Component list[], int count, int id);
int db_find_index_by_code(const Component list[], int count, const char *code);

int db_add_component(Component list[], int *count, int *next_id,
                     const Component *input, Component *out_created,
                     char *err_msg, int err_len);

int db_update_component(Component list[], int count, int id,
                        const Component *input, Component *out_updated,
                        char *err_msg, int err_len);

int db_delete_component(Component list[], int *count, int id);

int db_search_filter(const Component list[], int count,
                     const char *query, const char *category,
                     Component results[], int max_results);

void db_compute_stats(const Component list[], int count, DatabaseStats *stats);

#endif /* DATABASE_H */
