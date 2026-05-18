#ifndef RASYID_h
#define RASYID_h

#include "CONFIG.h"

// OTHERS
int isCtrlPressed();
int isAltPressed();

// SET
void setStructEditor(Editor *E);

// RESET
void resetTab(TabNode *TT);
void resetLine(LineNode *lineSekarang);
void resetChar(CharNode *CC);

// CURSOR
void hideCursor();
void showCursor();
void moveCursor(int row, int col);

// CLEAR
void clearScreen();
void clearRows(int start, int end, int width);

// RENDER 
void renderHeader(Editor *E);
void renderScroll(TabNode *TT);

// REDRAW 
void redrawText(TabNode *TT);



// ADD
void addTab(Editor *E);
TabNode *AlokasiTab();

// TAB
void addTab(Editor *E);

// INPUT HANDLER
void inputCharHandler(TabNode **TT, int c);
void arrowKeyHandler(TabNode **TT,int c);






#endif
