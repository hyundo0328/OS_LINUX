#include "../include/main.h"

//find
int ReadDir(DirectoryTree* dirTree, char* tmp, char* dirName, int o)
{
    char* str;
    char str2[MAX_NAME];
    if(o == 0){
        str = strtok(tmp, " ");
        strcpy(str2, str);
        for(int i=0;i<10;i++){
            str = strtok(NULL, " ");
        }
        if(str != NULL){
            if(strstr(str2, dirName) != NULL){
                str[strlen(str)-1] = '\0';
                if(strcmp(str, "/") == 0)
                    printf("/%s\n", str2);
                else
                    printf("%s/%s\n", str, str2);
            }
        }
    }
    else{
        str = strtok(tmp, " ");
        strcpy(str2, str);
        for(int i=0;i<10;i++){
            str = strtok(NULL, " ");
        }
        if(str != NULL){
            if(strstr(str, dirName) != NULL){
                str[strlen(str)-1] = '\0';
                if(strcmp(str, "/") == 0)
                    printf("/%s\n", str2);
                else
                    printf("%s/%s\n", str, str2);
            }
        }
    }
    return 0;
}

void FindDir(DirectoryTree* dirTree, char* dirName, int o)
{
    char tmp[MAX_LENGTH];

    Dir = fopen("Directory.txt", "r");

    while(fgets(tmp, MAX_LENGTH, Dir) != NULL){
        ReadDir(dirTree, tmp, dirName, o);
    }

    fclose(Dir);
}

int find_(DirectoryTree* dirTree, char* cmd)
{
    char* str;
    if(cmd == NULL){
        FindDir(dirTree, dirTree->current->name, 1);
        return 0;
    }
    else if(cmd[0] == '-'){
        if(strcmp(cmd, "-name") == 0){
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("Try 'find --help' for more information.\n");
                return -1;
            }
            FindDir(dirTree, str, 0);
        }
        else if(strcmp(cmd, "--help") == 0){
            printf("사용법: find [<옵션>]... [<파일>]...\n");
            printf("\n");
            printf("  Options:\n");
            printf("    -name\t finds file by name\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else{
            printf("Try 'find --help' for more information.\n");
            return -1;
        }
    }
    else{
        FindDir(dirTree, cmd, 1);
    }

    return 0;
}
