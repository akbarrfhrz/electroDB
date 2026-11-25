# ElectroDB — Portal Referensi Komponen, Kalkulator & Pengetahuan Elektronika (C Backend + Web UI)

Proyek Tugas Besar Mata Kuliah **Algoritma dan Pemrograman**  
**Program Studi Sarjana Terapan Teknologi Rekayasa Otomasi — Universitas Diponegoro**

## Anggota Kelompok 7
1. **Aurel Akbar Fahrezi** (`40040326630037`)
2. **Adythia Mardohar Lumbantobing** (`40040326630038`)
3. **Diko Solehandro Hadiatmaja** (`40040326630040`)

## Konsep & Fitur Utama
1. **Katalog Referensi Komponen & Inspektor Spesifikasi Terkecil (`src/database.c` & `src/storage.c`)**:
   - **Tampilan Awal Ringkas**: Menampilkan kode part, nama komponen, kategori, nilai nominal utama, kemasan (*package*), fungsi singkat, estimasi harga pasar, dan rekomendasi tempat membeli.
   - **Inspektor Detail Terkecil**: Ketika komponen dipilih/diklik, menampilkan 4 blok spesifikasi mendetail hingga parameter terkecil:
     - *Identitas & Klasifikasi*: Kode internal, Sub-kategori teknis, Manufacturer Part Number (MPN), dan Produsen.
     - *Spesifikasi Elektrikal & Fisik Terkecil*: Nilai nominal, toleransi/akurasi, rating tegangan maksimum, rating arus maksimum, disipasi daya maksimum, rentang suhu operasi, package/footprint, dimensi fisik (mm), jumlah pin, dan konfigurasi pinout lengkap.
     - *Fungsi, Aplikasi & Catatan Pemasangan*: Prinsip kerja, contoh penerapan rangkaian, serta peringatan polaritas/derating.
     - *Referensi Pasar & Dokumentasi*: Estimasi harga pasar, rekomendasi toko lokal (Semarang) & online, serta tautan **Datasheet PDF** resmi.
   - Mendukung penambahan, pencarian multi-atribut, pengeditan, penghapusan, ekspor CSV, dan reset default (`remove()`) yang tersimpan permanen di file biner `data/components.dat`.
2. **Kalkulator Perancangan Elektronika (`src/calculator.c`)**:
   - Resistor Pembatas LED + rekomendasi resistor standar seri E24 terdekat.
   - Pembagi Tegangan (*Voltage Divider*).
   - Hukum Ohm ($V, I, R$) & disipasi daya ($P$).
   - Resistor & Kapasitor Seri / Paralel.
   - Timer IC NE555 Mode Astable (Frekuensi $f$, Periode $T$, dan *Duty Cycle*).
3. **Ensiklopedia & Pengetahuan Elektronika (`src/knowledge.c`)**:
   - Panduan membaca kode warna resistor, kode angka kapasitor (`104`, `103`), perbandingan BJT vs MOSFET, cara membaca datasheet, proteksi dioda flyback, dan tips belanja/substitusi komponen.
4. **Dual Mode (Web Server HTTP & Terminal CLI)**:
   - Jalankan `./electrodb` untuk membuka antarmuka web di `http://localhost:8080`.
   - Jalankan `./electrodb --cli` untuk mode terminal CLI interaktif sesuai flowchart laporan Alpro.

## Cara Kompilasi & Menjalankan
### Linux / macOS / Android Termux
```bash
make
./electrodb        # Mode Web Server (http://localhost:8080)
./electrodb --cli  # Mode Terminal CLI
```

### Windows (MinGW / GCC)
```powershell
gcc -Wall -Wextra -std=c11 -O2 -Iinclude src/main.c src/storage.c src/database.c src/calculator.c src/knowledge.c src/cli.c src/server.c -o electrodb.exe -lws2_32
.\electrodb.exe
```
