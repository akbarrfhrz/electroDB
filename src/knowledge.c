#include "../include/knowledge.h"

static const KnowledgeArticle ARTICLES[] = {
    {
        1,
        "Identifikasi Komponen",
        "Cara Membaca Kode Warna Resistor (4-Gelang & 5-Gelang)",
        "Panduan cepat menerjemahkan gelang warna pada badan resistor axial tanpa harus selalu mengukur dengan multimeter.",
        "Pada resistor 4-gelang: Gelang 1 adalah digit pertama, Gelang 2 adalah digit kedua, Gelang 3 adalah faktor pengali (10^n), dan Gelang 4 adalah toleransi (Emas = +-5%, Perak = +-10%). Urutan warna standar: Hitam(0), Cokelat(1), Merah(2), Oranye(3), Kuning(4), Hijau(5), Biru(6), Ungu(7), Abu-abu(8), Putih(9). Contoh: Cokelat - Hitam - Merah - Emas berarti 10 x 10^2 = 1.000 Ohm (1 kOhm) toleransi 5%.",
        "Nilai = (Digit1 * 10 + Digit2) * 10^(Gelang3) Ohm"
    },
    {
        2,
        "Identifikasi Komponen",
        "Cara Membaca Kode Angka Kapasitor Keramik & Mylar",
        "Memahami arti kode 3 digit seperti 104, 103, atau 222 pada kapasitor non-polar.",
        "Kapasitor keramik dan film menggunakan sistem 3 digit dalam satuan pikofarad (pF). Dua digit pertama adalah nilai signifikan dan digit ketiga adalah jumlah angka nol di belakangnya. Contoh: Kode '104' = 10 diikuti 4 nol = 100.000 pF = 100 nF = 0.1 uF. Kode '103' = 10 nF (0.01 uF). Kode '222' = 2.200 pF = 2.2 nF.",
        "1 uF = 1.000 nF = 1.000.000 pF"
    },
    {
        3,
        "Semikonduktor",
        "Perbedaan Transistor BJT (2N2222A) vs MOSFET (IRFZ44N)",
        "Kapan memilih BJT dan kapan wajib menggunakan MOSFET dalam perancangan driver aktuator.",
        "Transistor BJT (Bipolar Junction Transistor) dikendalikan oleh ARUS pada kaki Base (Ib), sehingga membutuhkan resistor pembatas pada Base dan cocok untuk beban arus kecil-menengah (< 500 mA). Sebaliknya, MOSFET dikendalikan oleh TEGANGAN pada kaki Gate (Vgs) dengan impedansi input sangat tinggi dan hambatan kanal Rds(on) sangat kecil (mili-ohm), sehingga jauh lebih dingin dan efisien untuk beban arus besar seperti motor DC atau PWM daya tinggi.",
        "BJT: Ic = hFE * Ib  |  MOSFET: Saklar aktif saat Vgs > Vgs(th)"
    },
    {
        4,
        "Dokumentasi & Datasheet",
        "4 Bagian Wajib Dibaca di Datasheet Komponen",
        "Strategi membaca dokumen PDF datasheet pabrikan (TI, Vishay, ON Semi, Espressif) agar komponen tidak rusak.",
        "1) Absolute Maximum Ratings: Batas mutlak tegangan, arus, dan suhu sebelum komponen rusak permanen (jangan pernah dijalankan terus-menerus di angka ini). 2) Recommended Operating Conditions: Rentang kerja normal yang dijamin pabrikan. 3) Pin Configuration / Pinout: Peta posisi kaki komponen (perhatikan tanda titik/lekukan pada IC DIP-8 atau sisi logam pada TO-220). 4) Typical Application Circuit: Skema rangkaian standar yang direkomendasikan pabrikan.",
        "Aturan Derating Aman: Tegangan kerja <= 50-70% dari Absolute Max Rating"
    },
    {
        5,
        "Proteksi Rangkaian",
        "Dioda Flyback & Resistor Pull-Up / Pull-Down",
        "Dua teknik proteksi dasar agar mikrokontroler tidak reset atau rusak saat mengendalikan relay dan tombol.",
        "Saat beban induktif (relay, solenoid, motor DC) dimatikan mendadak, kumparan menghasilkan lonjakan tegangan balik tinggi (back-EMF). Pasang dioda 1N4007 secara paralel terbalik (Katoda ke +VCC) untuk meredam lonjakan tersebut. Sementara pada input tombol/sensor digital, gunakan resistor Pull-Up atau Pull-Down (4.7 kOhm - 10 kOhm) agar pin tidak mengambang (floating) dan memicu pembacaan acak.",
        "Dioda Flyback: Katoda ke +V Suplai, Anoda ke Drain/Collector"
    },
    {
        6,
        "Panduan Belanja & Substitusi",
        "Tips Memilih & Membeli Komponen Elektronika yang Tepat",
        "Panduan praktis sebelum membeli komponen di toko offline maupun marketplace online.",
        "Selalu periksa jenis kemasan (Through-Hole DIP/Axial/Radial untuk breadboard & PCB lubang, vs SMD 0805/SOIC untuk PCB cetak pabrik). Perhatikan suffix kode belakang IC (misal 'N' atau 'P' menandakan kemasan DIP lubang, sedangkan 'D' atau 'S' sering menandakan SMD). Untuk kapasitor elektrolit, boleh mengganti dengan nilai tegangan (Volt) yang lebih tinggi (misal 16V diganti 25V atau 50V) selama kapasitas (uF) sama dan muat di papan rangkaian.",
        "Substitusi Aman Kapasitor: uF sama, Rating Volt >= Volt asli"
    }
};

int knowledge_get_all(const KnowledgeArticle **out_articles) {
    *out_articles = ARTICLES;
    return (int)(sizeof(ARTICLES) / sizeof(ARTICLES[0]));
}
