#ifndef rama_h
#define rama_h

#include "CONFIG.h"
#include "rasyid.h"

LineNode *AlokasiLine();
CharNode *AlokasiChar();
void addLine(TabNode *TT, LineNode *diKursor);
void insert(TabNode *TT, char c);
void handleOverflow(TabNode *TT, LineNode *line);
void newline(TabNode *TT);
void saveFile(TabNode *TT);
void saveAs(TabNode *TT);

#endif