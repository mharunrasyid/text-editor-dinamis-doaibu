#ifndef RASYID_h
#define RASYID_h

#include "../CONFIG.h"

// DEALOKASI
void DelokasiTab(TabNode *TT);
void DelokasiLine(LineNode *LL);
void DelokasiChar(CharNode *CC);

// DELETE
void delete(TabNode *TT);
void delLine(TabNode *TT);
void deleteTab(Editor *E);

// MERGE
void merge(LineNode *LL, int gap);

// REPLACE
void replaceV(Editor *E);
void replaceAll(TabNode *TT, char* findW, char* replaceW);
void replace(TabNode *TT, CharNode *LastCharReplace, int LfindW, char* replaceW);

#endif
