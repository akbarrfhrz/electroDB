/**
 * Modul UI Ensiklopedia & Pengetahuan Elektronika (dari src/knowledge.c)
 */
async function loadKnowledgeArticles() {
  const grid = document.getElementById('knowledgeGrid');
  try {
    const data = await ElectroAPI.fetchKnowledge();
    const articles = data.articles || [];
    grid.innerHTML = articles.map((art) => `
      <article class="knowledge-card">
        <div>
          <span class="badge">${escapeHtml(art.topic)}</span>
          <h3 style="font-size: 1.05rem; margin-top: 0.45rem;">${escapeHtml(art.title)}</h3>
          <p style="font-size: 0.82rem; color: var(--text-muted); margin-top: 0.2rem;">${escapeHtml(art.summary)}</p>
        </div>
        <p style="font-size: 0.87rem; color: var(--text-secondary); line-height: 1.6;">
          ${escapeHtml(art.content)}
        </p>
        <div class="rule-box">
          ${escapeHtml(art.formula_or_rule)}
        </div>
      </article>
    `).join('');
  } catch (err) {
    grid.innerHTML = `<div class="card" style="color: var(--accent-rose);">${escapeHtml(err.message)}</div>`;
  }
}
