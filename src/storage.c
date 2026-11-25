#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

#include "../include/storage.h"

typedef struct {
    char magic[8];
    int count;
    int next_id;
} FileHeader;

void storage_ensure_dir(void) {
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0755);
#endif
}

void storage_seed_defaults(Component list[], int *count, int *next_id) {
    *count = 0;
    *next_id = 1;

    Component defaults[] = {
        {
            1,
            "RES-10K-025",
            "Resistor Carbon Film 10k Ohm 1/4W",
            "Resistor",
            "Resistor Tetap (Carbon Film)",
            "CFR-25JB-52-10K",
            "Yageo",
            "10 kOhm",
            "+-5% (4-Gelang)",
            "250 V DC (Pulse 500V)",
            "5.0 mA (nominal)",
            "0.25 W (250 mW)",
            "-55 C s.d. +155 C",
            "Axial Through-Hole (0207)",
            "Panjang 6.5 mm x Diameter 2.3 mm",
            2,
            "Pin 1 & Pin 2: Non-polar (bolak-balik sama)",
            "Membatasi arus listrik, pembagi tegangan, serta resistor pull-up/pull-down pada jalur sinyal digital mikrokontroler.",
            "Pull-up tombol/I2C, pembagi tegangan sensor ADC, bias basis transistor BJT.",
            "Non-polar. Kode warna 4 gelang: Cokelat - Hitam - Oranye - Emas. Gunakan batas daya maks 50% (0.125W) agar tidak panas berlebih.",
            "Rp 100 - Rp 300 / pcs",
            "Toko Komponen Lokal (Candi Elektronik / Sinar Jaya Semarang), Tokopedia, Shopee",
            "https://www.vishay.com/docs/28705/cf.pdf"
        },
        {
            2,
            "CAP-100U-25",
            "Kapasitor Elektrolit (Elco) 100uF 25V",
            "Kapasitor",
            "Aluminum Electrolytic (Polar)",
            "UVR1E101MDD",
            "Nichicon",
            "100 uF",
            "+-20% @ 120Hz",
            "25 V DC",
            "Ripple Current 180 mA rms",
            "ESR ~0.45 Ohm @ 100kHz",
            "-40 C s.d. +105 C",
            "Radial Through-Hole",
            "Diameter 5.0 mm x Tinggi 11.0 mm (Pitch 2.0 mm)",
            2,
            "Pin Panjang: Anoda (+) | Pin Pendek & Garis Putih: Katoda (-)",
            "Menyaring riak (ripple) tegangan pada catu daya DC dan menyimpan muatan energi sementara untuk stabilisasi suplai IC.",
            "Filter output regulator LM7805/AMS1117, decoupling daya motor DC, rangkaian pewaktu.",
            "BERPOLARITAS! Jangan sampai terbalik polaritas (+) dan (-) karena dapat menyebabkan kapasitor meledak. Pilih rating tegangan minimal 2x tegangan kerja.",
            "Rp 500 - Rp 1.200 / pcs",
            "Toko Elektronika Lokal Semarang, Tokopedia, DigiKey",
            "https://www.nichicon.co.jp/english/products/pdfs/e-vr.pdf"
        },
        {
            3,
            "CAP-104-CER",
            "Kapasitor Keramik Multilayer 100nF (Kode 104)",
            "Kapasitor",
            "MLCC Ceramic (Non-Polar)",
            "RPE5C1H104K2P1Z03B",
            "Murata",
            "100 nF (0.1 uF)",
            "+-10% (Dielektrik X7R)",
            "50 V DC",
            "Arus Bocor < 10 nA",
            "Dissipation Factor <= 2.5%",
            "-55 C s.d. +125 C",
            "Radial Through-Hole",
            "5.0 mm x 3.5 mm x 2.5 mm (Pitch 2.54 mm)",
            2,
            "Pin 1 & Pin 2: Non-polar (bolak-balik bebas)",
            "Kapasitor decoupling/bypass untuk meredam noise frekuensi tinggi tepat di dekat pin VCC-GND IC digital maupun mikrokontroler.",
            "Bypass catu daya IC NE555/LM358/ESP32, filter low-pass sinyal analog, debouncing tombol.",
            "Pasang sedekat mungkin dengan pin VCC dan GND pada IC. Kode 3 digit '104' berarti 10 x 10^4 pF = 100.000 pF = 100 nF.",
            "Rp 150 - Rp 400 / pcs",
            "Toko Komponen Lokal Semarang, Tokopedia, Shopee",
            "https://www.murata.com/en-us/products/capacitor/ceramiccapacitor"
        },
        {
            4,
            "DIO-1N4007",
            "Dioda Penyearah Silikon 1N4007",
            "Dioda",
            "Standard Recovery Rectifier",
            "1N4007-E3/54",
            "Vishay",
            "1000V / 1A",
            "Vf max: 1.1 V @ 1A",
            "Vrrm: 1000 V (RMS 700V)",
            "If(AV): 1.0 A | Ifsm: 30 A",
            "Disipasi Daya Max: 3.0 W",
            "-55 C s.d. +175 C",
            "DO-41 Axial Through-Hole",
            "Panjang 5.2 mm x Diameter 2.7 mm",
            2,
            "Sisi Polos: Anoda (+) | Sisi Cincin Perak: Katoda (-)",
            "Menyearahkan arus AC ke DC pada rangkaian power supply serta menjadi dioda flyback pelindung transistor/MOSFET dari beban induktif.",
            "Penyearah jembatan (bridge rectifier) 50/60Hz, proteksi polaritas terbalik, dioda flyback relay/solenoid.",
            "Garis cincin perak pada bodi menandakan kaki Katoda (-). Tidak cocok untuk switching frekuensi tinggi (gunakan 1N4148 atau Schottky).",
            "Rp 300 - Rp 600 / pcs",
            "Toko Elektronika Lokal Semarang, Tokopedia, Shopee",
            "https://www.vishay.com/docs/88503/1n4001.pdf"
        },
        {
            5,
            "TRN-2N2222A",
            "Transistor Bipolar NPN 2N2222A",
            "Transistor",
            "Bipolar Junction Transistor (BJT NPN)",
            "P2N2222AG",
            "ON Semiconductor",
            "40V / 600mA",
            "hFE Gain: 100 - 300",
            "Vceo max: 40 V (Vcbo: 75V)",
            "Ic max: 600 mA",
            "Ptot max: 625 mW",
            "-55 C s.d. +150 C",
            "TO-92 (3-Pin Through-Hole)",
            "4.58 mm x 4.58 mm x 3.45 mm",
            3,
            "Sisi datar menghadap depan (kiri ke kanan): Pin 1 Emitter (E), Pin 2 Base (B), Pin 3 Collector (C)",
            "Saklar elektronik arus rendah-menengah (menggerakkan relay/LED dari pin GPIO) dan penguat sinyal kecil (common-emitter amplifier).",
            "Driver relay 5V, driver buzzer aktif, penguat sinyal audio/sensor awal, inverter logika.",
            "Wajib pasang resistor basis (mis. 1 kOhm) antara pin mikrokontroler dan kaki Base agar arus basis tidak merusak GPIO.",
            "Rp 800 - Rp 1.500 / pcs",
            "Toko Komponen Lokal Semarang, Tokopedia, Mouser",
            "https://www.onsemi.com/pdf/datasheet/p2n2222a-d.pdf"
        },
        {
            6,
            "MOS-IRFZ44N",
            "Power MOSFET N-Channel IRFZ44N",
            "Transistor",
            "N-Channel Enhancement MOSFET",
            "IRFZ44NPBF",
            "Infineon Technologies",
            "55V / 49A",
            "Rds(on): 17.5 mOhm @ Vgs=10V",
            "Vdss max: 55 V | Vgs: +-20 V",
            "Id kontinu: 49 A (Pulse: 160 A)",
            "Pd max: 94 W (@ Tc=25C)",
            "-55 C s.d. +175 C",
            "TO-220AB (3-Pin + Heatsink Tab)",
            "15.6 mm x 10.4 mm x 4.4 mm (Pitch 2.54 mm)",
            3,
            "Tampak depan (kiri ke kanan): Pin 1 Gate (G), Pin 2 Drain (D) & Tab Logam, Pin 3 Source (S)",
            "Saklar daya arus besar untuk mengendalikan motor DC, lampu LED daya tinggi, pemanas, dan modul PWM efisiensi tinggi.",
            "Driver motor DC kecepatan variabel (PWM), saklar beban 12V/24V, konverter DC-DC.",
            "Gunakan tegangan Gate 10V-12V untuk saturasi penuh (Rds_on minimum). Untuk sinyal 3.3V langsung, pilih MOSFET Logic-Level seperti IRLZ44N.",
            "Rp 4.500 - Rp 8.000 / pcs",
            "Toko Elektronika Lokal Semarang, Tokopedia, Shopee",
            "https://www.infineon.com/dgdl/irfz44npbf.pdf?fileId=5546d462533600a40153563b3a9f220d"
        },
        {
            7,
            "IC-NE555P",
            "IC Timer & Osilator Presisi NE555P",
            "IC",
            "Single Precision Timer IC",
            "NE555P",
            "Texas Instruments",
            "4.5V - 16V / 500kHz",
            "Timing Accuracy +-1%",
            "Vcc: 4.5 V s.d. 16 V DC",
            "Output Sink/Source: +-200 mA",
            "Disipasi Daya Max: 1180 mW",
            "0 C s.d. +70 C",
            "PDIP-8 (8-Pin Dual In-Line)",
            "9.81 mm x 6.35 mm (Pitch 2.54 mm)",
            8,
            "1:GND | 2:TRIG | 3:OUT | 4:RESET | 5:CONT | 6:THRES | 7:DISCH | 8:VCC",
            "Pembangkit pulsa pewaktu (monostable/one-shot), pembangkit gelombang kotak/PWM (astable), dan rangkaian delay timer analog.",
            "Generator sinyal clock, pengatur kecepatan motor PWM analog, timer tunda relay, pembangkit nada buzzer.",
            "Hubungkan Pin 4 (RESET) ke VCC jika tidak digunakan, dan pasang kapasitor keramik 10nF pada Pin 5 (CONT) ke GND untuk stabilitas.",
            "Rp 2.500 - Rp 5.000 / pcs",
            "Toko Elektronika Lokal Semarang, Tokopedia, Shopee, DigiKey",
            "https://www.ti.com/lit/ds/symlink/ne555.pdf"
        },
        {
            8,
            "IC-LM358N",
            "IC Dual Operational Amplifier LM358N",
            "IC",
            "Dual Low-Power Op-Amp",
            "LM358N/NOPB",
            "Texas Instruments",
            "3V - 32V / 1 MHz",
            "Vos max: 3 mV | GBW: 1 MHz",
            "3V - 32V Single (atau +-1.5V s.d. +-16V)",
            "Quiescent: 0.7 mA | Out: 40 mA",
            "Disipasi Daya Max: 830 mW",
            "0 C s.d. +70 C",
            "PDIP-8 (8-Pin Dual In-Line)",
            "9.81 mm x 6.35 mm (Pitch 2.54 mm)",
            8,
            "1:OUT1 | 2:IN1- | 3:IN1+ | 4:GND | 5:IN2+ | 6:IN2- | 7:OUT2 | 8:VCC",
            "Penguat sinyal sensor analog, komparator tegangan, filter aktif, dan buffer sinyal dengan catu daya tunggal (single supply).",
            "Pengkondisi sinyal sensor suhu/tekanan, sensor garis inframerah (komparator), penguat non-inverting.",
            "Tegangan output maksimum LM358 sekitar (Vcc - 1.5V), sehingga tidak rail-to-rail pada sisi positif.",
            "Rp 3.000 - Rp 5.500 / pcs",
            "Toko Elektronika Lokal Semarang, Tokopedia, Shopee",
            "https://www.ti.com/lit/ds/symlink/lm358.pdf"
        },
        {
            9,
            "IC-LM7805",
            "IC Regulator Tegangan Linear +5V LM7805",
            "IC",
            "Positive Fixed Linear Regulator",
            "LM7805CT",
            "ON Semiconductor",
            "+5V DC / 1.0A",
            "Output Tolerance +-4%",
            "Vin: 7 V - 25 V (Abs Max: 35 V)",
            "Iout max: 1.0 A (Peak: 2.2 A)",
            "Internal Thermal Overload Limit",
            "0 C s.d. +125 C",
            "TO-220 (3-Pin + GND Tab)",
            "15.6 mm x 10.4 mm x 4.4 mm (Pitch 2.54 mm)",
            3,
            "Tampak depan (kiri ke kanan): Pin 1 VIN (+7V..25V) | Pin 2 & Tab: GND | Pin 3 VOUT (+5.0V)",
            "Menurunkan dan menstabilkan tegangan DC tidak teregulasi (7V-24V) menjadi tegangan keluaran tetap +5V DC untuk rangkaian logika.",
            "Catu daya rangkaian TTL/CMOS 5V, suplai sensor ultrasonik/servo kecil dari adaptor 12V.",
            "Pasang kapasitor 0.33uF di input dan 0.1uF di output. Wajib pasang heatsink aluminium jika selisih (Vin - 5V) * Iout > 1 Watt.",
            "Rp 2.500 - Rp 4.500 / pcs",
            "Toko Elektronika Lokal Semarang, Tokopedia, Shopee",
            "https://www.onsemi.com/pdf/datasheet/mc7800-d.pdf"
        },
        {
            10,
            "MCU-ESP32-WROOM",
            "Modul Mikrokontroler Wi-Fi & BLE ESP32-WROOM-32",
            "Sensor & Modul",
            "32-Bit Dual-Core Wi-Fi/BLE SoC",
            "ESP32-WROOM-32E-N4",
            "Espressif Systems",
            "240 MHz / 3.3V",
            "ADC 12-bit (0 - 4095)",
            "3.0 V s.d. 3.6 V DC",
            "Peak TX RF: 500 mA",
            "Daya Operasi Max: 1.6 W",
            "-40 C s.d. +85 C",
            "SMD-38 Castellated / DevKit 30-Pin",
            "25.5 mm x 18.0 mm x 3.1 mm",
            38,
            "3V3, EN, GPIO0-39, UART0 TX/RX (GPIO1/3), I2C (GPIO21/22), SPI (GPIO18/19/23), GND",
            "Otak kendali sistem otomasi IoT, robotika, telemetri sensor nirkabel, dan pemrosesan sinyal PWM multi-kanal.",
            "Sistem kendali ROV/AUV, gateway telemetri IoT, kontrol aktuator cerdas berbasis web/Wi-Fi.",
            "Level logika GPIO adalah 3.3V (tidak toleran 5V langsung). Gunakan logic level converter saat terhubung ke sensor 5V.",
            "Rp 48.000 - Rp 65.000 / pcs",
            "Tokopedia, Shopee, Toko Robotika & Elektronika Semarang",
            "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf"
        }
    };

    int n = (int)(sizeof(defaults) / sizeof(defaults[0]));
    for (int i = 0; i < n && i < MAX_COMPONENTS; i++) {
        list[i] = defaults[i];
    }
    *count = n;
    *next_id = n + 1;
}

