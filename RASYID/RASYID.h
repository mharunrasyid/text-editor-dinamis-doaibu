#ifndef RASYID_h
#define RASYID_h

#include "../CONFIG.h"

// DEALOKASI
void DelokasiTab(TabNode *TT);
void DelokasiLine(LineNode *LL);
void DelokasiChar(CharNode *CC);

// DELETE
void delete(TabNode *TT);

// MERGE
void merge(LineNode *LL, int gap);

#endif
