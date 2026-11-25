#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET socket_t;
  #define CLOSE_SOCKET(s) closesocket(s)
#else
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  typedef int socket_t;
  #define INVALID_SOCKET (-1)
  #define CLOSE_SOCKET(s) close(s)
#endif

#include "../include/server.h"
#include "../include/storage.h"
#include "../include/database.h"
#include "../include/calculator.h"
#include "../include/knowledge.h"

#define REQ_BUF_SIZE  65536
#define RESP_BUF_SIZE 524288

static void json_escape(const char *src, char *dst, int max_len) {
    int j = 0;
    for (int i = 0; src[i] != '\0' && j < max_len - 3; i++) {
        unsigned char c = (unsigned char)src[i];
        if (c == '"' || c == '\\') {
            dst[j++] = '\\';
            dst[j++] = (char)c;
        } else if (c == '\n') {
            dst[j++] = '\\';
            dst[j++] = 'n';
        } else if (c == '\r') {
            dst[j++] = '\\';
            dst[j++] = 'r';
        } else if (c == '\t') {
            dst[j++] = '\\';
            dst[j++] = 't';
        } else if (c >= 32) {
            dst[j++] = (char)c;
        }
    }
    dst[j] = '\0';
}

static int json_get_string(const char *json, const char *key, char *out, int out_len) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *p = strstr(json, pattern);
    if (!p) {
        out[0] = '\0';
        return 0;
    }
    p += strlen(pattern);
    while (*p && (*p == ' ' || *p == ':' || *p == '\t' || *p == '\r' || *p == '\n')) p++;
    if (*p != '"') {
        out[0] = '\0';
        return 0;
    }
    p++;
    int i = 0;
    while (*p && *p != '"' && i < out_len - 1) {
        if (*p == '\\' && *(p + 1) != '\0') {
            p++;
            if (*p == 'n') { out[i++] = '\n'; p++; continue; }
            if (*p == 'r') { out[i++] = '\r'; p++; continue; }
            if (*p == 't') { out[i++] = '\t'; p++; continue; }
        }
        out[i++] = *p++;
    }
    out[i] = '\0';
    return 1;
}

static double json_get_number(const char *json, const char *key, double default_val) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *p = strstr(json, pattern);
    if (!p) return default_val;
    p += strlen(pattern);
    while (*p && (*p == ' ' || *p == ':' || *p == '\t' || *p == '\r' || *p == '\n' || *p == '"')) p++;
    if (*p == '\0') return default_val;
    return atof(p);
}

static void url_decode(const char *src, char *dst, int max_len) {
    int j = 0;
    for (int i = 0; src[i] != '\0' && j < max_len - 1; i++) {
        if (src[i] == '%' && isxdigit((unsigned char)src[i + 1]) && isxdigit((unsigned char)src[i + 2])) {
            char hex[3] = { src[i + 1], src[i + 2], '\0' };
            dst[j++] = (char)strtol(hex, NULL, 16);
            i += 2;
        } else if (src[i] == '+') {
            dst[j++] = ' ';
        } else {
            dst[j++] = src[i];
        }
    }
    dst[j] = '\0';
}

static void get_query_param(const char *query_str, const char *key, char *out, int out_len) {
    out[0] = '\0';
    if (!query_str) return;
    size_t klen = strlen(key);
    const char *p = query_str;
    while (*p) {
        if (strncmp(p, key, klen) == 0 && p[klen] == '=') {
            p += klen + 1;
            const char *end = strchr(p, '&');
            char raw[256];
            int len = end ? (int)(end - p) : (int)strlen(p);
            if (len >= (int)sizeof(raw)) len = (int)sizeof(raw) - 1;
            strncpy(raw, p, (size_t)len);
            raw[len] = '\0';
            url_decode(raw, out, out_len);
            return;
        }
        const char *next = strchr(p, '&');
        if (!next) break;
        p = next + 1;
    }
}

