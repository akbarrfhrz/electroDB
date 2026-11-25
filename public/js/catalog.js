/**
 * Modul Rendering Katalog Referensi Komponen & Inspektor Spesifikasi Terkecil
 */
let catalogItems = [];
let activeDetailId = null;

function escapeHtml(str) {
  return String(str ?? '-')
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

function renderCatalogCards(items) {
  catalogItems = items;
  const grid = document.getElementById('catalogGrid');

  if (items.length === 0) {
    grid.innerHTML = `
      <div class="card" style="grid-column: 1 / -1; text-align: center; color: var(--text-muted); padding: 2rem;">
        Tidak ada referensi komponen yang sesuai dengan kata kunci pencarian.
      </div>
    `;
    return;
  }

  grid.innerHTML = items.map((c) => `
    <article class="comp-card">
      <div>
        <div class="comp-card-header">
          <div>
            <div class="comp-code">${escapeHtml(c.code)}</div>
            <h3 class="comp-title">${escapeHtml(c.name)}</h3>
            <div class="comp-sub">${escapeHtml(c.sub_category)}</div>
          </div>
          <span class="badge badge-${escapeHtml(c.category)}">${escapeHtml(c.category)}</span>
        </div>

        <div class="pill-row">
          <span class="spec-pill">${escapeHtml(c.nominal_value)}</span>
          <span class="pkg-pill">${escapeHtml(c.package_type)}</span>
        </div>

        <p class="comp-desc" style="margin-top: 0.65rem;">${escapeHtml(c.function_desc)}</p>

        <div class="comp-meta-box" style="margin-top: 0.75rem;">
          <div class="meta-row">
            <strong>Estimasi Harga: </strong>
            <span class="price-tag">${escapeHtml(c.price_estimate)}</span>
          </div>
          <div class="meta-row">
            <strong>Tempat Beli: </strong>
            <span>${escapeHtml(c.where_to_buy)}</span>
          </div>
        </div>
      </div>

      <div class="comp-footer">
        <div style="display: flex; gap: 0.45rem; flex-wrap: wrap;">
          <button class="btn btn-primary btn-sm" onclick="openDetailModal(${c.id})">Lihat Detail Terkecil</button>
          ${c.datasheet_url && c.datasheet_url.startsWith('http')
            ? `<a href="${escapeHtml(c.datasheet_url)}" target="_blank" rel="noopener noreferrer" class="btn btn-emerald btn-sm">Datasheet</a>`
            : ''}
        </div>
        <div style="display: flex; gap: 0.4rem;">
          <button class="btn btn-secondary btn-sm" onclick="openEditModal(${c.id})">Edit</button>
          <button class="btn btn-danger btn-sm" onclick="handleDeleteComponent(${c.id}, '${escapeHtml(c.name)}')">Hapus</button>
        </div>
      </div>
    </article>
  `).join('');
}

function openDetailModal(id) {
  const c = catalogItems.find((item) => item.id === id);
  if (!c) return;
  activeDetailId = id;

  document.getElementById('detCode').textContent = `${c.code} • ID #${c.id}`;
  document.getElementById('detName').textContent = c.name;
  document.getElementById('detSubCat').textContent = `${c.sub_category} • ${c.manufacturer}`;
  document.getElementById('detCategory').textContent = c.category;

  document.getElementById('detGranularBody').innerHTML = `
    <div class="spec-section">
      <div class="spec-section-title">1. Identitas & Klasifikasi Part</div>
      <div class="spec-kv-grid">
        <span class="spec-k">Kode Internal</span>
        <span class="spec-v mono">${escapeHtml(c.code)}</span>

        <span class="spec-k">Kategori / Sub-Kategori</span>
        <span class="spec-v">${escapeHtml(c.category)} — ${escapeHtml(c.sub_category)}</span>

        <span class="spec-k">Part Number (MPN)</span>
        <span class="spec-v mono">${escapeHtml(c.mpn)}</span>

        <span class="spec-k">Produsen / Pabrikan</span>
        <span class="spec-v">${escapeHtml(c.manufacturer)}</span>
      </div>
    </div>

    <div class="spec-section">
      <div class="spec-section-title">2. Spesifikasi Elektrikal & Fisik Terkecil</div>
      <div class="spec-kv-grid">
        <span class="spec-k">Nilai Nominal Utama</span>
        <span class="spec-v mono" style="color: #34d399; font-weight: 700;">${escapeHtml(c.nominal_value)}</span>

        <span class="spec-k">Toleransi / Akurasi</span>
        <span class="spec-v mono">${escapeHtml(c.tolerance)}</span>

        <span class="spec-k">Rating Tegangan Maks</span>
        <span class="spec-v mono">${escapeHtml(c.voltage_rating)}</span>

        <span class="spec-k">Rating Arus Maks</span>
        <span class="spec-v mono">${escapeHtml(c.current_rating)}</span>

        <span class="spec-k">Disipasi Daya / ESR</span>
        <span class="spec-v mono">${escapeHtml(c.power_rating)}</span>

        <span class="spec-k">Rentang Suhu Operasi</span>
        <span class="spec-v mono">${escapeHtml(c.temp_range)}</span>

        <span class="spec-k">Kemasan / Footprint</span>
        <span class="spec-v mono">${escapeHtml(c.package_type)}</span>

        <span class="spec-k">Dimensi Fisik</span>
        <span class="spec-v mono">${escapeHtml(c.dimensions)}</span>

        <span class="spec-k">Jumlah Pin / Kaki</span>
        <span class="spec-v mono">${escapeHtml(c.pin_count)} pin</span>

        <span class="spec-k">Konfigurasi Pinout</span>
        <span class="spec-v mono">${escapeHtml(c.pinout)}</span>
      </div>
    </div>

    <div class="spec-section">
      <div class="spec-section-title">3. Fungsi Kerja, Aplikasi & Catatan Pemasangan</div>
      <div class="spec-kv-grid">
        <span class="spec-k">Prinsip & Fungsi Utama</span>
        <span class="spec-v">${escapeHtml(c.function_desc)}</span>

        <span class="spec-k">Contoh Aplikasi Rangkaian</span>
        <span class="spec-v">${escapeHtml(c.application)}</span>
      </div>
      <div class="note-callout">
        <strong>Catatan Teknis / Polaritas:</strong> ${escapeHtml(c.usage_notes)}
      </div>
    </div>

    <div class="spec-section">
      <div class="spec-section-title">4. Referensi Harga Pasar & Tempat Membeli</div>
      <div class="spec-kv-grid">
        <span class="spec-k">Estimasi Harga Pasar</span>
        <span class="spec-v price-tag">${escapeHtml(c.price_estimate)}</span>

        <span class="spec-k">Rekomendasi Tempat Beli</span>
        <span class="spec-v">${escapeHtml(c.where_to_buy)}</span>
      </div>
    </div>
  `;

  const dsLink = document.getElementById('detDatasheetLink');
  if (c.datasheet_url && c.datasheet_url.startsWith('http')) {
    dsLink.href = c.datasheet_url;
    dsLink.style.display = 'inline-flex';
  } else {
    dsLink.style.display = 'none';
  }

  document.getElementById('detailModal').classList.add('open');
}

function closeDetailModal() {
  document.getElementById('detailModal').classList.remove('open');
}
