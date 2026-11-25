#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

typedef struct {
    int id;
    char topic[48];
    char title[120];
    char summary[220];
    char content[900];
    char formula_or_rule[240];
} KnowledgeArticle;

int knowledge_get_all(const KnowledgeArticle **out_articles);

#endif /* KNOWLEDGE_H */