static void parse_component_payload(const char *body, Component *out) {
    memset(out, 0, sizeof(Component));
    json_get_string(body, "code", out->code, sizeof(out->code));
    json_get_string(body, "name", out->name, sizeof(out->name));
    json_get_string(body, "category", out->category, sizeof(out->category));
    json_get_string(body, "sub_category", out->sub_category, sizeof(out->sub_category));
    json_get_string(body, "mpn", out->mpn, sizeof(out->mpn));
    json_get_string(body, "manufacturer", out->manufacturer, sizeof(out->manufacturer));

    json_get_string(body, "nominal_value", out->nominal_value, sizeof(out->nominal_value));
    json_get_string(body, "tolerance", out->tolerance, sizeof(out->tolerance));
    json_get_string(body, "voltage_rating", out->voltage_rating, sizeof(out->voltage_rating));
    json_get_string(body, "current_rating", out->current_rating, sizeof(out->current_rating));
    json_get_string(body, "power_rating", out->power_rating, sizeof(out->power_rating));
    json_get_string(body, "temp_range", out->temp_range, sizeof(out->temp_range));
    json_get_string(body, "package_type", out->package_type, sizeof(out->package_type));
    json_get_string(body, "dimensions", out->dimensions, sizeof(out->dimensions));
    out->pin_count = (int)json_get_number(body, "pin_count", 0.0);
    json_get_string(body, "pinout", out->pinout, sizeof(out->pinout));

    json_get_string(body, "function_desc", out->function_desc, sizeof(out->function_desc));
    json_get_string(body, "application", out->application, sizeof(out->application));
    json_get_string(body, "usage_notes", out->usage_notes, sizeof(out->usage_notes));

    json_get_string(body, "price_estimate", out->price_estimate, sizeof(out->price_estimate));
    json_get_string(body, "where_to_buy", out->where_to_buy, sizeof(out->where_to_buy));
    json_get_string(body, "datasheet_url", out->datasheet_url, sizeof(out->datasheet_url));
}

static void component_to_json(const Component *c, char *buf, int buf_len) {
    char code_e[96], name_e[256], cat_e[96], sub_e[160], mpn_e[128], mfr_e[128];
    char val_e[128], tol_e[96], volt_e[96], curr_e[96], pow_e[96], temp_e[96];
    char pkg_e[128], dim_e[160], pinout_e[450];
    char func_e[512], app_e[450], note_e[512];
    char price_e[160], buy_e[400], url_e[400];

    json_escape(c->code, code_e, sizeof(code_e));
    json_escape(c->name, name_e, sizeof(name_e));
    json_escape(c->category, cat_e, sizeof(cat_e));
    json_escape(c->sub_category, sub_e, sizeof(sub_e));
    json_escape(c->mpn, mpn_e, sizeof(mpn_e));
    json_escape(c->manufacturer, mfr_e, sizeof(mfr_e));

    json_escape(c->nominal_value, val_e, sizeof(val_e));
    json_escape(c->tolerance, tol_e, sizeof(tol_e));
    json_escape(c->voltage_rating, volt_e, sizeof(volt_e));
    json_escape(c->current_rating, curr_e, sizeof(curr_e));
    json_escape(c->power_rating, pow_e, sizeof(pow_e));
    json_escape(c->temp_range, temp_e, sizeof(temp_e));
    json_escape(c->package_type, pkg_e, sizeof(pkg_e));
    json_escape(c->dimensions, dim_e, sizeof(dim_e));
    json_escape(c->pinout, pinout_e, sizeof(pinout_e));

    json_escape(c->function_desc, func_e, sizeof(func_e));
    json_escape(c->application, app_e, sizeof(app_e));
    json_escape(c->usage_notes, note_e, sizeof(note_e));

    json_escape(c->price_estimate, price_e, sizeof(price_e));
    json_escape(c->where_to_buy, buy_e, sizeof(buy_e));
    json_escape(c->datasheet_url, url_e, sizeof(url_e));

    snprintf(buf, (size_t)buf_len,
             "{\"id\":%d,\"code\":\"%s\",\"name\":\"%s\",\"category\":\"%s\","
             "\"sub_category\":\"%s\",\"mpn\":\"%s\",\"manufacturer\":\"%s\","
             "\"nominal_value\":\"%s\",\"tolerance\":\"%s\",\"voltage_rating\":\"%s\","
             "\"current_rating\":\"%s\",\"power_rating\":\"%s\",\"temp_range\":\"%s\","
             "\"package_type\":\"%s\",\"dimensions\":\"%s\",\"pin_count\":%d,\"pinout\":\"%s\","
             "\"function_desc\":\"%s\",\"application\":\"%s\",\"usage_notes\":\"%s\","
             "\"price_estimate\":\"%s\",\"where_to_buy\":\"%s\",\"datasheet_url\":\"%s\"}",
             c->id, code_e, name_e, cat_e, sub_e, mpn_e, mfr_e,
             val_e, tol_e, volt_e, curr_e, pow_e, temp_e, pkg_e, dim_e, c->pin_count, pinout_e,
             func_e, app_e, note_e, price_e, buy_e, url_e);
}

