#include "../include/main.h"

//chown
int ChangeOwner(DirectoryTree* dirTree, char* userName, char* dirName, int flag)
{
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    UserNode* tmpUser = NULL;

    tmpNode = IsExistDir(dirTree, dirName, 'd');
    tmpNode2 = IsExistDir(dirTree, dirName, 'f');


    if(tmpNode != NULL){
        if(HasPermission(tmpNode, 'w') != 0){
            printf("chown: %s: Operation not permitted\n", dirName);
            return -1;
        }
        tmpUser = IsExistUser(usrList, userName);
        if(tmpUser != NULL){
            if (flag == 0)
                tmpNode->UID = tmpUser->UID;
            else
                tmpNode->GID = tmpUser->GID;
        }
        else{
            printf("chown: %s: illegal user name\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else if(tmpNode2 != NULL){
        if(HasPermission(tmpNode2, 'w') != 0){
            printf("chown: %s: Operation not permitted\n", dirName);
            return -1;
        }
        tmpUser = IsExistUser(usrList, userName);
        if(tmpUser != NULL){
            if (flag == 0)
                tmpNode->UID = tmpUser->UID;
            else
                tmpNode->GID = tmpUser->GID;
        }
        else{
            printf("chown: %s: illegal user name\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else{
        printf("chown: %s: No such file or directory\n", dirName);
        return -1;
    }

    return 0;
}

void ChangeOwnerAll(DirectoryNode* dirNode, char* userName)
{
    UserNode* tmpUser = NULL;

    tmpUser = IsExistUser(usrList, userName);

    if(dirNode->RightSibling != NULL){
        ChangeOwnerAll(dirNode->RightSibling, userName);
    }
    if(dirNode->LeftChild != NULL){
        ChangeOwnerAll(dirNode->LeftChild, userName);
    }
    dirNode->UID = tmpUser->UID;
    dirNode->GID = tmpUser->GID;
}

int chown_(DirectoryTree* dirTree, char* cmd)      //완료
{
    DirectoryNode* tmpNode = NULL;
    UserNode* tmpUser = NULL;
    char* str;
    char tmp[MAX_NAME];

    if(cmd == NULL){
        printf("Try 'chown --help' for more information.\n");
        return -1;
    }
    if(cmd[0] == '-'){
        if(strcmp(cmd, "--help") == 0){
            printf("사용법: chown [옵션]... [소유자]... 파일...\n");
            printf("  Change the owner and/or group of each FILE to OWNER and/or GROUP.\n\n");
            printf("  Options:\n");
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
        }
        return -1;
    }
    else{
        strncpy(tmp, cmd, MAX_NAME);
        str = strtok(NULL, " ");
        if(str == NULL){
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
        else{
            //그룹
            if (strstr(tmp, ":") == NULL){
                ChangeOwner(dirTree, tmp, str, 0);
            }
            else{
                char tmp2[MAX_NAME];
                strncpy(tmp2, tmp, MAX_NAME);
                char *str2 = strtok(tmp, ":");
                if (str2 != NULL && strcmp(tmp, tmp2) != 0){
                    ChangeOwner(dirTree, str2, str, 0);
                    str2 = strtok(NULL, " ");
                    if (str2 != NULL)
                        ChangeOwner(dirTree, str2, str, 1);
                }
                else if (str2 != NULL && strcmp(tmp, tmp2) == 0)
                    ChangeOwner(dirTree, str2, str, 1);
            }
        }
    }
    return 0;
}
