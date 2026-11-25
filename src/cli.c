#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cli.h"
#include "../include/storage.h"
#include "../include/database.h"
#include "../include/calculator.h"
#include "../include/knowledge.h"

static void trim_newline(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

static void read_line(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin)) {
        trim_newline(buf);
    } else {
        buf[0] = '\0';
    }
}

static void read_line_keep(const char *label, char *buf, int size) {
    char tmp[256];
    printf("%s [%s]: ", label, buf);
    if (fgets(tmp, sizeof(tmp), stdin)) {
        trim_newline(tmp);
        if (strlen(tmp) > 0) {
            snprintf(buf, (size_t)size, "%s", tmp);
        }
    }
}

static void print_table(const Component list[], int count) {
    if (count == 0) {
        printf("\n[Info] Tidak ada referensi komponen ditemukan.\n");
        return;
    }
    printf("\n%-3s | %-15s | %-30s | %-13s | %-15s | %-20s\n",
           "ID", "KODE / PART", "NAMA KOMPONEN", "KATEGORI", "NILAI UTAMA", "ESTIMASI HARGA");
    printf("----+-----------------+--------------------------------+---------------+-----------------+----------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-3d | %-15.15s | %-30.30s | %-13.13s | %-15.15s | %-20.20s\n",
               list[i].id, list[i].code, list[i].name,
               list[i].category, list[i].nominal_value, list[i].price_estimate);
    }
}

static void print_detail(const Component *c) {
    printf("\n================ INSPEKTOR DETAIL SPESIFIKASI TERKECIL ================\n");
    printf("[1] IDENTITAS & KLASIFIKASI PART\n");
    printf("  ID & Kode Internal : #%d | %s\n", c->id, c->code);
    printf("  Nama Komponen      : %s\n", c->name);
    printf("  Kategori / Sub     : %s (%s)\n", c->category, c->sub_category);
    printf("  Part Number (MPN)  : %s  |  Produsen: %s\n\n", c->mpn, c->manufacturer);

    printf("[2] SPESIFIKASI ELEKTRIKAL & FISIK TERKECIL\n");
    printf("  Nilai Nominal      : %s  (Toleransi: %s)\n", c->nominal_value, c->tolerance);
    printf("  Rating Tegangan    : %s\n", c->voltage_rating);
    printf("  Rating Arus        : %s\n", c->current_rating);
    printf("  Disipasi Daya/ESR  : %s\n", c->power_rating);
    printf("  Suhu Operasi       : %s\n", c->temp_range);
    printf("  Kemasan / Footprint: %s\n", c->package_type);
    printf("  Dimensi Fisik      : %s\n", c->dimensions);
    printf("  Jumlah Pin         : %d pin\n", c->pin_count);
    printf("  Konfigurasi Pinout : %s\n\n", c->pinout);

    printf("[3] FUNGSI KERJA, APLIKASI & CATATAN PEMASANGAN\n");
    printf("  Fungsi Utama       : %s\n", c->function_desc);
    printf("  Contoh Aplikasi    : %s\n", c->application);
    printf("  Catatan Teknis     : %s\n\n", c->usage_notes);

    printf("[4] REFERENSI PASAR & DATASHEET RESMI\n");
    printf("  Estimasi Harga     : %s\n", c->price_estimate);
    printf("  Rekomendasi Beli   : %s\n", c->where_to_buy);
    printf("  Datasheet PDF      : %s\n", c->datasheet_url);
    printf("=======================================================================\n");
}

