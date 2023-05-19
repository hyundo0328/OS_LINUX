#include "../include/main.h"

void DestroyDir(DirectoryNode* dirNode)
{
    if(dirNode->RightSibling != NULL){
        DestroyDir(dirNode->RightSibling);
    }
    if(dirNode->LeftChild != NULL){
        DestroyDir(dirNode->LeftChild);
    }

    dirNode->LeftChild = NULL;
    dirNode->RightSibling = NULL;

    DestroyNode(dirNode);
}

DirectoryNode* IsExistDir(DirectoryTree* dirTree, char* dirName, char type)
{
    //variables
    DirectoryNode* returnNode = NULL;

    returnNode = dirTree->current->LeftChild;

    while(returnNode != NULL){
        if(strcmp(returnNode->name, dirName) == 0 && returnNode->type == type)
            break;
        returnNode = returnNode->RightSibling;
    }

    return returnNode;
}

char* getDir(char* dirPath)
{
    char* tmpPath = (char*)malloc(MAX_DIR);
    char* str = NULL;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];

    strncpy(tmp, dirPath, MAX_DIR);
    str = strtok(dirPath, "/");
    while(str != NULL){
        strncpy(tmp2, str, MAX_DIR);
        str  = strtok(NULL, "/");
    }
    strncpy(tmpPath, tmp, strlen(tmp)-strlen(tmp2)-1);
    tmpPath[strlen(tmp)-strlen(tmp2)-1] = '\0';

    return tmpPath;
}

void SaveDir(DirectoryTree* dirTree, Stack* dirStack)
{

    Dir = fopen("Directory.txt", "w");

    WriteNode(dirTree, dirTree->root, dirStack);

    fclose(Dir);
}
DirectoryTree* LoadDir()
{
    DirectoryTree* dirTree = (DirectoryTree*)malloc(sizeof(DirectoryTree));
    char tmp[MAX_LENGTH];

    Dir = fopen("Directory.txt", "r");

    while(fgets(tmp, MAX_LENGTH, Dir) != NULL){
        ReadNode(dirTree, tmp);
    }

    fclose(Dir);

    dirTree->current = dirTree->root;

    return dirTree;
}