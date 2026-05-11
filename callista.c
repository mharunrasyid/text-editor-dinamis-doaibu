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
