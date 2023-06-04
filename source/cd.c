#include "../include/main.h"

int cd(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* tmpNode = NULL;
    char tmp[MAX_DIR];
    int val;

    if(cmd == NULL){    //자신의 홈디렉토리로 이동
        strcpy(tmp, usrList->current->dir);
        MovePath(dirTree, tmp);
    }
    else if(cmd[0] == '-'){     //옵션이 있을 경우
        if(strcmp(cmd, "--help") == 0){     //--help 입력시
            printf("cd: cd [-L|[-P [-e]] [-@]] [dir]\n");
            printf("    Change the shell working directory.\n\n");
            
            printf("    Change the current directory to DIR. The default DIR is the value of the\n");
            printf("    HOME shell variable.\n\n");
            
            printf("    The variable CDPATH defines the search path for the directory containing\n");
            printf("    DIR. Alternative directory names in CDPATH are separated by a colon (:).\n");
            printf("    A null directory name is the same as the current directory. If DIR begins\n");
            printf("    with a slash (/), then CDPATH is not used.\n\n");
            
            printf("    If the directory is not found, and the shell option 'cdable_vars' is set,\n");
            printf("    the word is assumed to be a variable name. If that variable has value,\n");
            printf("    its value is used for DIR.\n\n");
            
            printf("    Options:\n");
            printf("        --help\t display this help and exit\n\n");
            
            printf("The default is to follow symbolic links, as if '-L' were specified.\n");
            printf("'..' is processed by removing the immediately previous pathname component\n");
            printf("back to a slash or the beginning of DIR.\n\n");
            
            printf("Exit Status:\n");
            printf("Returns 0 if the directory is changed, and if $PWD is set successfully when\n");
            printf("-P is used; non-zero otherwise.\n");
        }
        else{
            cmd = strtok(cmd, "-");     //그 외의 옵션들 에러처리
            printf("cd: invalid option -- '%s'\n", cmd);
            printf("cd: usage: cd [-L|[-P [-e]] [-@]] [dir]\n");
        }
        return -1;
    }
    else{   //옵션 없을 경우
        tmpNode = IsExistDir(dirTree, cmd, 'd');
        if(tmpNode != NULL){
            if(HasPermission(tmpNode, 'x') != 0){       //접근권한이 거부되어 있을 경우
                printf("cd: %s: Permission denied\n", cmd);
                return -1;
            }
        }
        tmpNode = IsExistDir(dirTree, cmd,  'f');
        if(tmpNode != NULL){        //디렉토리가 아닐 경우
            printf("cd: %s: Not a directory\n", cmd);
            return -1;
        }
        val = MovePath(dirTree, cmd);
        if(val != 0)        //파일이 없을 경우
            printf("cd: %s: No such file or directory\n", cmd);
    }
    return 0;
}

int Movecurrent(DirectoryTree* dirTree, char* dirPath)      //파일이 있는 디렉토리로 이동
{
    DirectoryNode* tmpNode = NULL;

    if(strcmp(dirPath,".") == 0){
    }
    else if(strcmp(dirPath,"..") == 0){
        if(dirTree->current != dirTree->root){
            dirTree->current = dirTree->current->Parent;
        }
    }
    else{
        tmpNode = IsExistDir(dirTree, dirPath, 'd');
        if(tmpNode != NULL){
            dirTree->current = tmpNode;
        }
        else
            return -1;
    }
    return 0;
}

int MovePath(DirectoryTree* dirTree, char* dirPath)     //경로 이동 함수
{
    //variables
    DirectoryNode* tmpNode = NULL;
    char tmpPath[MAX_DIR];
    char* str = NULL;
    int val = 0;

    //set tmp
    strncpy(tmpPath, dirPath, MAX_DIR);
    tmpNode = dirTree->current;
    if(strcmp(dirPath, "/") == 0){      // 최상위 디렉토리일 경우
        dirTree->current = dirTree->root;
    }
    else{
        if(strncmp(dirPath, "/",1) == 0){
            if(strtok(dirPath, "/") == NULL){
                return -1;
            }
            dirTree->current = dirTree->root;
        }
        str = strtok(tmpPath, "/");
        while(str != NULL){
            val = Movecurrent(dirTree, str);
            if(val != 0){   //경로가 없을 경우
                dirTree->current = tmpNode;
                return -1;
            }
            str = strtok( NULL, "/");
        }
    }
    return 0;
}