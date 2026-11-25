#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../include/calculator.h"

static double nearest_e24_resistor(double r_ohm) {
    if (r_ohm <= 0.0) return 0.0;
    const double e24[] = {
        1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0,
        3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1
    };
    double exponent = floor(log10(r_ohm));
    double decade = pow(10.0, exponent);

    double best = e24[0] * decade;
    double min_diff = fabs(r_ohm - best);
    for (int k = 0; k < 24; k++) {
        double cand = e24[k] * decade;
        double diff = fabs(r_ohm - cand);
        if (diff < min_diff) {
            min_diff = diff;
            best = cand;
        }
    }
    double next_dec = 10.0 * decade;
    if (fabs(r_ohm - next_dec) < min_diff) {
        best = next_dec;
    }
    return best;
}

CalcResult calc_electronics(const char *mode,
                            double v, double i, double r,
                            double vf, double if_ma,
                            double r1, double r2, double c_uf) {
    CalcResult res;
    memset(&res, 0, sizeof(CalcResult));
    if (!mode || strlen(mode) == 0) {
        strcpy(res.error_msg, "Mode kalkulator tidak boleh kosong.");
        return res;
    }
    strncpy(res.mode, mode, sizeof(res.mode) - 1);

    if (strcmp(mode, "OHM_V") == 0) {
        if (i < 0.0 || r < 0.0) {
            strcpy(res.error_msg, "Arus (I) dan Hambatan (R) tidak boleh negatif.");
            return res;
        }
        res.valid = 1;
        strcpy(res.formula_used, "V = I * R  dan  P = V * I");
        strcpy(res.primary_label, "Tegangan (V)");
        res.primary_value = i * r;
        strcpy(res.primary_unit, "Volt (V)");
        strcpy(res.secondary_label, "Disipasi Daya (P)");
        res.secondary_value = res.primary_value * i;
        strcpy(res.secondary_unit, "Watt (W)");
        snprintf(res.recommendation, sizeof(res.recommendation),
                 "Gunakan komponen dengan rating tegangan minimal %.2f V (margin aman 1.5x).",
                 res.primary_value * 1.5);

    } else if (strcmp(mode, "OHM_I") == 0) {
        if (r <= 0.0) {
            strcpy(res.error_msg, "Hambatan (R) harus lebih besar dari 0 Ohm.");
            return res;
        }
        res.valid = 1;
        strcpy(res.formula_used, "I = V / R  dan  P = V^2 / R");
        strcpy(res.primary_label, "Arus Listrik (I)");
        res.primary_value = v / r;
        strcpy(res.primary_unit, "Ampere (A)");
        strcpy(res.secondary_label, "Disipasi Daya (P)");
        res.secondary_value = v * res.primary_value;
        strcpy(res.secondary_unit, "Watt (W)");
        snprintf(res.recommendation, sizeof(res.recommendation),
                 "Setara dengan %.2f mA. Pilih resistor dengan rating daya minimal %.3f W.",
                 res.primary_value * 1000.0, res.secondary_value * 2.0);

    } else if (strcmp(mode, "OHM_R") == 0) {
        if (i <= 0.0) {
            strcpy(res.error_msg, "Arus (I) harus lebih besar dari 0 Ampere.");
            return res;
        }
        res.valid = 1;
        strcpy(res.formula_used, "R = V / I  dan  P = V * I");
        strcpy(res.primary_label, "Hambatan (R)");
        res.primary_value = v / i;
        strcpy(res.primary_unit, "Ohm");
        strcpy(res.secondary_label, "Disipasi Daya (P)");
        res.secondary_value = v * i;
        strcpy(res.secondary_unit, "Watt (W)");
        snprintf(res.recommendation, sizeof(res.recommendation),
                 "Nilai resistor standar E24 terdekat: %.1f Ohm.",
                 nearest_e24_resistor(res.primary_value));

    } else if (strcmp(mode, "LED_RES") == 0) {
        if (v <= vf) {
            strcpy(res.error_msg, "Tegangan sumber (Vs) harus lebih besar dari tegangan maju LED (Vf).");
            return res;
        }
        if (if_ma <= 0.0) {
            strcpy(res.error_msg, "Arus target LED (If) harus lebih besar dari 0 mA (umumnya 10-20 mA).");
            return res;
        }
        double i_amp = if_ma / 1000.0;
        double r_exact = (v - vf) / i_amp;
        double p_diss = (v - vf) * i_amp;
        double r_std = nearest_e24_resistor(r_exact);

        res.valid = 1;
        strcpy(res.formula_used, "R = (Vs - Vf) / If");
        strcpy(res.primary_label, "Resistor Eksak");
        res.primary_value = r_exact;
        strcpy(res.primary_unit, "Ohm");
        strcpy(res.secondary_label, "Daya pada Resistor");
        res.secondary_value = p_diss;
        strcpy(res.secondary_unit, "Watt (W)");
        snprintf(res.recommendation, sizeof(res.recommendation),
                 "Gunakan resistor standar %.0f Ohm (daya min. %s).",
                 r_std, (p_diss * 2.0 <= 0.25) ? "1/4 Watt (0.25W)" : "1/2 Watt atau 1 Watt");

    } else if (strcmp(mode, "DIVIDER") == 0) {
        if (r1 < 0.0 || r2 < 0.0 || (r1 + r2) <= 0.0) {
            strcpy(res.error_msg, "Nilai R1 dan R2 tidak valid (total hambatan harus > 0).");
            return res;
        }
        double vout = v * (r2 / (r1 + r2));
        double i_bleed_ma = (v / (r1 + r2)) * 1000.0;

        res.valid = 1;
        strcpy(res.formula_used, "Vout = Vin * (R2 / (R1 + R2))");
        strcpy(res.primary_label, "Tegangan Keluaran (Vout)");
        res.primary_value = vout;
        strcpy(res.primary_unit, "Volt (V)");
        strcpy(res.secondary_label, "Arus Pembagi (I_div)");
        res.secondary_value = i_bleed_ma;
        strcpy(res.secondary_unit, "mA");
        snprintf(res.recommendation, sizeof(res.recommendation),
                 "Rasio pembagi: %.2f%% dari Vin. Cocok untuk pembacaan ADC sensor.",
                 (v > 0.0) ? ((vout / v) * 100.0) : 0.0);

    } else if (strcmp(mode, "SER_PAR") == 0) {
        if (r1 <= 0.0 || r2 <= 0.0) {
            strcpy(res.error_msg, "Masukkan nilai R1 dan R2 lebih besar dari 0 Ohm.");
            return res;
        }
        double r_ser = r1 + r2;
        double r_par = (r1 * r2) / (r1 + r2);

        res.valid = 1;
        strcpy(res.formula_used, "R_seri = R1 + R2  |  R_paralel = (R1 * R2) / (R1 + R2)");
        strcpy(res.primary_label, "Total Paralel (R_par)");
        res.primary_value = r_par;
        strcpy(res.primary_unit, "Ohm");
        strcpy(res.secondary_label, "Total Seri (R_seri)");
        res.secondary_value = r_ser;
        strcpy(res.secondary_unit, "Ohm");
        strcpy(res.recommendation,
                 "Rangkaian paralel selalu menghasilkan hambatan lebih kecil dari resistor terkecilnya.");

    } else if (strcmp(mode, "CAP_SER_P") == 0) {
        if (r1 <= 0.0 || r2 <= 0.0) {
            strcpy(res.error_msg, "Masukkan nilai C1 dan C2 lebih besar dari 0 uF.");
            return res;
        }
        double c_par = r1 + r2;
        double c_ser = (r1 * r2) / (r1 + r2);

        res.valid = 1;
        strcpy(res.formula_used, "C_paralel = C1 + C2  |  C_seri = (C1 * C2) / (C1 + C2)");
        strcpy(res.primary_label, "Total Paralel (C_par)");
        res.primary_value = c_par;
        strcpy(res.primary_unit, "uF");
        strcpy(res.secondary_label, "Total Seri (C_seri)");
        res.secondary_value = c_ser;
        strcpy(res.secondary_unit, "uF");
        strcpy(res.recommendation,
                 "Kapasitor paralel menjumlahkan kapasitas; kapasitor seri membagi kapasitas namun menaikkan batas tegangan.");

    } else if (strcmp(mode, "NE555") == 0) {
        if (r1 <= 0.0 || r2 <= 0.0 || c_uf <= 0.0) {
            strcpy(res.error_msg, "Masukkan R1 (Ohm), R2 (Ohm), dan C (uF) bernilai positif.");
            return res;
        }
        double c_farad = c_uf * 1e-6;
        double freq = 1.44 / ((r1 + 2.0 * r2) * c_farad);
        double duty = ((r1 + r2) / (r1 + 2.0 * r2)) * 100.0;

        res.valid = 1;
        strcpy(res.formula_used, "f = 1.44 / ((R1 + 2*R2) * C)");
        strcpy(res.primary_label, "Frekuensi Osilasi (f)");
        res.primary_value = freq;
        strcpy(res.primary_unit, "Hz");
        strcpy(res.secondary_label, "Duty Cycle High");
        res.secondary_value = duty;
        strcpy(res.secondary_unit, "%");
        snprintf(res.recommendation, sizeof(res.recommendation),
                 "Periode pulsa T = %.4f detik (High = %.4f s, Low = %.4f s).",
                 1.0 / freq, 0.693 * (r1 + r2) * c_farad, 0.693 * r2 * c_farad);
    } else {
        strcpy(res.error_msg, "Mode kalkulator tidak dikenali.");
    }

    return res;
}
