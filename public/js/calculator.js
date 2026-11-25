/**
 * Modul UI Kalkulator Rangkaian Elektronika
 */
const CALC_SCHEMAS = {
  LED_RES: [
    { id: 'v', label: 'Tegangan Sumber Vs (Volt)', placeholder: 'Contoh: 5 atau 12', val: '5' },
    { id: 'vf', label: 'Tegangan Maju LED Vf (Volt) [Merah ~2.0V, Putih/Biru ~3.2V]', placeholder: '2.0', val: '2.0' },
    { id: 'if_ma', label: 'Arus Target LED If (mA)', placeholder: '15', val: '15' },
  ],
  DIVIDER: [
    { id: 'v', label: 'Tegangan Input Vin (Volt)', placeholder: '12', val: '12' },
    { id: 'r1', label: 'Resistor Atas R1 (Ohm)', placeholder: '10000', val: '10000' },
    { id: 'r2', label: 'Resistor Bawah R2 (Ohm)', placeholder: '4700', val: '4700' },
  ],
  OHM_I: [
    { id: 'v', label: 'Tegangan V (Volt)', placeholder: '12', val: '12' },
    { id: 'r', label: 'Hambatan R (Ohm)', placeholder: '470', val: '470' },
  ],
  OHM_V: [
    { id: 'i', label: 'Arus I (Ampere)', placeholder: '0.025', val: '0.025' },
    { id: 'r', label: 'Hambatan R (Ohm)', placeholder: '330', val: '330' },
  ],
  OHM_R: [
    { id: 'v', label: 'Tegangan V (Volt)', placeholder: '9', val: '9' },
    { id: 'i', label: 'Arus I (Ampere)', placeholder: '0.02', val: '0.02' },
  ],
  SER_PAR: [
    { id: 'r1', label: 'Resistor R1 (Ohm)', placeholder: '1000', val: '1000' },
    { id: 'r2', label: 'Resistor R2 (Ohm)', placeholder: '1000', val: '1000' },
  ],
  CAP_SER_P: [
    { id: 'r1', label: 'Kapasitor C1 (uF)', placeholder: '100', val: '100' },
    { id: 'r2', label: 'Kapasitor C2 (uF)', placeholder: '100', val: '100' },
  ],
  NE555: [
    { id: 'r1', label: 'Resistor R1 (Ohm)', placeholder: '1000', val: '1000' },
    { id: 'r2', label: 'Resistor R2 (Ohm)', placeholder: '10000', val: '10000' },
    { id: 'c_uf', label: 'Kapasitor C (uF)', placeholder: '10', val: '10' },
  ],
};

function renderCalcInputs(mode) {
  const container = document.getElementById('dynamicCalcInputs');
  const fields = CALC_SCHEMAS[mode] || [];
  container.innerHTML = fields.map((f) => `
    <div class="form-group">
      <label for="field_${f.id}">${f.label}</label>
      <input type="number" step="any" id="field_${f.id}" data-key="${f.id}" class="input" placeholder="${f.placeholder}" value="${f.val}" required />
    </div>
  `).join('');
}

document.addEventListener('DOMContentLoaded', () => {
  const modeSelect = document.getElementById('calcMode');
  const form = document.getElementById('calcForm');
  const output = document.getElementById('calcOutput');

  renderCalcInputs(modeSelect.value);
  modeSelect.addEventListener('change', () => renderCalcInputs(modeSelect.value));

  form.addEventListener('submit', async (e) => {
    e.preventDefault();
    const payload = { mode: modeSelect.value };
    document.querySelectorAll('#dynamicCalcInputs input').forEach((inp) => {
      payload[inp.dataset.key] = parseFloat(inp.value) || 0;
    });

    try {
      const res = await ElectroAPI.calculate(payload);
      output.innerHTML = `
        <div style="color: #38bdf8; font-size: 0.82rem;">Rumus: ${escapeHtml(res.formula)}</div>
        <div style="padding: 0.65rem; background: rgba(16, 185, 129, 0.12); border-radius: 6px;">
          <div style="font-size: 0.78rem; color: var(--text-secondary);">${escapeHtml(res.primary_label)}</div>
          <div style="font-size: 1.35rem; font-weight: 700; color: #34d399;">
            ${Number(res.primary_value).toFixed(4)} ${escapeHtml(res.primary_unit)}
          </div>
        </div>
        <div>
          <span style="color: var(--text-secondary);">${escapeHtml(res.secondary_label)}: </span>
          <strong>${Number(res.secondary_value).toFixed(4)} ${escapeHtml(res.secondary_unit)}</strong>
        </div>
        <div style="font-family: var(--font-sans); font-size: 0.85rem; color: #fbbf24; margin-top: 0.25rem;">
          <strong>Rekomendasi Praktis:</strong> ${escapeHtml(res.recommendation)}
        </div>
      `;
    } catch (err) {
      output.innerHTML = `<span style="color: var(--accent-rose);">${escapeHtml(err.message)}</span>`;
    }
  });
});
