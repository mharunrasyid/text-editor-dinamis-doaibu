#ifndef callista_h
#define callista_h

#include "../CONFIG.h"

int isCtrlPressed();
int isAltPressed();

void clearScreen();
void clearRows(int start, int end, int width);

void redrawText(TabNode *TT);

void hideCursor();
void showCursor();
void moveCursor(int row, int col);

void renderHeader();
void renderScroll(TabNode *TT);

void inputCharHandler(Editor *E, TabNode **TT, int c);
void arrowKeyHandler(TabNode **TT,int c);

#endif