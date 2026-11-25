/**
 * Controller Utama & Navigasi Tab ElectroDB
 */
function showToast(message) {
  const toast = document.getElementById('toast');
  toast.textContent = message;
  toast.classList.add('show');
  setTimeout(() => toast.classList.remove('show'), 2800);
}

async function refreshDashboard() {
  const query = document.getElementById('searchInput').value.trim();
  const category = document.getElementById('categoryFilter').value;

  try {
    const [listData, stats] = await Promise.all([
      ElectroAPI.fetchComponents(query, category),
      ElectroAPI.fetchStats(),
    ]);
    renderCatalogCards(listData.items || []);
    renderStatsAndChips(stats, category);
  } catch (err) {
    showToast(err.message);
  }
}

function renderStatsAndChips(stats, activeCategory) {
  document.getElementById('kpiTotalComp').textContent = stats.total_components;
  document.getElementById('kpiDatasheets').textContent = stats.total_datasheets;
  document.getElementById('kpiTopCat').textContent = stats.top_category;
  document.getElementById('kpiTopCatSub').textContent =
    stats.top_category_count > 0 ? `${stats.top_category_count} referensi komponen` : 'Belum ada kategori';

  const chipsContainer = document.getElementById('categoryChips');
  const cats = Object.entries(stats.categories || {});
  const allChip = `<button type="button" class="chip ${activeCategory === 'Semua' ? 'active' : ''}" onclick="selectCategoryChip('Semua')">Semua (${stats.total_components})</button>`;
  const catChips = cats.map(([name, cnt]) => `
    <button type="button" class="chip ${activeCategory === name ? 'active' : ''}" onclick="selectCategoryChip('${name}')">
      ${name} (${cnt})
    </button>
  `).join('');
  chipsContainer.innerHTML = allChip + catChips;
}

function selectCategoryChip(cat) {
  document.getElementById('categoryFilter').value = cat;
  refreshDashboard();
}

function openAddModal() {
  document.getElementById('formModalTitle').textContent = 'Kontribusi Referensi Komponen Baru';
  document.getElementById('compId').value = '';
  document.getElementById('componentForm').reset();
  document.getElementById('compPinCount').value = 2;
  document.getElementById('formModal').classList.add('open');
}

function openEditModal(id) {
  const c = catalogItems.find((item) => item.id === id);
  if (!c) return;

  document.getElementById('formModalTitle').textContent = `Edit Referensi #${c.id} (${c.code})`;
  document.getElementById('compId').value = c.id;
  document.getElementById('compCode').value = c.code;
  document.getElementById('compCategory').value = c.category;
  document.getElementById('compName').value = c.name;
  document.getElementById('compNominal').value = c.nominal_value;
  document.getElementById('compPackage').value = c.package_type;
  document.getElementById('compFunction').value = c.function_desc;

  document.getElementById('compSubCategory').value = c.sub_category;
  document.getElementById('compMpn').value = c.mpn;
  document.getElementById('compManufacturer').value = c.manufacturer;
  document.getElementById('compTolerance').value = c.tolerance;
  document.getElementById('compVoltage').value = c.voltage_rating;
  document.getElementById('compCurrent').value = c.current_rating;
  document.getElementById('compPower').value = c.power_rating;
  document.getElementById('compTemp').value = c.temp_range;
  document.getElementById('compDimensions').value = c.dimensions;
  document.getElementById('compPinCount').value = c.pin_count;
  document.getElementById('compPinout').value = c.pinout;
  document.getElementById('compApplication').value = c.application;
  document.getElementById('compNotes').value = c.usage_notes;
  document.getElementById('compPrice').value = c.price_estimate;
  document.getElementById('compDatasheet').value = c.datasheet_url;
  document.getElementById('compWhereToBuy').value = c.where_to_buy;

  document.getElementById('formModal').classList.add('open');
}

function closeFormModal() {
  document.getElementById('formModal').classList.remove('open');
}

async function handleDeleteComponent(id, name) {
  if (!confirm(`Hapus referensi "${name}" dari katalog?`)) return;
  try {
    await ElectroAPI.deleteComponent(id);
    showToast(`Referensi "${name}" berhasil dihapus`);
    refreshDashboard();
  } catch (err) {
    showToast(err.message);
  }
}

document.addEventListener('DOMContentLoaded', () => {
  refreshDashboard();
  loadKnowledgeArticles();

  document.querySelectorAll('.nav-tab').forEach((btn) => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.nav-tab').forEach((b) => b.classList.remove('active'));
      document.querySelectorAll('.tab-panel').forEach((p) => p.classList.remove('active'));
      btn.classList.add('active');
      document.getElementById(btn.dataset.tab).classList.add('active');
    });
  });

  document.getElementById('searchInput').addEventListener('input', refreshDashboard);
  document.getElementById('categoryFilter').addEventListener('change', refreshDashboard);

  document.getElementById('btnOpenAdd').addEventListener('click', openAddModal);
  document.getElementById('btnCancelForm').addEventListener('click', closeFormModal);
  document.getElementById('btnCloseDetail').addEventListener('click', closeDetailModal);

  document.getElementById('btnEditFromDetail').addEventListener('click', () => {
    closeDetailModal();
    if (activeDetailId) openEditModal(activeDetailId);
  });

  document.getElementById('btnResetDb').addEventListener('click', async () => {
    if (!confirm('Reset katalog referensi ke daftar komponen bawaan?')) return;
    try {
      await ElectroAPI.resetDatabase();
      showToast('Database referensi dikembalikan ke data awal');
      refreshDashboard();
    } catch (err) {
      showToast(err.message);
    }
  });

  document.getElementById('componentForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    const id = document.getElementById('compId').value;
    const payload = {
      code: document.getElementById('compCode').value.trim(),
      category: document.getElementById('compCategory').value,
      name: document.getElementById('compName').value.trim(),
      nominal_value: document.getElementById('compNominal').value.trim(),
      package_type: document.getElementById('compPackage').value.trim(),
      function_desc: document.getElementById('compFunction').value.trim(),
      sub_category: document.getElementById('compSubCategory').value.trim(),
      mpn: document.getElementById('compMpn').value.trim(),
      manufacturer: document.getElementById('compManufacturer').value.trim(),
      tolerance: document.getElementById('compTolerance').value.trim(),
      voltage_rating: document.getElementById('compVoltage').value.trim(),
      current_rating: document.getElementById('compCurrent').value.trim(),
      power_rating: document.getElementById('compPower').value.trim(),
      temp_range: document.getElementById('compTemp').value.trim(),
      dimensions: document.getElementById('compDimensions').value.trim(),
      pin_count: Number(document.getElementById('compPinCount').value || 0),
      pinout: document.getElementById('compPinout').value.trim(),
      application: document.getElementById('compApplication').value.trim(),
      usage_notes: document.getElementById('compNotes').value.trim(),
      price_estimate: document.getElementById('compPrice').value.trim(),
      datasheet_url: document.getElementById('compDatasheet').value.trim(),
      where_to_buy: document.getElementById('compWhereToBuy').value.trim(),
    };

    try {
      if (id) {
        await ElectroAPI.updateComponent(id, payload);
        showToast('Perubahan referensi berhasil disimpan');
      } else {
        await ElectroAPI.createComponent(payload);
        showToast('Referensi baru berhasil ditambahkan');
      }
      closeFormModal();
      refreshDashboard();
    } catch (err) {
      showToast(err.message);
    }
  });
});
