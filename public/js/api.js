/**
 * Modul REST API Client menuju Backend Bahasa C (src/server.c)
 */
const ElectroAPI = {
  async fetchComponents(query = '', category = 'Semua') {
    const params = new URLSearchParams();
    if (query) params.set('q', query);
    if (category && category !== 'Semua') params.set('category', category);
    const res = await fetch(`/api/components?${params.toString()}`);
    if (!res.ok) throw new Error('Gagal memuat katalog referensi dari server C');
    return res.json();
  },

  async createComponent(payload) {
    const res = await fetch('/api/components', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload),
    });
    const data = await res.json();
    if (!res.ok) throw new Error(data.error || 'Gagal menyimpan referensi komponen');
    return data;
  },

  async updateComponent(id, payload) {
    const res = await fetch(`/api/components/${id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload),
    });
    const data = await res.json();
    if (!res.ok) throw new Error(data.error || 'Gagal memperbarui referensi komponen');
    return data;
  },

  async deleteComponent(id) {
    const res = await fetch(`/api/components/${id}`, {
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
    const res = await fetch('/api/reset', { method: 'POST' });
    if (!res.ok) throw new Error('Gagal mereset database');
    return res.json();
  },
};