void cli_run(Component list[], int *count, int *next_id) {
    int choice = 0;
    char input_buf[128];

    while (1) {
        printf("\n=======================================================================\n");
        printf("  ELECTRODB CLI - KATALOG REFERENSI & KALKULATOR ELEKTRONIKA (C11)\n");
        printf("  Kelompok 7 | D4 Teknologi Rekayasa Otomasi — Universitas Diponegoro\n");
        printf("=======================================================================\n");
        printf("1. Tambah Referensi Komponen Baru (Ringkas + Spesifikasi Terkecil)\n");
        printf("2. Tampilkan Semua Referensi & Buka Inspektor Spesifikasi Terkecil\n");
        printf("3. Cari Komponen (Nama / Kode / MPN / Nilai / Aplikasi)\n");
        printf("4. Edit Referensi Komponen\n");
        printf("5. Hapus Referensi Komponen\n");
        printf("6. Kalkulator Elektronika (Ohm, LED E24, Voltage Divider, NE555)\n");
        printf("7. Statistik Database & Ensiklopedia Pengetahuan Elektronika\n");
        printf("8. Reset Database ke Data Awal (menggunakan remove())\n");
        printf("9. Keluar & Simpan ke File\n");
        printf("-----------------------------------------------------------------------\n");
        printf("Pilih menu [1-9]: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("[!] Masukkan angka 1 sampai 9.\n");
            continue;
        }
        while (getchar() != '\n');

        if (choice == 1) {
            Component comp;
            memset(&comp, 0, sizeof(Component));
            char err[160];

            printf("\n--- [Bagian 1] Identitas & Informasi Utama ---\n");
            read_line("Kode / Part Internal (mis. RES-4K7) : ", comp.code, sizeof(comp.code));
            read_line("Nama Komponen                       : ", comp.name, sizeof(comp.name));
            read_line("Kategori (Resistor/Kapasitor/IC/dll): ", comp.category, sizeof(comp.category));
            read_line("Sub-Kategori Teknis                 : ", comp.sub_category, sizeof(comp.sub_category));
            read_line("Manufacturer Part Number (MPN)      : ", comp.mpn, sizeof(comp.mpn));
            read_line("Produsen / Merek                    : ", comp.manufacturer, sizeof(comp.manufacturer));

            printf("\n--- [Bagian 2] Spesifikasi Elektrikal & Fisik Terkecil (Enter untuk '-') ---\n");
            read_line("Nilai Nominal Utama (mis. 4.7 kOhm) : ", comp.nominal_value, sizeof(comp.nominal_value));
            read_line("Toleransi / Akurasi (mis. +-1%)     : ", comp.tolerance, sizeof(comp.tolerance));
            read_line("Rating Tegangan Maksimum            : ", comp.voltage_rating, sizeof(comp.voltage_rating));
            read_line("Rating Arus Maksimum                : ", comp.current_rating, sizeof(comp.current_rating));
            read_line("Disipasi Daya / ESR Maksimum        : ", comp.power_rating, sizeof(comp.power_rating));
            read_line("Rentang Suhu Operasi                : ", comp.temp_range, sizeof(comp.temp_range));
            read_line("Kemasan / Package (Axial/DIP-8/SMD) : ", comp.package_type, sizeof(comp.package_type));
            read_line("Dimensi Fisik (mm)                  : ", comp.dimensions, sizeof(comp.dimensions));
            read_line("Jumlah Pin / Kaki (angka)           : ", input_buf, sizeof(input_buf));
            comp.pin_count = atoi(input_buf);
            read_line("Konfigurasi Pinout Lengkap          : ", comp.pinout, sizeof(comp.pinout));

            printf("\n--- [Bagian 3 & 4] Fungsi, Aplikasi, Harga & Datasheet ---\n");
            read_line("Fungsi Utama                        : ", comp.function_desc, sizeof(comp.function_desc));
            read_line("Contoh Aplikasi Rangkaian           : ", comp.application, sizeof(comp.application));
            read_line("Catatan Polaritas & Tips Pasang     : ", comp.usage_notes, sizeof(comp.usage_notes));
            read_line("Estimasi Harga Pasar                : ", comp.price_estimate, sizeof(comp.price_estimate));
            read_line("Rekomendasi Tempat Beli             : ", comp.where_to_buy, sizeof(comp.where_to_buy));
            read_line("URL Datasheet PDF Resmi             : ", comp.datasheet_url, sizeof(comp.datasheet_url));

            Component created;
            if (db_add_component(list, count, next_id, &comp, &created, err, sizeof(err))) {
                storage_save(list, *count, *next_id);
                printf("[Sukses] Referensi '%s' berhasil disimpan (ID: %d).\n", created.name, created.id);
            } else {
                printf("[Gagal] %s\n", err);
            }
        } else if (choice == 2) {
            print_table(list, *count);
            read_line("\nMasukkan ID untuk membuka DETAIL SPESIFIKASI TERKECIL (0 untuk kembali): ", input_buf, sizeof(input_buf));
            int id = atoi(input_buf);
            if (id > 0) {
                int idx = db_find_index_by_id(list, *count, id);
                if (idx != -1) print_detail(&list[idx]);
                else printf("[Info] ID %d tidak ditemukan.\n", id);
            }
        } else if (choice == 3) {
            char query[64];
            Component results[MAX_COMPONENTS];
            read_line("Kata kunci (nama/kode/MPN/nilai/aplikasi): ", query, sizeof(query));
            int found = db_search_filter(list, *count, query, "Semua", results, MAX_COMPONENTS);
            printf("\nDitemukan %d referensi:\n", found);
            print_table(results, found);
            if (found > 0) {
                read_line("\nMasukkan ID hasil pencarian untuk detail lengkap (0 untuk kembali): ", input_buf, sizeof(input_buf));
                int id = atoi(input_buf);
                if (id > 0) {
                    int idx = db_find_index_by_id(list, *count, id);
                    if (idx != -1) print_detail(&list[idx]);
                }
            }
        } else if (choice == 4) {
            read_line("Masukkan ID komponen yang akan diubah: ", input_buf, sizeof(input_buf));
            int id = atoi(input_buf);
            int idx = db_find_index_by_id(list, *count, id);
            if (idx == -1) {
                printf("[Gagal] ID %d tidak ditemukan.\n", id);
                continue;
            }
            Component comp = list[idx];
            char err[160];
            printf("Mengedit: %s (%s) — Tekan [ENTER] untuk mempertahankan nilai lama.\n", comp.name, comp.code);
            read_line_keep("Kode", comp.code, sizeof(comp.code));
            read_line_keep("Nama Komponen", comp.name, sizeof(comp.name));
            read_line_keep("Kategori", comp.category, sizeof(comp.category));
            read_line_keep("Sub-Kategori", comp.sub_category, sizeof(comp.sub_category));
            read_line_keep("MPN", comp.mpn, sizeof(comp.mpn));
            read_line_keep("Produsen", comp.manufacturer, sizeof(comp.manufacturer));
            read_line_keep("Nilai Nominal", comp.nominal_value, sizeof(comp.nominal_value));
            read_line_keep("Toleransi", comp.tolerance, sizeof(comp.tolerance));
            read_line_keep("Rating Tegangan", comp.voltage_rating, sizeof(comp.voltage_rating));
            read_line_keep("Rating Arus", comp.current_rating, sizeof(comp.current_rating));
            read_line_keep("Disipasi Daya", comp.power_rating, sizeof(comp.power_rating));
            read_line_keep("Suhu Operasi", comp.temp_range, sizeof(comp.temp_range));
            read_line_keep("Kemasan", comp.package_type, sizeof(comp.package_type));
            read_line_keep("Dimensi Fisik", comp.dimensions, sizeof(comp.dimensions));
            printf("Jumlah Pin [%d]: ", comp.pin_count);
            read_line("", input_buf, sizeof(input_buf));
            if (strlen(input_buf) > 0) comp.pin_count = atoi(input_buf);
            read_line_keep("Pinout", comp.pinout, sizeof(comp.pinout));
            read_line_keep("Fungsi Utama", comp.function_desc, sizeof(comp.function_desc));
            read_line_keep("Aplikasi", comp.application, sizeof(comp.application));
            read_line_keep("Catatan Teknis", comp.usage_notes, sizeof(comp.usage_notes));
            read_line_keep("Estimasi Harga", comp.price_estimate, sizeof(comp.price_estimate));
            read_line_keep("Tempat Beli", comp.where_to_buy, sizeof(comp.where_to_buy));
            read_line_keep("URL Datasheet", comp.datasheet_url, sizeof(comp.datasheet_url));

            if (db_update_component(list, *count, id, &comp, NULL, err, sizeof(err))) {
                storage_save(list, *count, *next_id);
                printf("[Sukses] Referensi ID %d diperbarui.\n", id);
            } else {
                printf("[Gagal] %s\n", err);
            }
        } else if (choice == 5) {
            read_line("Masukkan ID komponen yang akan dihapus: ", input_buf, sizeof(input_buf));
            int id = atoi(input_buf);
            int idx = db_find_index_by_id(list, *count, id);
            if (idx == -1) {
                printf("[Gagal] ID %d tidak ditemukan.\n", id);
                continue;
            }
            printf("Yakin hapus referensi '%s'? [y/n]: ", list[idx].name);
            read_line("", input_buf, sizeof(input_buf));
            if (input_buf[0] == 'y' || input_buf[0] == 'Y') {
                db_delete_component(list, count, id);
                storage_save(list, *count, *next_id);
                printf("[Sukses] Referensi dihapus dari file.\n");
            }
        } else if (choice == 6) {
            printf("\nPilih Kalkulator:\n");
            printf("1. Hukum Ohm (Hitung Arus I = V/R)\n");
            printf("2. Resistor Pembatas LED R = (Vs - Vf)/If + Standar E24\n");
            printf("3. Pembagi Tegangan Vout = Vin * R2/(R1+R2)\n");
            printf("4. Timer NE555 Astable (Frekuensi & Duty Cycle)\n");
            read_line("Pilihan [1-4]: ", input_buf, sizeof(input_buf));
            int c_mode = atoi(input_buf);
            CalcResult res;
            memset(&res, 0, sizeof(res));

            if (c_mode == 1) {
                read_line("Tegangan V (Volt): ", input_buf, sizeof(input_buf)); double v = atof(input_buf);
                read_line("Hambatan R (Ohm) : ", input_buf, sizeof(input_buf)); double r = atof(input_buf);
                res = calc_electronics("OHM_I", v, 0, r, 0, 0, 0, 0, 0);
            } else if (c_mode == 2) {
                read_line("Tegangan Sumber Vs (Volt): ", input_buf, sizeof(input_buf)); double vs = atof(input_buf);
                read_line("Tegangan LED Vf (Volt)   : ", input_buf, sizeof(input_buf)); double vf = atof(input_buf);
                read_line("Arus LED If (mA)         : ", input_buf, sizeof(input_buf)); double if_ma = atof(input_buf);
                res = calc_electronics("LED_RES", vs, 0, 0, vf, if_ma, 0, 0, 0);
            } else if (c_mode == 3) {
                read_line("Tegangan Input Vin (Volt): ", input_buf, sizeof(input_buf)); double vin = atof(input_buf);
                read_line("Resistor R1 (Ohm)        : ", input_buf, sizeof(input_buf)); double r1 = atof(input_buf);
                read_line("Resistor R2 (Ohm)        : ", input_buf, sizeof(input_buf)); double r2 = atof(input_buf);
                res = calc_electronics("DIVIDER", vin, 0, 0, 0, 0, r1, r2, 0);
            } else {
                read_line("Resistor R1 (Ohm): ", input_buf, sizeof(input_buf)); double r1 = atof(input_buf);
                read_line("Resistor R2 (Ohm): ", input_buf, sizeof(input_buf)); double r2 = atof(input_buf);
                read_line("Kapasitor C (uF) : ", input_buf, sizeof(input_buf)); double c_uf = atof(input_buf);
                res = calc_electronics("NE555", 0, 0, 0, 0, 0, r1, r2, c_uf);
            }

            if (res.valid) {
                printf("\n[Hasil] Rumus: %s\n", res.formula_used);
                printf("  %s : %.4f %s\n", res.primary_label, res.primary_value, res.primary_unit);
                printf("  %s : %.4f %s\n", res.secondary_label, res.secondary_value, res.secondary_unit);
                printf("  Rekomendasi : %s\n", res.recommendation);
            } else {
                printf("[Error] %s\n", res.error_msg);
            }
        } else if (choice == 7) {
            DatabaseStats st;
            db_compute_stats(list, *count, &st);
            printf("\n--- STATISTIK KATALOG REFERENSI ---\n");
            printf("Total Referensi Komponen : %d jenis\n", st.total_components);
            printf("Datasheet Terdokumentasi : %d tautan\n", st.total_datasheets);
            printf("Kategori Terbanyak       : %s (%d jenis)\n", st.top_category, st.top_category_count);

            const KnowledgeArticle *articles = NULL;
            int acount = knowledge_get_all(&articles);
            printf("\n--- RINGKASAN PENGETAHUAN ELEKTRONIKA (%d Topik) ---\n", acount);
            for (int i = 0; i < acount; i++) {
                printf("%d. [%s] %s\n   -> %s\n",
                       articles[i].id, articles[i].topic, articles[i].title, articles[i].formula_or_rule);
            }
        } else if (choice == 8) {
            storage_reset_default(list, count, next_id);
            printf("[Sukses] File database lama dihapus dengan remove() dan dikembalikan ke 10 data awal.\n");
        } else if (choice == 9) {
            storage_save(list, *count, *next_id);
            printf("Database disimpan ke %s. Terima kasih!\n", DB_FILE_PATH);
            break;
        } else {
            printf("Pilihan tidak valid [1-9].\n");
        }
    }
}