int storage_save(const Component list[], int count, int next_id) {
    storage_ensure_dir();
    FILE *fp = fopen(DB_FILE_PATH, "wb");
    if (!fp) {
        return 0;
    }

    FileHeader header;
    memset(&header, 0, sizeof(header));
    memcpy(header.magic, "ELDBv3", 7);
    header.count = count;
    header.next_id = next_id;

    if (fwrite(&header, sizeof(FileHeader), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }

    if (count > 0) {
        if ((int)fwrite(list, sizeof(Component), (size_t)count, fp) != count) {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

int storage_load(Component list[], int *count, int *next_id) {
    storage_ensure_dir();
    FILE *fp = fopen(DB_FILE_PATH, "rb");
    if (!fp) {
        storage_seed_defaults(list, count, next_id);
        storage_save(list, *count, *next_id);
        return 1;
    }

    FileHeader header;
    if (fread(&header, sizeof(FileHeader), 1, fp) != 1 ||
        strncmp(header.magic, "ELDBv3", 6) != 0 ||
        header.count < 0 || header.count > MAX_COMPONENTS) {
        fclose(fp);
        storage_seed_defaults(list, count, next_id);
        storage_save(list, *count, *next_id);
        return 1;
    }

    if (header.count > 0) {
        int read_items = (int)fread(list, sizeof(Component), (size_t)header.count, fp);
        if (read_items != header.count) {
            fclose(fp);
            return 0;
        }
    }

    *count = header.count;
    *next_id = header.next_id > 0 ? header.next_id : (header.count + 1);
    fclose(fp);
    return 1;
}

int storage_reset_default(Component list[], int *count, int *next_id) {
    remove(DB_FILE_PATH);
    storage_seed_defaults(list, count, next_id);
    return storage_save(list, *count, *next_id);
}
