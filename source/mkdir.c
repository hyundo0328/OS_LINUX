#include "../include/main.h"

//mkdir
DirectoryTree* InitializeTree()
{
    //variables
    DirectoryTree* dirTree = (DirectoryTree*)malloc(sizeof(DirectoryTree));
    DirectoryNode* NewNode = (DirectoryNode*)malloc(sizeof(DirectoryNode));
    //get time
    time(&ltime);
    today = localtime(&ltime);
    //set NewNode
    strncpy(NewNode->name, "/", MAX_NAME);
    //rwxr-xr-x
    NewNode->type ='d';
    NewNode->mode = 755;
    Mode2Permission(NewNode);
    NewNode->UID = usrList->head->UID;
    NewNode->GID = usrList->head->GID;
    NewNode->SIZE = 4096;
    NewNode->month = today->tm_mon+1;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->minute = today->tm_min;
    NewNode->Parent = NULL;
    NewNode->LeftChild = NULL;
    NewNode->RightSibling = NULL;

    //set dirTree
    dirTree->root = NewNode;
    dirTree->current = dirTree->root;

    return dirTree;
}

//type==0: folder, type==1: file
int MakeDir(DirectoryTree* dirTree, char* dirName, char type)
{
    //variables
    DirectoryNode* NewNode = (DirectoryNode*)malloc(sizeof(DirectoryNode));
    DirectoryNode* tmpNode = NULL;

    if(HasPermission(dirTree->current, 'w') != 0){
        printf("mkdir: %s: Permission denied\n", dirName);
        free(NewNode);
        return -1;
    }
    if(strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0){
        printf("mkdir: %s: File exists\n", dirName);
        free(NewNode);
        return -1;
    }
    tmpNode = IsExistDir(dirTree, dirName, type);
    if(tmpNode != NULL && tmpNode->type == 'd'){
        printf("mkdir: %s: File exists\n", dirName);
        free(NewNode);
        return -1;
    }
    //get time
    time(&ltime);
    today = localtime(&ltime);

    //initialize NewNode
    NewNode->LeftChild = NULL;
    NewNode->RightSibling = NULL;

    //set NewNode
    strncpy(NewNode->name, dirName, MAX_NAME);
    if(dirName[0] == '.'){
	    NewNode->type = 'd';
        //rwx------
        NewNode->mode = 700;
        NewNode->SIZE = 4096;
    }
    else if(type == 'd'){
        NewNode->type = 'd';
        //rwxr-xr-x
        NewNode->mode = 755;
        NewNode->SIZE = 4096;
    }
    else{
        NewNode->type = 'f';
        //rw-r--r--
        NewNode->mode = 644;
        NewNode->SIZE = 0;
    }
    Mode2Permission(NewNode);
    NewNode->UID = usrList->current->UID;
    NewNode->GID = usrList->current->GID;
    NewNode->month = today->tm_mon + 1;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->minute = today->tm_min;
	NewNode->Parent = dirTree->current;

	if(dirTree->current->LeftChild == NULL){
        dirTree->current->LeftChild = NewNode;
	}
	else{
        tmpNode = dirTree->current->LeftChild;

        while(tmpNode->RightSibling!= NULL){
            tmpNode = tmpNode->RightSibling;
        }
        tmpNode->RightSibling = NewNode;
	}

    return 0;
}

int mkdir(DirectoryTree* dirTree, char* cmd)        //멀티스레드 구현해야함
{
    DirectoryNode* tmpNode = NULL;
    char* str;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int val;
    int tmpMode;
    if(cmd == NULL){
        printf("mkdir: 잘못된 연산자\n");
        printf("Try 'mkdir --help' for more information.\n");
        return -1;
    }
    tmpNode = dirTree->current;
    if(cmd[0] == '-'){ //옵션 있을 경우
        if(strcmp(cmd, "-p") == 0){
            str = strtok(NULL, " ");
            printf("%s\n", str);
            if(str == NULL){
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            if(strncmp(str, "/", 1) == 0){
                dirTree->current = dirTree->root;
            }
            str = strtok(str, "/");
            while(str != NULL){
                val = Movecurrent(dirTree, str);
                if(val != 0){
                    MakeDir(dirTree, str, 'd');
                    Movecurrent(dirTree, str);
                }
                str = strtok(NULL, "/");
            }
            dirTree->current = tmpNode;
        }
        else if(strcmp(cmd, "-m") == 0){
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            if(str[0]-'0'<8 && str[1]-'0'<8 && str[2]-'0'<8 && strlen(str)==3){
                tmpMode = atoi(str);
                str = strtok(NULL, " ");
                if(str == NULL){
                    printf("mkdir: 잘못된 연산자\n");
                    printf("Try 'mkdir --help' for more information.\n");
                    return -1;
                }
                val = MakeDir(dirTree, str, 'd');
                if(val == 0){
                    tmpNode = IsExistDir(dirTree, str, 'd');
                    tmpNode->mode = tmpMode;
                    Mode2Permission(tmpNode);
                }
            }
            else{
                printf("mkdir: 잘못된 모드: '%s'\n", str);
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
        }
        else if(strcmp(cmd, "--help") == 0){
            printf("사용법: mkdir [옵션]... 디렉터리...\n");
            printf("  Create the DIRECTORY(ies), if they do not already exists.\n\n");
            printf("  Options:\n");
            printf("    -m, --mode=MODE\t set file mode (as in chmod)\n");
            printf("    -p, --parents  \t no error if existing, make parent directories as needed\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else{
            str = strtok(cmd, "-");
            if(str == NULL){
                printf("mkdir: 잘못된 연산자\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            else{
                printf("mkdir: 부적절한 옵션 -- '%s'\n", str);
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
        }
    }
    else{ //옵션 없을 경우
        str = strtok(NULL, " ");
        if(str == NULL){ //하나 생성
            strncpy(tmp, cmd, MAX_DIR);
            if(strstr(cmd, "/") == NULL){
                MakeDir(dirTree, cmd, 'd');
            }
            else{
                strncpy(tmp2, getDir(cmd), MAX_DIR);
                val = MovePath(dirTree, tmp2);
                if(val != 0){
                    printf("mkdir: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                    return -1;
                }
                str = strtok(tmp, "/");
                while(str != NULL){
                    strncpy(tmp3, str, MAX_NAME);
                    str = strtok(NULL, "/");
                }
                MakeDir(dirTree, tmp3 , 'd');
                dirTree->current = tmpNode;
            }
        }
    }

    return 0;
}