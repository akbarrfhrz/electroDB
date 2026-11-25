let adminToken = '';

async function adminFetch(url, options = {}) {
  const headers = new Headers(options.headers || {});
  if (adminToken) headers.set('Authorization', `Bearer ${adminToken}`);

  let response = await fetch(url, { ...options, headers });
  if (response.status !== 401) return response;

  adminToken = '';
  const token = window.prompt('Masukkan kunci admin untuk mengubah katalog:');
  if (!token) throw new Error('Aksi dibatalkan karena kunci admin tidak diberikan.');

  adminToken = token;
  headers.set('Authorization', `Bearer ${adminToken}`);
  response = await fetch(url, { ...options, headers });
  if (response.status === 401) adminToken = '';
  return response;
}

const ElectroAPI = {
  async fetchComponents(query = '', category = 'Semua') {
    const params = new URLSearchParams();
    if (query) params.set('q', query);
    if (category && category !== 'Semua') params.set('category', category);
    const res = await fetch(`/api/components?${params.toString()}`);
    if (!res.ok) throw new Error('Gagal memuat katalog referensi');
    return res.json();
  },

  async createComponent(payload) {
    const res = await adminFetch('/api/components', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload),
    });
    const data = await res.json();
    if (!res.ok) throw new Error(data.error || 'Gagal menyimpan referensi komponen');
    return data;
  },

  async updateComponent(id, payload) {
    const res = await adminFetch(`/api/components/${id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload),
    });
    const data = await res.json();
    if (!res.ok) throw new Error(data.error || 'Gagal memperbarui referensi komponen');
    return data;
  },

  async deleteComponent(id) {
    const res = await adminFetch(`/api/components/${id}`, {
      method: 'DELETE',
    });
    const data = await res.json();
    if (!res.ok) throw new Error(data.error || 'Gagal menghapus referensi');
    return data;
  },

  async fetchStats() {
    const res = await fetch('/api/stats');
    if (!res.ok) throw new Error('Gagal memuat statistik');
    return res.json();
  },

  async calculate(payload) {
    const res = await fetch('/api/calculator', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload),
    });
    const data = await res.json();
    if (!res.ok || !data.valid) {
      throw new Error(data.error || 'Perhitungan tidak valid');
    }
    return data;
  },

  async fetchKnowledge() {
    const res = await fetch('/api/knowledge');
    if (!res.ok) throw new Error('Gagal memuat ensiklopedia elektronika');
    return res.json();
  },

  async resetDatabase() {
    const res = await adminFetch('/api/reset', { method: 'POST' });
    const data = await res.json();
    if (!res.ok) throw new Error(data.error || 'Gagal mereset database');
    return data;
  },
};
