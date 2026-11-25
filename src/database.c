#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../include/database.h"

static void str_to_lower(const char *src, char *dst, int max_len) {
    int i = 0;
    for (; src[i] != '\0' && i < max_len - 1; i++) {
        dst[i] = (char)tolower((unsigned char)src[i]);
    }
    dst[i] = '\0';
}

static int str_contains_icase(const char *haystack, const char *needle) {
    if (!needle || needle[0] == '\0') return 1;
    if (!haystack) return 0;

    char h_low[512];
    char n_low[256];
    str_to_lower(haystack, h_low, sizeof(h_low));
    str_to_lower(needle, n_low, sizeof(n_low));
    return strstr(h_low, n_low) != NULL;
}

static int str_equals_icase(const char *a, const char *b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static void safe_copy(char *dst, const char *src, int max_len) {
    if (!src) {
        dst[0] = '\0';
        return;
    }
    snprintf(dst, (size_t)max_len, "%s", src);
}

static void safe_copy_or_default(char *dst, const char *src, const char *fallback, int max_len) {
    if (!src || strlen(src) == 0) {
        safe_copy(dst, fallback, max_len);
    } else {
        safe_copy(dst, src, max_len);
    }
}

int db_find_index_by_id(const Component list[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (list[i].id == id) {
            return i;
        }
    }
    return -1;
}

int db_find_index_by_code(const Component list[], int count, const char *code) {
    for (int i = 0; i < count; i++) {
        if (str_equals_icase(list[i].code, code)) {
            return i;
        }
    }
    return -1;
}

static void populate_component_fields(Component *dst, const Component *src) {
    safe_copy(dst->code, src->code, MAX_CODE_LEN);
    safe_copy(dst->name, src->name, MAX_NAME_LEN);
    safe_copy_or_default(dst->category, src->category, "Sensor & Modul", MAX_CAT_LEN);
    safe_copy_or_default(dst->sub_category, src->sub_category, "-", MAX_SUBCAT_LEN);
    safe_copy_or_default(dst->mpn, src->mpn, "-", MAX_MPN_LEN);
    safe_copy_or_default(dst->manufacturer, src->manufacturer, "-", MAX_MFR_LEN);

    safe_copy_or_default(dst->nominal_value, src->nominal_value, "-", MAX_VAL_LEN);
    safe_copy_or_default(dst->tolerance, src->tolerance, "-", MAX_TOL_LEN);
    safe_copy_or_default(dst->voltage_rating, src->voltage_rating, "-", MAX_VOLT_LEN);
    safe_copy_or_default(dst->current_rating, src->current_rating, "-", MAX_CURR_LEN);
    safe_copy_or_default(dst->power_rating, src->power_rating, "-", MAX_POW_LEN);
    safe_copy_or_default(dst->temp_range, src->temp_range, "-", MAX_TEMP_LEN);
    safe_copy_or_default(dst->package_type, src->package_type, "Standard", MAX_PKG_LEN);
    safe_copy_or_default(dst->dimensions, src->dimensions, "-", MAX_DIM_LEN);
    dst->pin_count = (src->pin_count >= 0 && src->pin_count <= 1024) ? src->pin_count : 0;
    safe_copy_or_default(dst->pinout, src->pinout, "-", MAX_PINOUT_LEN);

    safe_copy_or_default(dst->function_desc, src->function_desc, "-", MAX_FUNC_LEN);
    safe_copy_or_default(dst->application, src->application, "-", MAX_APP_LEN);
    safe_copy_or_default(dst->usage_notes, src->usage_notes, "-", MAX_NOTE_LEN);

    safe_copy_or_default(dst->price_estimate, src->price_estimate, "Hubungi penjual", MAX_PRICE_LEN);
    safe_copy_or_default(dst->where_to_buy, src->where_to_buy, "Toko Elektronika / Marketplace", MAX_BUY_LEN);
    safe_copy(dst->datasheet_url, src->datasheet_url, MAX_URL_LEN);
}

int db_add_component(Component list[], int *count, int *next_id,
                     const Component *input, Component *out_created,
                     char *err_msg, int err_len) {
    if (*count >= MAX_COMPONENTS) {
        safe_copy(err_msg, "Kapasitas database penuh (maksimum 500 komponen).", err_len);
        return 0;
    }
    if (!input || strlen(input->code) == 0 || strlen(input->name) == 0) {
        safe_copy(err_msg, "Kode/Part Number dan Nama Komponen wajib diisi.", err_len);
        return 0;
    }
    if (db_find_index_by_code(list, *count, input->code) != -1) {
        safe_copy(err_msg, "Kode komponen tersebut sudah ada di katalog referensi.", err_len);
        return 0;
    }

    Component item;
    memset(&item, 0, sizeof(Component));
    item.id = (*next_id)++;
    populate_component_fields(&item, input);

    list[*count] = item;
    (*count)++;

    if (out_created) {
        *out_created = item;
    }
    return 1;
}

int db_update_component(Component list[], int count, int id,
                        const Component *input, Component *out_updated,
                        char *err_msg, int err_len) {
    int idx = db_find_index_by_id(list, count, id);
    if (idx == -1) {
        safe_copy(err_msg, "Referensi komponen dengan ID tersebut tidak ditemukan.", err_len);
        return 0;
    }
    if (!input || strlen(input->code) == 0 || strlen(input->name) == 0) {
        safe_copy(err_msg, "Kode dan Nama Komponen tidak boleh kosong.", err_len);
        return 0;
    }

    int existing_code_idx = db_find_index_by_code(list, count, input->code);
    if (existing_code_idx != -1 && existing_code_idx != idx) {
        safe_copy(err_msg, "Kode komponen bentrok dengan referensi lain.", err_len);
        return 0;
    }

    populate_component_fields(&list[idx], input);

    if (out_updated) {
        *out_updated = list[idx];
    }
    return 1;
}

int db_delete_component(Component list[], int *count, int id) {
    int idx = db_find_index_by_id(list, *count, id);
    if (idx == -1) {
        return 0;
    }
    for (int i = idx; i < (*count) - 1; i++) {
        list[i] = list[i + 1];
    }
    (*count)--;
    return 1;
}

int db_search_filter(const Component list[], int count,
                     const char *query, const char *category,
                     Component results[], int max_results) {
    int found = 0;
    int filter_cat = (category && strlen(category) > 0 && !str_equals_icase(category, "Semua"));

    for (int i = 0; i < count && found < max_results; i++) {
        if (filter_cat && !str_equals_icase(list[i].category, category)) {
            continue;
        }
        if (query && strlen(query) > 0) {
            int match = str_contains_icase(list[i].name, query) ||
                        str_contains_icase(list[i].code, query) ||
                        str_contains_icase(list[i].sub_category, query) ||
                        str_contains_icase(list[i].mpn, query) ||
                        str_contains_icase(list[i].manufacturer, query) ||
                        str_contains_icase(list[i].nominal_value, query) ||
                        str_contains_icase(list[i].package_type, query) ||
                        str_contains_icase(list[i].function_desc, query) ||
                        str_contains_icase(list[i].application, query) ||
                        str_contains_icase(list[i].where_to_buy, query);
            if (!match) {
                continue;
            }
        }
        results[found++] = list[i];
    }
    return found;
}

void db_compute_stats(const Component list[], int count, DatabaseStats *stats) {
    memset(stats, 0, sizeof(DatabaseStats));
    stats->total_components = count;
    strcpy(stats->top_category, "-");

    for (int i = 0; i < count; i++) {
        if (strlen(list[i].datasheet_url) > 4) {
            stats->total_datasheets++;
        }

        if (str_equals_icase(list[i].category, "Resistor")) {
            stats->resistor_count++;
        } else if (str_equals_icase(list[i].category, "Kapasitor")) {
            stats->capacitor_count++;
        } else if (str_equals_icase(list[i].category, "Dioda")) {
            stats->diode_count++;
        } else if (str_equals_icase(list[i].category, "Transistor")) {
            stats->transistor_count++;
        } else if (str_equals_icase(list[i].category, "IC")) {
            stats->ic_count++;
        } else {
            stats->sensor_module_count++;
        }
    }

    struct {
        const char *name;
        int cnt;
    } cats[] = {
        {"Resistor", stats->resistor_count},
        {"Kapasitor", stats->capacitor_count},
        {"Dioda", stats->diode_count},
        {"Transistor", stats->transistor_count},
        {"IC", stats->ic_count},
        {"Sensor & Modul", stats->sensor_module_count}
    };

    for (int i = 0; i < 6; i++) {
        if (cats[i].cnt > stats->top_category_count) {
            stats->top_category_count = cats[i].cnt;
            strcpy(stats->top_category, cats[i].name);
        }
    }
}
