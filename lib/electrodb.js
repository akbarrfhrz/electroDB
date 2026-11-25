import defaults from './default-components.json' with { type: 'json' };

export const DEFAULT_COMPONENTS = defaults;

const COMPONENT_FIELDS = {
  code: [32, ''],
  name: [100, ''],
  category: [40, 'Sensor & Modul'],
  sub_category: [64, '-'],
  mpn: [48, '-'],
  manufacturer: [48, '-'],
  nominal_value: [48, '-'],
  tolerance: [32, '-'],
  voltage_rating: [40, '-'],
  current_rating: [40, '-'],
  power_rating: [40, '-'],
  temp_range: [36, '-'],
  package_type: [48, 'Standard'],
  dimensions: [64, '-'],
  pinout: [200, '-'],
  function_desc: [240, '-'],
  application: [200, '-'],
  usage_notes: [240, '-'],
  price_estimate: [64, 'Hubungi penjual'],
  where_to_buy: [180, 'Toko Elektronika / Marketplace'],
  datasheet_url: [180, ''],
};

const CATEGORIES = [
  'Resistor',
  'Kapasitor',
  'Dioda',
  'Transistor',
  'IC',
  'Sensor & Modul',
];

export const ARTICLES = [
  {
    id: 1,
    topic: 'Identifikasi Komponen',
    title: 'Cara Membaca Kode Warna Resistor (4-Gelang & 5-Gelang)',
    summary: 'Panduan cepat menerjemahkan gelang warna pada badan resistor axial tanpa harus selalu mengukur dengan multimeter.',
    content: 'Pada resistor 4-gelang: Gelang 1 adalah digit pertama, Gelang 2 adalah digit kedua, Gelang 3 adalah faktor pengali (10^n), dan Gelang 4 adalah toleransi (Emas = +-5%, Perak = +-10%). Urutan warna standar: Hitam(0), Cokelat(1), Merah(2), Oranye(3), Kuning(4), Hijau(5), Biru(6), Ungu(7), Abu-abu(8), Putih(9). Contoh: Cokelat - Hitam - Merah - Emas berarti 10 x 10^2 = 1.000 Ohm (1 kOhm) toleransi 5%.',
    formula_or_rule: 'Nilai = (Digit1 * 10 + Digit2) * 10^(Gelang3) Ohm',
  },
  {
    id: 2,
    topic: 'Identifikasi Komponen',
    title: 'Cara Membaca Kode Angka Kapasitor Keramik & Mylar',
    summary: 'Memahami arti kode 3 digit seperti 104, 103, atau 222 pada kapasitor non-polar.',
    content: "Kapasitor keramik dan film menggunakan sistem 3 digit dalam satuan pikofarad (pF). Dua digit pertama adalah nilai signifikan dan digit ketiga adalah jumlah angka nol di belakangnya. Contoh: Kode '104' = 10 diikuti 4 nol = 100.000 pF = 100 nF = 0.1 uF. Kode '103' = 10 nF (0.01 uF). Kode '222' = 2.200 pF = 2.2 nF.",
    formula_or_rule: '1 uF = 1.000 nF = 1.000.000 pF',
  },
  {
    id: 3,
    topic: 'Semikonduktor',
    title: 'Perbedaan Transistor BJT (2N2222A) vs MOSFET (IRFZ44N)',
    summary: 'Kapan memilih BJT dan kapan wajib menggunakan MOSFET dalam perancangan driver aktuator.',
    content: 'Transistor BJT (Bipolar Junction Transistor) dikendalikan oleh ARUS pada kaki Base (Ib), sehingga membutuhkan resistor pembatas pada Base dan cocok untuk beban arus kecil-menengah (< 500 mA). Sebaliknya, MOSFET dikendalikan oleh TEGANGAN pada kaki Gate (Vgs) dengan impedansi input sangat tinggi dan hambatan kanal Rds(on) sangat kecil (mili-ohm), sehingga jauh lebih dingin dan efisien untuk beban arus besar seperti motor DC atau PWM daya tinggi.',
    formula_or_rule: 'BJT: Ic = hFE * Ib  |  MOSFET: Saklar aktif saat Vgs > Vgs(th)',
  },
  {
    id: 4,
    topic: 'Dokumentasi & Datasheet',
    title: '4 Bagian Wajib Dibaca di Datasheet Komponen',
    summary: 'Strategi membaca dokumen PDF datasheet pabrikan (TI, Vishay, ON Semi, Espressif) agar komponen tidak rusak.',
    content: '1) Absolute Maximum Ratings: Batas mutlak tegangan, arus, dan suhu sebelum komponen rusak permanen (jangan pernah dijalankan terus-menerus di angka ini). 2) Recommended Operating Conditions: Rentang kerja normal yang dijamin pabrikan. 3) Pin Configuration / Pinout: Peta posisi kaki komponen (perhatikan tanda titik/lekukan pada IC DIP-8 atau sisi logam pada TO-220). 4) Typical Application Circuit: Skema rangkaian standar yang direkomendasikan pabrikan.',
    formula_or_rule: 'Aturan Derating Aman: Tegangan kerja <= 50-70% dari Absolute Max Rating',
  },
  {
    id: 5,
    topic: 'Proteksi Rangkaian',
    title: 'Dioda Flyback & Resistor Pull-Up / Pull-Down',
    summary: 'Dua teknik proteksi dasar agar mikrokontroler tidak reset atau rusak saat mengendalikan relay dan tombol.',
    content: 'Saat beban induktif (relay, solenoid, motor DC) dimatikan mendadak, kumparan menghasilkan lonjakan tegangan balik tinggi (back-EMF). Pasang dioda 1N4007 secara paralel terbalik (Katoda ke +VCC) untuk meredam lonjakan tersebut. Sementara pada input tombol/sensor digital, gunakan resistor Pull-Up atau Pull-Down (4.7 kOhm - 10 kOhm) agar pin tidak mengambang (floating) dan memicu pembacaan acak.',
    formula_or_rule: 'Dioda Flyback: Katoda ke +V Suplai, Anoda ke Drain/Collector',
  },
  {
    id: 6,
    topic: 'Panduan Belanja & Substitusi',
    title: 'Tips Memilih & Membeli Komponen Elektronika yang Tepat',
    summary: 'Panduan praktis sebelum membeli komponen di toko offline maupun marketplace online.',
    content: "Selalu periksa jenis kemasan (Through-Hole DIP/Axial/Radial untuk breadboard & PCB lubang, vs SMD 0805/SOIC untuk PCB cetak pabrik). Perhatikan suffix kode belakang IC (misal 'N' atau 'P' menandakan kemasan DIP lubang, sedangkan 'D' atau 'S' sering menandakan SMD). Untuk kapasitor elektrolit, boleh mengganti dengan nilai tegangan (Volt) yang lebih tinggi (misal 16V diganti 25V atau 50V) selama kapasitas (uF) sama dan muat di papan rangkaian.",
    formula_or_rule: 'Substitusi Aman Kapasitor: uF sama, Rating Volt >= Volt asli',
  },
];

