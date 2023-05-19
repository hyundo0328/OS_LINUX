#include "../include/main.h"

//chmod
int ChangeMode(DirectoryTree* dirTree, int mode, char* dirName)
{
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;

    tmpNode = IsExistDir(dirTree, dirName, 'd');
    tmpNode2 = IsExistDir(dirTree, dirName, 'f');

    if(tmpNode != NULL){
        if(HasPermission(tmpNode, 'w') != 0){
            printf("chmod: '%s'파일을 수정할 수 없음: 허가거부\n", dirName);
            return -1;
        }
        tmpNode->mode = mode;
        Mode2Permission(tmpNode);
    }
    else if(tmpNode2 != NULL){
        if(HasPermission(tmpNode2, 'w') != 0){
            printf("chmod: '%s'파일을 수정할 수 없음: 허가거부\n", dirName);
            return -1;
        }
        tmpNode2->mode = mode;
        Mode2Permission(tmpNode2);
    }
    else{
        printf("chmod: '%s에 접근할 수 없습니다: 그런 파일이나 디렉터리가 없습니다\n", dirName);
        return -1;
    }
    return 0;
}

void ChangeModeAll(DirectoryNode* dirNode, int mode)
{
    if(dirNode->RightSibling != NULL){
        ChangeModeAll(dirNode->RightSibling, mode);
    }
    if(dirNode->LeftChild != NULL){
        ChangeModeAll(dirNode->LeftChild, mode);
    }
    dirNode->mode = mode;
    Mode2Permission(dirNode);
}

int chmod(DirectoryTree* dirTree, char* cmd)         //완료
{
    DirectoryNode* tmpNode = NULL;
    char* str;
    int tmp;

    if(cmd == NULL){
        printf("Try 'chmod --help' for more information.\n");
        return -1;
    }
    if(cmd[0] == '-'){
        if(strcmp(cmd, "--help") == 0){
            printf("사용법: chmod [옵션]... 8진수-MODE... 디렉터리...\n");
            printf("  Change the mode of each FILE to MODE.\n\n");
            printf("  Options:\n");
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
        }
        return -1;
    }
    else{
        if(cmd[0]-'0'<8 && cmd[1]-'0'<8 && cmd[2]-'0'<8 && strlen(cmd)==3){
            tmp = atoi(cmd);
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            ChangeMode(dirTree, tmp, str);
        }
        else{
            printf("chmod: Invalid file mode: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}