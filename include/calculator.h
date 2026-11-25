#ifndef CALCULATOR_H
#define CALCULATOR_H

typedef struct {
    int valid;
    char mode[32];
    char formula_used[160];
    char primary_label[64];
    double primary_value;
    char primary_unit[24];
    char secondary_label[64];
    double secondary_value;
    char secondary_unit[24];
    char recommendation[220];
    char error_msg[160];
} CalcResult;

CalcResult calc_electronics(const char *mode,
                            double v, double i, double r,
                            double vf, double if_ma,
                            double r1, double r2, double c_uf);

#endif /* CALCULATOR_H */