function normalizedString(value, fallback, maxLength) {
  const text = typeof value === 'string' ? value.trim() : '';
  return (text || fallback).slice(0, maxLength - 1);
}

export function normalizeComponent(input) {
  if (!input || typeof input !== 'object' || Array.isArray(input)) {
    return { error: 'Data komponen tidak valid.' };
  }

  const component = {};
  for (const [field, [maxLength, fallback]] of Object.entries(COMPONENT_FIELDS)) {
    component[field] = normalizedString(input[field], fallback, maxLength);
  }

  if (!component.code || !component.name) {
    return { error: 'Kode/Part Number dan Nama Komponen wajib diisi.' };
  }

  const pinCount = Number(input.pin_count);
  component.pin_count = Number.isInteger(pinCount) && pinCount >= 0 && pinCount <= 1024
    ? pinCount
    : 0;
  return { component };
}

export function filterComponents(components, query = '', category = '') {
  const q = String(query || '').toLocaleLowerCase();
  const selectedCategory = String(category || '');
  const searchableFields = [
    'name',
    'code',
    'sub_category',
    'mpn',
    'manufacturer',
    'nominal_value',
    'package_type',
    'function_desc',
    'application',
    'where_to_buy',
  ];

  return components.filter((component) => {
    if (selectedCategory && selectedCategory.toLocaleLowerCase() !== 'semua' &&
        component.category.toLocaleLowerCase() !== selectedCategory.toLocaleLowerCase()) {
      return false;
    }
    return !q || searchableFields.some((field) =>
      component[field].toLocaleLowerCase().includes(q));
  });
}

