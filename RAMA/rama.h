#ifndef rama_h
#define rama_h

#include "../CONFIG.h"

void setStructEditor();

//alokasi
TabNode *AlokasiTab();
LineNode *AlokasiLine();
CharNode *AlokasiChar();

// Reset
void resetTab(TabNode *TT);
void resetLine(LineNode *LL);
void resetChar(CharNode *CC);

//add tab
void addTab(Editor *E);

//insert 
void insert(TabNode *TT, char c);
void handleOverflow(TabNode *TT, LineNode *line);

//newline
void newline(TabNode *TT);
void addLine(TabNode *TT, LineNode *diKursor);

//savefile
int namaFileIlegal(char *nama);
void tulisIsiFile(FILE *file, TabNode *TT);
void saveFile(TabNode *TT);
void saveAs(TabNode *TT);

#endif