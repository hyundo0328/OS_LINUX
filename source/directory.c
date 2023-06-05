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
    DirectoryNode* returnNode = NULL;

    returnNode = dirTree->current->LeftChild;
    while(returnNode != NULL){
        if(strcasecmp(returnNode->name, dirName) == 0 && returnNode->type == type)
            break;
        returnNode = returnNode->RightSibling;
    }
    return returnNode;
}

char* getDir(char* dirPath)
{
    char* tmpPath = (char*)malloc(MAX_DIR);
    char* directory = NULL;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];

    strncpy(tmp, dirPath, MAX_DIR);
    directory = strtok(dirPath, "/");
    while(directory != NULL){
        strncpy(tmp2, directory, MAX_DIR);
        directory  = strtok(NULL, "/");
    }
    strncpy(tmpPath, tmp, strlen(tmp)-strlen(tmp2)-1);
    tmpPath[strlen(tmp)-strlen(tmp2)-1] = '\0';
    return tmpPath;
}

void SaveDir(DirectoryTree* dirTree, Stack* dirStack)
{

    Directory = fopen("Directory.txt", "w");
    WriteNode(dirTree, dirTree->root, dirStack);
    fclose(Directory);
}

DirectoryTree* LoadDir()
{
    DirectoryTree* dirTree = (DirectoryTree*)malloc(sizeof(DirectoryTree));
    char tmp[MAX_LENGTH];

    Directory = fopen("Directory.txt", "r");
    while(fgets(tmp, MAX_LENGTH, Directory) != NULL){
        ReadNode(dirTree, tmp);
    }
    fclose(Directory);
    dirTree->current = dirTree->root;
    return dirTree;
}