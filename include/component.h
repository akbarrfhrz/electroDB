#ifndef COMPONENT_H
#define COMPONENT_H

#define MAX_COMPONENTS 500

#define MAX_CODE_LEN   32
#define MAX_NAME_LEN   100
#define MAX_CAT_LEN    40
#define MAX_SUBCAT_LEN 64
#define MAX_MPN_LEN    48
#define MAX_MFR_LEN    48

#define MAX_VAL_LEN    48
#define MAX_TOL_LEN    32
#define MAX_VOLT_LEN   40
#define MAX_CURR_LEN   40
#define MAX_POW_LEN    40
#define MAX_TEMP_LEN   36
#define MAX_PKG_LEN    48
#define MAX_DIM_LEN    64
#define MAX_PINOUT_LEN 200

#define MAX_FUNC_LEN   240
#define MAX_APP_LEN    200
#define MAX_NOTE_LEN   240

#define MAX_PRICE_LEN  64
#define MAX_BUY_LEN    180
#define MAX_URL_LEN    180

#define DB_FILE_PATH "data/components.dat"

typedef struct {
    int id;

    /* 1. Identitas & Klasifikasi */
    char code[MAX_CODE_LEN];
    char name[MAX_NAME_LEN];
    char category[MAX_CAT_LEN];
    char sub_category[MAX_SUBCAT_LEN];
    char mpn[MAX_MPN_LEN];
    char manufacturer[MAX_MFR_LEN];

    /* 2. Spesifikasi Elektrikal & Fisik Terkecil */
    char nominal_value[MAX_VAL_LEN];
    char tolerance[MAX_TOL_LEN];
    char voltage_rating[MAX_VOLT_LEN];
    char current_rating[MAX_CURR_LEN];
    char power_rating[MAX_POW_LEN];
    char temp_range[MAX_TEMP_LEN];
    char package_type[MAX_PKG_LEN];
    char dimensions[MAX_DIM_LEN];
    int  pin_count;
    char pinout[MAX_PINOUT_LEN];

    /* 3. Fungsi Kerja, Aplikasi & Catatan Pemasangan */
    char function_desc[MAX_FUNC_LEN];
    char application[MAX_APP_LEN];
    char usage_notes[MAX_NOTE_LEN];

    /* 4. Referensi Pasar & Datasheet */
    char price_estimate[MAX_PRICE_LEN];
    char where_to_buy[MAX_BUY_LEN];
    char datasheet_url[MAX_URL_LEN];
} Component;

typedef struct {
    int total_components;
    int total_datasheets;
    char top_category[MAX_CAT_LEN];
    int top_category_count;
    int resistor_count;
    int capacitor_count;
    int diode_count;
    int transistor_count;
    int ic_count;
    int sensor_module_count;
} DatabaseStats;

#endif /* COMPONENT_H */
