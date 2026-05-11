#include <stdio.h>
#include "CONFIG.h"

void setStructEditor() {
    E.activeTab = NULL; 
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


