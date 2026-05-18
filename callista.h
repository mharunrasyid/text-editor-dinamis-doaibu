#include "callista.c"

void setStructEditor();

int isCtrlPressed();
int isAltPressed();

LineNode* createEmptyLine();
void createFirstTab();

void clearScreen();
void clearRows(int start, int end, int width);

void redrawText(TabNode *TT);
void resetTab(TabNode *TT);
void resetLine(LineNode *LL);
void resetChar(CharNode *CC);

void hideCursor();
void showCursor();
void moveCursor(int row, int col);

void renderHeader();
void renderScroll(TabNode *TT);

void arrowKeyHandler(TabNode **TT,int c);