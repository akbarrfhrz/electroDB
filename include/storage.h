#ifndef STORAGE_H
#define STORAGE_H

#include "component.h"

void storage_ensure_dir(void);
int storage_load(Component list[], int *count, int *next_id);
int storage_save(const Component list[], int count, int next_id);
int storage_reset_default(Component list[], int *count, int *next_id);
void storage_seed_defaults(Component list[], int *count, int *next_id);

#endif /* STORAGE_H */