export function computeStats(components) {
  const categoryCounts = Object.fromEntries(CATEGORIES.map((category) => [category, 0]));
  let totalDatasheets = 0;

  for (const component of components) {
    if (component.datasheet_url.length > 4) totalDatasheets += 1;
    const category = CATEGORIES.find((name) =>
      name.toLocaleLowerCase() === component.category.toLocaleLowerCase());
    categoryCounts[category || 'Sensor & Modul'] += 1;
  }

  let topCategory = '-';
  let topCategoryCount = 0;
  for (const category of CATEGORIES) {
    if (categoryCounts[category] > topCategoryCount) {
      topCategory = category;
      topCategoryCount = categoryCounts[category];
    }
  }

  return {
    total_components: components.length,
    total_datasheets: totalDatasheets,
    top_category: topCategory,
    top_category_count: topCategoryCount,
    categories: categoryCounts,
  };
}

function nearestE24Resistor(ohms) {
  if (ohms <= 0) return 0;
  const values = [
    1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0,
    3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1,
  ];
  const decade = 10 ** Math.floor(Math.log10(ohms));
  let best = values[0] * decade;
  let difference = Math.abs(ohms - best);
  for (const value of values) {
    const candidate = value * decade;
    const candidateDifference = Math.abs(ohms - candidate);
    if (candidateDifference < difference) {
      difference = candidateDifference;
      best = candidate;
    }
  }
  const nextDecade = 10 * decade;
  if (Math.abs(ohms - nextDecade) < difference) best = nextDecade;
  return best;
}

