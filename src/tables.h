#ifndef _TABLES_H_
#define _TABLES_H_

#include "types.h"
#include "containers.h"
#include "verb.h"

extern Verb *G_VERB_LIST;
extern Hashtable *G_WORD_TABLE_VERBS;

#define ADD_VERB(text, verb)hashtable_insert(G_WORD_TABLE_VERBS, \
    text, \
    (void *)&G_VERB_LIST[verb])

bool init_tables();
bool init_verb_table();

#endif