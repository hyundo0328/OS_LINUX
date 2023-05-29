#include "../include/main.h"

int pwd(DirectoryTree* dirTree, Stack* dirStack, char* cmd)
{
    char* str = NULL;
    if(cmd == NULL){
        PrintPath(dirTree, dirStack);
    }
    else if(cmd[0] == '-'){
        if(strcmp(cmd, "--help") == 0){
            printf("pwd: pwd [-LP]\n");
            printf("  Print the name of the current working directory.\n\n");
            
            printf("  Options:\n");
            printf("    -L\t print the value of $PWD if it names the current working\n");
            printf("  \t directory\n");
            printf("    -P\t print the physical direcrtory, without any symbolic links\n\n");
            
            printf("  By default, 'pwd' behaves as if '-L' were specified.\n\n");
            
            printf("  Exit status:\n");
            printf("  Returns 0 unless an invalid option is given or the current directory\n");
            printf("  cannot be read.\n");
        }
        return -1;
    }
    return 0;
}

int PrintPath(DirectoryTree* dirTree, Stack* dirStack)
{
    //variables
    DirectoryNode* tmpNode = NULL;

    tmpNode = dirTree->current;

    //if current directory is root
    if(tmpNode == dirTree->root){
        printf("/");
    }
    else{
        //until current directory is root, repeat Push
        while(tmpNode->Parent != NULL){
            Push(dirStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
    //until stack is empty, repeat Pop
        while(IsEmpty(dirStack) == 0){
            printf("/");
            printf("%s",Pop(dirStack));
        }
    }
    printf("\n");

    return 0;
}