export function calculate(payload) {
  const mode = typeof payload?.mode === 'string' ? payload.mode : '';
  const number = (key) => {
    const value = Number(payload?.[key] ?? 0);
    return Number.isFinite(value) ? value : 0;
  };
  const v = number('v');
  const i = number('i');
  const r = number('r');
  const vf = number('vf');
  const ifMa = number('if_ma');
  const r1 = number('r1');
  const r2 = number('r2');
  const cUf = number('c_uf');
  const result = {
    valid: false,
    mode,
    formula: '',
    primary_label: '',
    primary_value: 0,
    primary_unit: '',
    secondary_label: '',
    secondary_value: 0,
    secondary_unit: '',
    recommendation: '',
  };
  const invalid = (error) => ({ valid: false, error });

  if (!mode) return invalid('Mode kalkulator tidak boleh kosong.');

  switch (mode) {
    case 'OHM_V':
      if (i < 0 || r < 0) return invalid('Arus (I) dan Hambatan (R) tidak boleh negatif.');
      Object.assign(result, {
        valid: true,
        formula: 'V = I * R  dan  P = V * I',
        primary_label: 'Tegangan (V)',
        primary_value: i * r,
        primary_unit: 'Volt (V)',
        secondary_label: 'Disipasi Daya (P)',
        secondary_value: i * r * i,
        secondary_unit: 'Watt (W)',
        recommendation: `Gunakan komponen dengan rating tegangan minimal ${(i * r * 1.5).toFixed(2)} V (margin aman 1.5x).`,
      });
      break;
    case 'OHM_I':
      if (r <= 0) return invalid('Hambatan (R) harus lebih besar dari 0 Ohm.');
      Object.assign(result, {
        valid: true,
        formula: 'I = V / R  dan  P = V^2 / R',
        primary_label: 'Arus Listrik (I)',
        primary_value: v / r,
        primary_unit: 'Ampere (A)',
        secondary_label: 'Disipasi Daya (P)',
        secondary_value: v * (v / r),
        secondary_unit: 'Watt (W)',
        recommendation: `Setara dengan ${((v / r) * 1000).toFixed(2)} mA. Pilih resistor dengan rating daya minimal ${((v * (v / r)) * 2).toFixed(3)} W.`,
      });
      break;
    case 'OHM_R': {
      if (i <= 0) return invalid('Arus (I) harus lebih besar dari 0 Ampere.');
      const resistance = v / i;
      Object.assign(result, {
        valid: true,
        formula: 'R = V / I  dan  P = V * I',
        primary_label: 'Hambatan (R)',
        primary_value: resistance,
        primary_unit: 'Ohm',
        secondary_label: 'Disipasi Daya (P)',
        secondary_value: v * i,
        secondary_unit: 'Watt (W)',
        recommendation: `Nilai resistor standar E24 terdekat: ${nearestE24Resistor(resistance).toFixed(1)} Ohm.`,
      });
      break;
    }
    case 'LED_RES': {
      if (v <= vf) return invalid('Tegangan sumber (Vs) harus lebih besar dari tegangan maju LED (Vf).');
      if (ifMa <= 0) return invalid('Arus target LED (If) harus lebih besar dari 0 mA (umumnya 10-20 mA).');
      const current = ifMa / 1000;
      const resistance = (v - vf) / current;
      const dissipation = (v - vf) * current;
      Object.assign(result, {
        valid: true,
        formula: 'R = (Vs - Vf) / If',
        primary_label: 'Resistor Eksak',
        primary_value: resistance,
        primary_unit: 'Ohm',
        secondary_label: 'Daya pada Resistor',
        secondary_value: dissipation,
        secondary_unit: 'Watt (W)',
        recommendation: `Gunakan resistor standar ${nearestE24Resistor(resistance).toFixed(0)} Ohm (daya min. ${dissipation * 2 <= 0.25 ? '1/4 Watt (0.25W)' : '1/2 Watt atau 1 Watt'}).`,
      });
      break;
    }
    case 'DIVIDER': {
      if (r1 < 0 || r2 < 0 || r1 + r2 <= 0) {
        return invalid('Nilai R1 dan R2 tidak valid (total hambatan harus > 0).');
      }
      const output = v * (r2 / (r1 + r2));
      Object.assign(result, {
        valid: true,
        formula: 'Vout = Vin * (R2 / (R1 + R2))',
        primary_label: 'Tegangan Keluaran (Vout)',
        primary_value: output,
        primary_unit: 'Volt (V)',
        secondary_label: 'Arus Pembagi (I_div)',
        secondary_value: (v / (r1 + r2)) * 1000,
        secondary_unit: 'mA',
        recommendation: `Rasio pembagi: ${((v > 0 ? output / v : 0) * 100).toFixed(2)}% dari Vin. Cocok untuk pembacaan ADC sensor.`,
      });
      break;
    }
    case 'SER_PAR':
    case 'CAP_SER_P': {
      if (r1 <= 0 || r2 <= 0) {
        return invalid(mode === 'SER_PAR'
          ? 'Masukkan nilai R1 dan R2 lebih besar dari 0 Ohm.'
          : 'Masukkan nilai C1 dan C2 lebih besar dari 0 uF.');
      }
      const parallel = (r1 * r2) / (r1 + r2);
      Object.assign(result, {
        valid: true,
        formula: mode === 'SER_PAR'
          ? 'R_seri = R1 + R2  |  R_paralel = (R1 * R2) / (R1 + R2)'
          : 'C_paralel = C1 + C2  |  C_seri = (C1 * C2) / (C1 + C2)',
        primary_label: mode === 'SER_PAR' ? 'Total Paralel (R_par)' : 'Total Paralel (C_par)',
        primary_value: mode === 'SER_PAR' ? parallel : r1 + r2,
        primary_unit: mode === 'SER_PAR' ? 'Ohm' : 'uF',
        secondary_label: mode === 'SER_PAR' ? 'Total Seri (R_seri)' : 'Total Seri (C_seri)',
        secondary_value: mode === 'SER_PAR' ? r1 + r2 : parallel,
        secondary_unit: mode === 'SER_PAR' ? 'Ohm' : 'uF',
        recommendation: mode === 'SER_PAR'
          ? 'Rangkaian paralel selalu menghasilkan hambatan lebih kecil dari resistor terkecilnya.'
          : 'Kapasitor paralel menjumlahkan kapasitas; kapasitor seri membagi kapasitas namun menaikkan batas tegangan.',
      });
      break;
    }
    case 'NE555': {
      if (r1 <= 0 || r2 <= 0 || cUf <= 0) {
        return invalid('Masukkan R1 (Ohm), R2 (Ohm), dan C (uF) bernilai positif.');
      }
      const capacitance = cUf * 1e-6;
      const frequency = 1.44 / ((r1 + 2 * r2) * capacitance);
      const duty = ((r1 + r2) / (r1 + 2 * r2)) * 100;
      Object.assign(result, {
        valid: true,
        formula: 'f = 1.44 / ((R1 + 2*R2) * C)',
        primary_label: 'Frekuensi Osilasi (f)',
        primary_value: frequency,
        primary_unit: 'Hz',
        secondary_label: 'Duty Cycle High',
        secondary_value: duty,
        secondary_unit: '%',
        recommendation: `Periode pulsa T = ${(1 / frequency).toFixed(4)} detik (High = ${(0.693 * (r1 + r2) * capacitance).toFixed(4)} s, Low = ${(0.693 * r2 * capacitance).toFixed(4)} s).`,
      });
      break;
    }
    default:
      return invalid('Mode kalkulator tidak dikenali.');
  }

  if (!Number.isFinite(result.primary_value) || !Number.isFinite(result.secondary_value)) {
    return invalid('Hasil perhitungan berada di luar rentang yang didukung.');
  }
  return result;
}

export function exportCsv(components) {
  const headers = [
    'ID', 'Kode', 'Nama', 'Kategori', 'Sub_Kategori', 'MPN', 'Produsen',
    'Nilai_Nominal', 'Toleransi', 'Tegangan_Maks', 'Arus_Maks', 'Daya_Maks',
    'Suhu_Operasi', 'Kemasan', 'Dimensi', 'Jumlah_Pin', 'Pinout', 'Fungsi',
    'Aplikasi', 'Catatan', 'Estimasi_Harga', 'Tempat_Beli', 'Datasheet',
  ];
  const fields = [
    'id', 'code', 'name', 'category', 'sub_category', 'mpn', 'manufacturer',
    'nominal_value', 'tolerance', 'voltage_rating', 'current_rating', 'power_rating',
    'temp_range', 'package_type', 'dimensions', 'pin_count', 'pinout', 'function_desc',
    'application', 'usage_notes', 'price_estimate', 'where_to_buy', 'datasheet_url',
  ];
  const escape = (value) => `"${String(value ?? '').replaceAll('"', '""')}"`;
  return [
    headers.join(','),
    ...components.map((component) => fields.map((field) => escape(component[field])).join(',')),
  ].join('\r\n');
}