static void send_response(socket_t client, const char *status, const char *content_type,
                          const char *body, size_t body_len) {
    char header[1024];
    int hlen = snprintf(header, sizeof(header),
                        "HTTP/1.1 %s\r\n"
                        "Content-Type: %s\r\n"
                        "Content-Length: %zu\r\n"
                        "Access-Control-Allow-Origin: *\r\n"
                        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                        "Access-Control-Allow-Headers: Content-Type\r\n"
                        "Connection: close\r\n\r\n",
                        status, content_type, body_len);
    send(client, header, hlen, 0);
    if (body && body_len > 0) {
        send(client, body, (int)body_len, 0);
    }
}

static void send_json(socket_t client, const char *status, const char *json_body) {
    send_response(client, status, "application/json; charset=utf-8", json_body, strlen(json_body));
}

static void serve_static_file(socket_t client, const char *url_path) {
    if (strstr(url_path, "..") != NULL) {
        send_json(client, "403 Forbidden", "{\"error\":\"Akses ditolak\"}");
        return;
    }

    char filepath[256];
    if (strcmp(url_path, "/") == 0) {
        strcpy(filepath, "public/index.html");
    } else {
        snprintf(filepath, sizeof(filepath), "public%s", url_path);
    }

    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        send_json(client, "404 Not Found", "{\"error\":\"Halaman atau aset tidak ditemukan\"}");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fsize < 0 || fsize > 2097152) {
        fclose(fp);
        send_json(client, "500 Internal Server Error", "{\"error\":\"Ukuran file tidak valid\"}");
        return;
    }

    char *content = (char *)malloc((size_t)fsize + 1);
    if (!content) {
        fclose(fp);
        send_json(client, "500 Internal Server Error", "{\"error\":\"Memori tidak cukup\"}");
        return;
    }

    size_t read_bytes = fread(content, 1, (size_t)fsize, fp);
    fclose(fp);

    const char *mime = "text/plain; charset=utf-8";
    if (strstr(filepath, ".html")) mime = "text/html; charset=utf-8";
    else if (strstr(filepath, ".css")) mime = "text/css; charset=utf-8";
    else if (strstr(filepath, ".js")) mime = "application/javascript; charset=utf-8";
    else if (strstr(filepath, ".svg")) mime = "image/svg+xml";

    send_response(client, "200 OK", mime, content, read_bytes);
    free(content);
}

