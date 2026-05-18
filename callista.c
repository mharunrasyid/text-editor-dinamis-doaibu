#include <stdio.h>
#include <stdlib.h>
#include "CONFIG.h"

void setStructEditor() {
    E.activeTab = NULL; 
}

// cek apakah CTRL ditekan
int isCtrlPressed() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000);
}

// cek apakah ALT ditekan
int isAltPressed() {
    return (GetAsyncKeyState(VK_MENU) & 0x8000);
}

LineNode* createEmptyLine(){
    LineNode *newLine = (LineNode*)malloc(sizeof(LineNode));
    if (newLine != NULL){
        newLine->firstChar = NULL;
        newLine->lastChar = NULL;

        newLine->up = NULL;        
        newLine->down = NULL;

        newLine->length = 0;      
        newLine->isNewLine = true;
    }
    return newLine;
}

void createFirstTab() {
    TabNode *newTab = (TabNode*)malloc(sizeof(TabNode));
    if (newTab != NULL) {
        strcpy(newTab->fileName, "Untitled");
        newTab->firstLine = createEmptyLine();
        newTab->currLine = newTab->firstLine;
        newTab->topLine = newTab->firstLine;
        newTab->currChar = NULL;
        newTab->next = newTab->prev = NULL;
        
        E.activeTab = newTab;
    }
}

// CLEAR

void clearScreen() {
   system("cls");
}

void clearRows(int start, int end, int width) {
    for(int r = start; r <= end; r++) {
        printf("\033[%d;1H", r);
        printf("%*s", width, "");
    }
}

// RENDER
void renderHeader() {
    printf("\033[1;1H\033[K"); 
    printf(" [DOA IBU'S EDITOR]  |  ");

    TabNode *tempTab = E.activeTab; 
    if (tempTab == NULL) {
        printf("No Tabs Open");
        return;
    }

    while (tempTab->prev != NULL) {
        tempTab = tempTab->prev; 
    }

    while (tempTab != NULL) {
        if (tempTab == E.activeTab) {
            printf("\033[1;32m[%s *]\033[0m ", tempTab->fileName); 
        } else {
            printf("[%s] ", tempTab->fileName); 
        }
        tempTab = tempTab->next; 
    }
    printf("\n\033[K----------------------------------------------------------------------\n");
    fflush(stdout);
}

void redrawText(TabNode *TT) {
    if (TT == NULL) return;

    if (TT->cursorY > SCREEN_HEIGHT) {
        if (TT->topLine != NULL && TT->topLine->down != NULL) {
            TT->topLine = TT->topLine->down; 
            TT->topIndex++; 
            TT->cursorY = SCREEN_HEIGHT; 
        }
    } 
    
    else if (TT->cursorY < 1) {
        if (TT->topLine != NULL && TT->topLine->up != NULL) {
            TT->topLine = TT->topLine->up; 
            TT->topIndex--; 
            TT->cursorY = 1; 
        }
    }

    hideCursor();
    renderScroll(TT); 
    moveCursor(TT->cursorY, TT->cursorX); 
    
    showCursor();
}

// RESET

void resetTab(TabNode *TT) {
    TT->fileName[0] = '\0';

    TT->currLine = NULL;
    TT->currChar = NULL;
    TT->topLine = NULL;

    TT->targetX = 1;
    TT->cursorX = 1;
    TT->cursorY = 1;
    TT->topIndex = 1;

    TT->next = NULL;
    TT->prev = NULL;
}

void resetLine(LineNode *LL) {
    LL->firstChar = NULL;
    LL->lastChar = NULL;
    LL->up = NULL;
    LL->down = NULL;

    LL->length = 0;
    LL->isNewLine = false;
}

void resetChar(CharNode *CC) {
    CC->data = '\0';
    CC->next = NULL;
    CC->prev = NULL;
}

// CURSOR

void hideCursor() {
    printf("\033[?25l");
}

void showCursor() {
    printf("\033[?25h");
}

void moveCursor(int row, int col) {
    printf("\033[%d;%dH", row + 2, col);
}

// RENDER

void renderScroll(TabNode *TT) {
    if (TT == NULL) return;

    LineNode *currentLine = TT->topLine;
    int screenRow = 1;

    while (currentLine != NULL && screenRow <= SCREEN_HEIGHT) {
        moveCursor(screenRow, 1); 
        
        printf("\033[K"); 

        CharNode *currentChar = currentLine->firstChar;
        while (currentChar != NULL) {
            printf("%c", currentChar->data);
            currentChar = currentChar->next;
        }

        currentLine = currentLine->down;
        screenRow++;
    }


    while (screenRow <= SCREEN_HEIGHT) {
        moveCursor(screenRow, 1);
        printf("\033[K");
        screenRow++;
    }
}

void arrowKeyHandler(TabNode **TT,int c) {
    switch (c) {  
        // arrow up
        // case 72 : {
        //     // ini mmasih perlu dikembangin lagi
        //     if(TT->cursor_y> 0) {
        //         if (TT->text[TT->cursor_y-1][TT->cursor_x] != '\0') TT->cursor_y--;
        //         else {
        //             setPosToNTRow(TT, TT->cursor_y-1, 'd');
        //         }
        //     }

        //     break;
        // }

        // arrow down
        // case 80 : {
        //     if(TT->cursor_y < MAX_ROWS - 1) {
        //         if (TT->text[TT->cursor_y+1][TT->cursor_x] != '\0') TT->cursor_y++;
        //         else {
        //             setPosToNTRow(TT, TT->cursor_y+1, 'u');
        //         }
        //     }
            
        //     break;
        // }

        // arrow left 
        case 75 : {
        LineNode *LL = (*TT)->currLine;

        if((*TT)->cursorX > 1) { 
            if ((*TT)->currChar != NULL) {
                (*TT)->currChar = (*TT)->currChar->prev;
            } 

            (*TT)->cursorX--;
            (*TT)->targetX = (*TT)->cursorX;
        } else if(LL != NULL && LL->up != NULL) {
            (*TT)->currLine = LL->up;
            (*TT)->currChar = LL->up->lastChar; 
            (*TT)->cursorY--;
            (*TT)->cursorX = LL->up->length + 1;
            (*TT)->targetX = (*TT)->cursorX;
        }
        
        break;
    }

        // arrow right
        case 77 : { // ARROW RIGHT
            LineNode *LL = (*TT)->currLine;

            if((*TT)->cursorX <= LL->length) {
                if ((*TT)->currChar == NULL) {
                    (*TT)->currChar = LL->firstChar;
                } else {
                    (*TT)->currChar = (*TT)->currChar->next;
                }
                (*TT)->cursorX++;
                (*TT)->targetX = (*TT)->cursorX;
            } else if(LL->down != NULL) {
                (*TT)->currLine = LL->down;
                (*TT)->currChar = NULL;
                (*TT)->cursorY++;
                (*TT)->cursorX = 1;
                (*TT)->targetX = 1;
            }
            
            break;
        }
            
        
        default:
            break;
    }

    if((*TT)->cursorY < 1 || (*TT)->cursorY > SCREEN_HEIGHT) {
        redrawText(*TT);
    } else {
        moveCursor((*TT)->cursorY, (*TT)->cursorX);
    }
}