static void handle_client(socket_t client, Component list[], int *count, int *next_id) {
    char *req = (char *)calloc(1, REQ_BUF_SIZE);
    if (!req) {
        CLOSE_SOCKET(client);
        return;
    }

    int total_read = 0;
    int header_end = -1;
    int content_length = 0;

    while (total_read < REQ_BUF_SIZE - 1) {
        int n = recv(client, req + total_read, REQ_BUF_SIZE - 1 - total_read, 0);
        if (n <= 0) break;
        total_read += n;
        req[total_read] = '\0';

        if (header_end == -1) {
            char *p = strstr(req, "\r\n\r\n");
            if (p) {
                header_end = (int)(p - req) + 4;
                char *cl = strstr(req, "Content-Length:");
                if (!cl) cl = strstr(req, "content-length:");
                if (cl) {
                    content_length = atoi(cl + 15);
                }
            }
        }
        if (header_end != -1 && total_read >= header_end + content_length) {
            break;
        }
    }

    if (total_read <= 0) {
        free(req);
        CLOSE_SOCKET(client);
        return;
    }

    char method[16] = {0};
    char full_target[512] = {0};
    sscanf(req, "%15s %511s", method, full_target);

    if (strcmp(method, "OPTIONS") == 0) {
        send_response(client, "204 No Content", "text/plain", "", 0);
        free(req);
        CLOSE_SOCKET(client);
        return;
    }

    char path[256] = {0};
    char query_str[256] = {0};
    char *qmark = strchr(full_target, '?');
    if (qmark) {
        int plen = (int)(qmark - full_target);
        if (plen >= (int)sizeof(path)) plen = (int)sizeof(path) - 1;
        strncpy(path, full_target, (size_t)plen);
        path[plen] = '\0';
        snprintf(query_str, sizeof(query_str), "%s", qmark + 1);
    } else {
        snprintf(path, sizeof(path), "%s", full_target);
    }

    const char *body = (header_end != -1) ? (req + header_end) : "";

    if (strcmp(method, "GET") == 0 && strcmp(path, "/api/components") == 0) {
        char q[128], cat[64];
        get_query_param(query_str, "q", q, sizeof(q));
        get_query_param(query_str, "category", cat, sizeof(cat));

        Component filtered[MAX_COMPONENTS];
        int found = db_search_filter(list, *count, q, cat, filtered, MAX_COMPONENTS);

        char *resp = (char *)malloc(RESP_BUF_SIZE);
        int offset = snprintf(resp, RESP_BUF_SIZE, "{\"count\":%d,\"items\":[", found);
        for (int i = 0; i < found; i++) {
            char item_json[6144];
            component_to_json(&filtered[i], item_json, sizeof(item_json));
            offset += snprintf(resp + offset, (size_t)(RESP_BUF_SIZE - offset), "%s%s",
                               i > 0 ? "," : "", item_json);
        }
        snprintf(resp + offset, (size_t)(RESP_BUF_SIZE - offset), "]}");
        send_json(client, "200 OK", resp);
        free(resp);

    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/api/components") == 0) {
        Component payload, created;
        char err[160];
        parse_component_payload(body, &payload);

        if (db_add_component(list, count, next_id, &payload, &created, err, sizeof(err))) {
            storage_save(list, *count, *next_id);
            char item_json[6144], resp[8192];
            component_to_json(&created, item_json, sizeof(item_json));
            snprintf(resp, sizeof(resp), "{\"message\":\"Referensi komponen berhasil ditambahkan\",\"item\":%s}", item_json);
            send_json(client, "201 Created", resp);
        } else {
            char err_e[320], resp[512];
            json_escape(err, err_e, sizeof(err_e));
            snprintf(resp, sizeof(resp), "{\"error\":\"%s\"}", err_e);
            send_json(client, "400 Bad Request", resp);
        }

    } else if (strncmp(path, "/api/components/", 16) == 0) {
        int id = atoi(path + 16);
        if (strcmp(method, "GET") == 0) {
            int idx = db_find_index_by_id(list, *count, id);
            if (idx != -1) {
                char item_json[6144];
                component_to_json(&list[idx], item_json, sizeof(item_json));
                send_json(client, "200 OK", item_json);
            } else {
                send_json(client, "404 Not Found", "{\"error\":\"Referensi tidak ditemukan\"}");
            }
        } else if (strcmp(method, "PUT") == 0) {
            Component payload, updated;
            char err[160];
            parse_component_payload(body, &payload);

            if (db_update_component(list, *count, id, &payload, &updated, err, sizeof(err))) {
                storage_save(list, *count, *next_id);
                char item_json[6144], resp[8192];
                component_to_json(&updated, item_json, sizeof(item_json));
                snprintf(resp, sizeof(resp), "{\"message\":\"Referensi komponen diperbarui\",\"item\":%s}", item_json);
                send_json(client, "200 OK", resp);
            } else {
                char err_e[320], resp[512];
                json_escape(err, err_e, sizeof(err_e));
                snprintf(resp, sizeof(resp), "{\"error\":\"%s\"}", err_e);
                send_json(client, "400 Bad Request", resp);
            }
        } else if (strcmp(method, "DELETE") == 0) {
            if (db_delete_component(list, count, id)) {
                storage_save(list, *count, *next_id);
                send_json(client, "200 OK", "{\"message\":\"Referensi komponen berhasil dihapus\"}");
            } else {
                send_json(client, "404 Not Found", "{\"error\":\"Referensi komponen tidak ditemukan\"}");
            }
        } else {
            send_json(client, "405 Method Not Allowed", "{\"error\":\"Metode HTTP tidak didukung\"}");
        }

    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/api/stats") == 0) {
        DatabaseStats st;
        db_compute_stats(list, *count, &st);
        char resp[1024];
        snprintf(resp, sizeof(resp),
                 "{\"total_components\":%d,\"total_datasheets\":%d,"
                 "\"top_category\":\"%s\",\"top_category_count\":%d,"
                 "\"categories\":{\"Resistor\":%d,\"Kapasitor\":%d,\"Dioda\":%d,"
                 "\"Transistor\":%d,\"IC\":%d,\"Sensor & Modul\":%d}}",
                 st.total_components, st.total_datasheets,
                 st.top_category, st.top_category_count,
                 st.resistor_count, st.capacitor_count, st.diode_count,
                 st.transistor_count, st.ic_count, st.sensor_module_count);
        send_json(client, "200 OK", resp);

    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/api/calculator") == 0) {
        char mode[32] = {0};
        json_get_string(body, "mode", mode, sizeof(mode));
        double v = json_get_number(body, "v", 0.0);
        double i = json_get_number(body, "i", 0.0);
        double r = json_get_number(body, "r", 0.0);
        double vf = json_get_number(body, "vf", 0.0);
        double if_ma = json_get_number(body, "if_ma", 0.0);
        double r1 = json_get_number(body, "r1", 0.0);
        double r2 = json_get_number(body, "r2", 0.0);
        double c_uf = json_get_number(body, "c_uf", 0.0);

        CalcResult res = calc_electronics(mode, v, i, r, vf, if_ma, r1, r2, c_uf);
        char resp[1536];
        if (res.valid) {
            char rec_e[512];
            json_escape(res.recommendation, rec_e, sizeof(rec_e));
            snprintf(resp, sizeof(resp),
                     "{\"valid\":true,\"mode\":\"%s\",\"formula\":\"%s\","
                     "\"primary_label\":\"%s\",\"primary_value\":%.6f,\"primary_unit\":\"%s\","
                     "\"secondary_label\":\"%s\",\"secondary_value\":%.6f,\"secondary_unit\":\"%s\","
                     "\"recommendation\":\"%s\"}",
                     res.mode, res.formula_used,
                     res.primary_label, res.primary_value, res.primary_unit,
                     res.secondary_label, res.secondary_value, res.secondary_unit,
                     rec_e);
            send_json(client, "200 OK", resp);
        } else {
            char err_e[320];
            json_escape(res.error_msg, err_e, sizeof(err_e));
            snprintf(resp, sizeof(resp), "{\"valid\":false,\"error\":\"%s\"}", err_e);
            send_json(client, "400 Bad Request", resp);
        }

    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/api/knowledge") == 0) {
        const KnowledgeArticle *articles = NULL;
        int acount = knowledge_get_all(&articles);
        char *resp = (char *)malloc(RESP_BUF_SIZE);
        int offset = snprintf(resp, RESP_BUF_SIZE, "{\"count\":%d,\"articles\":[", acount);

        for (int i = 0; i < acount; i++) {
            char topic_e[128], title_e[256], sum_e[512], cont_e[2048], rule_e[512];
            json_escape(articles[i].topic, topic_e, sizeof(topic_e));
            json_escape(articles[i].title, title_e, sizeof(title_e));
            json_escape(articles[i].summary, sum_e, sizeof(sum_e));
            json_escape(articles[i].content, cont_e, sizeof(cont_e));
            json_escape(articles[i].formula_or_rule, rule_e, sizeof(rule_e));

            offset += snprintf(resp + offset, (size_t)(RESP_BUF_SIZE - offset),
                               "%s{\"id\":%d,\"topic\":\"%s\",\"title\":\"%s\","
                               "\"summary\":\"%s\",\"content\":\"%s\",\"formula_or_rule\":\"%s\"}",
                               i > 0 ? "," : "", articles[i].id, topic_e, title_e, sum_e, cont_e, rule_e);
        }
        snprintf(resp + offset, (size_t)(RESP_BUF_SIZE - offset), "]}");
        send_json(client, "200 OK", resp);
        free(resp);

    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/api/reset") == 0) {
        storage_reset_default(list, count, next_id);
        send_json(client, "200 OK", "{\"message\":\"Database referensi dikembalikan ke komponen bawaan\"}");

    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/api/export/csv") == 0) {
        char *csv = (char *)malloc(RESP_BUF_SIZE);
        int offset = snprintf(csv, RESP_BUF_SIZE,
                              "ID,Kode,Nama,Kategori,Sub_Kategori,MPN,Produsen,Nilai_Nominal,Toleransi,Tegangan_Maks,Arus_Maks,Daya_Maks,Suhu_Operasi,Kemasan,Dimensi,Jumlah_Pin,Pinout,Fungsi,Aplikasi,Catatan,Estimasi_Harga,Tempat_Beli,Datasheet\n");
        for (int i = 0; i < *count; i++) {
            offset += snprintf(csv + offset, (size_t)(RESP_BUF_SIZE - offset),
                               "%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
                               list[i].id, list[i].code, list[i].name, list[i].category,
                               list[i].sub_category, list[i].mpn, list[i].manufacturer,
                               list[i].nominal_value, list[i].tolerance, list[i].voltage_rating,
                               list[i].current_rating, list[i].power_rating, list[i].temp_range,
                               list[i].package_type, list[i].dimensions, list[i].pin_count, list[i].pinout,
                               list[i].function_desc, list[i].application, list[i].usage_notes,
                               list[i].price_estimate, list[i].where_to_buy, list[i].datasheet_url);
        }
        send_response(client, "200 OK", "text/csv; charset=utf-8", csv, strlen(csv));
        free(csv);

    } else if (strcmp(method, "GET") == 0) {
        serve_static_file(client, path);
    } else {
        send_json(client, "404 Not Found", "{\"error\":\"Endpoint tidak ditemukan\"}");
    }

    free(req);
    CLOSE_SOCKET(client);
}

int server_start(int port, Component list[], int *count, int *next_id) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "Gagal inisialisasi Winsock.\n");
        return 1;
    }
#endif

    socket_t server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        perror("socket gagal");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((unsigned short)port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind gagal");
        CLOSE_SOCKET(server_fd);
        return 1;
    }

    if (listen(server_fd, 16) < 0) {
        perror("listen gagal");
        CLOSE_SOCKET(server_fd);
        return 1;
    }

    printf("==================================================================\n");
    printf("  ELECTRODB — ENSIKLOPEDIA & DATABASE REFERENSI ELEKTRONIKA (C)\n");
    printf("  Kelompok 7 | D4 Teknologi Rekayasa Otomasi — UNDIP\n");
    printf("------------------------------------------------------------------\n");
    printf("  Buka Web UI di : http://localhost:%d\n", port);
    printf("  File Database  : %s (%d referensi komponen dimuat)\n", DB_FILE_PATH, *count);
    printf("  Mode CLI       : ./electrodb --cli\n");
    printf("==================================================================\n");
    fflush(stdout);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        socket_t client = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client == INVALID_SOCKET) {
            continue;
        }
        handle_client(client, list, count, next_id);
    }

    CLOSE_SOCKET(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
