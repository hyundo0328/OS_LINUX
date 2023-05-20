#include "../include/main.h"

void *cat_thread(void *arg) {
    ThreadTree *threadTree = (ThreadTree *)arg;
    DirectoryTree *dirTree = threadTree->threadTree;
    char *cmd = threadTree->cmd;
    DirectoryNode *currentNode = dirTree->current;
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    char *str;
    int option = threadTree->option;
    int val;

    strncpy(tmp, cmd, MAX_DIR);

    if (option == 1) {
        if (strstr(tmp, "/") == NULL) {
            if (HasPermission(dirTree->current, 'w')) {
                printf("cat: Can not create file '%s': Permission denied\n", dirTree->current->name);
                return NULL;
            }
            tmpNode = IsExistDir(dirTree, cmd, 'd');
            tmpNode2 = IsExistDir(dirTree, cmd, 'f');
            if (tmpNode == NULL && tmpNode2 == NULL) {
                printf("cat: '%s': No such file or directory.\n", cmd);
                return NULL;
            } else if (tmpNode != NULL && tmpNode2 == NULL) {
                printf("cat: '%s': Is a directory\n", cmd);
                return NULL;
            } else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
                printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                return NULL;
            } else 
                Concatenate(dirTree, cmd, 2);
        } else {
            strncpy(tmp2, getDir(tmp), MAX_DIR);
            val = MovePath(dirTree, tmp2);
            if (val) {
                printf("cat: '%s': No such file or directory.\n", tmp2);
                return NULL;
            }
            str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            tmpNode = IsExistDir(dirTree, tmp3, 'd');
            tmpNode2 = IsExistDir(dirTree, tmp3, 'f');
            if(tmpNode == NULL && tmpNode2 == NULL) {
                printf("cat: '%s': No such file or directory.\n", tmp3);
                dirTree->current = currentNode;
                return NULL;
            } else if (tmpNode != NULL && tmpNode2 == NULL) {
                printf("cat: '%s': Is a directory\n", tmp3);
                dirTree->current = currentNode;
                return NULL;
            } else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
                printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                dirTree->current = currentNode;
                return NULL;
            } else 
                Concatenate(dirTree, tmp3, 2);
            dirTree->current = currentNode;
        }
    } else {
        if (strstr(tmp, "/") == NULL) {
            if (HasPermission(dirTree->current, 'w') != 0) {
                printf("cat: Can not create file '%s': Permission denied\n", dirTree->current->name);
                return NULL;
            }
            tmpNode = IsExistDir(dirTree, cmd, 'd');
            tmpNode2 = IsExistDir(dirTree, cmd, 'f');
            if (tmpNode == NULL && tmpNode2 == NULL) {
                printf("cat: '%s': No such file or directory.\n", cmd);
                return NULL;
            } else if (tmpNode != NULL && tmpNode2 == NULL) {
                printf("cat: '%s': Is a directory\n", cmd);
                return NULL;
            } else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
                printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                return NULL;
            } else 
                Concatenate(dirTree, cmd, 1);
        } else {
            strncpy(tmp2, getDir(tmp), MAX_DIR);
            val = MovePath(dirTree, tmp2);
            if (val != 0) {
                printf("cat: '%s': No such file or directory.\n", tmp2);
                return NULL;
            }
            str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            tmpNode = IsExistDir(dirTree, tmp3, 'd');
            tmpNode2 = IsExistDir(dirTree, tmp3, 'f');
            if(tmpNode == NULL && tmpNode2 == NULL) {
                printf("cat: '%s': No such file or directory.\n", tmp3);
                dirTree->current = currentNode;
                return NULL;
            } else if (tmpNode != NULL && tmpNode2 == NULL) {
                printf("cat: '%s': Is a directory\n", tmp3);
                dirTree->current = currentNode;
                return NULL;
            } else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
                printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
                dirTree->current = currentNode;
                return NULL;
            } else 
                Concatenate(dirTree, tmp3, 1);
            dirTree->current = currentNode;
        }
    }
    pthread_exit(NULL);
}
//cat
int Concatenate(DirectoryTree* dirTree, char* fName, int o)
{
    UserNode* tmpUser = NULL;
    DirectoryNode* tmpNode = NULL;
    FILE* fp;
    char buf[MAX_BUFFER];
    char tmpName[MAX_NAME];
    char* str;
    int tmpSIZE = 0;
    int cnt = 1;

    //file read
    if(o != 0){
        if(o == 3){
            tmpUser = usrList->head;
            while(tmpUser != NULL){
                printf("%s:x:%d:%d:%s:%s\n", tmpUser->name, tmpUser->UID, tmpUser->GID, tmpUser->name, tmpUser->dir);
                tmpUser = tmpUser->LinkNode;
            }
            return 0;
        }
        tmpNode = IsExistDir(dirTree,fName, 'f');

        if(tmpNode == NULL){
            return -1;
        }
        fp = fopen(fName, "r");

        while(feof(fp) == 0){
            fgets(buf, sizeof(buf), fp);
            if(feof(fp) != 0){
                break;
            }
            //w/ line number
            if(o == 2){
                if(buf[strlen(buf)-1] == '\n'){
                    printf("     %d\t",cnt);
                    cnt++;
                }
            }
            fputs(buf, stdout);
        }

        fclose(fp);
    }
    //file write
    else{
        fp = fopen(fName, "w");
	    while(fgets(buf, sizeof(buf), stdin)){
            fputs(buf, fp);
            //get file size
            tmpSIZE += strlen(buf)-1;
        }
        rewind(stdin);
        fclose(fp);

        tmpNode = IsExistDir(dirTree, fName, 'f');
        //if exist
        if(tmpNode != NULL){
            time(&ltime);
            today = localtime(&ltime);

            tmpNode->month = today->tm_mon + 1;
            tmpNode->day = today->tm_mday;
            tmpNode->hour = today->tm_hour;
            tmpNode->minute = today->tm_min;
        }
        //if file doesn't exist
        else{
            MakeDir(dirTree, fName, 'f');
        }
        //write size
        tmpNode = IsExistDir(dirTree, fName, 'f');
        tmpNode->SIZE = tmpSIZE;
    }
    return 0;
}

int cat(DirectoryTree* dirTree, char* cmd)        //완료
{
    DirectoryNode* currentNode = NULL;
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    pthread_t threadPool[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];

    int thread_cnt = 0;
    char* str;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int val;
    /**
        cat0: write, EOF to save
        cat1: read
        cat2: read w/ line number
    **/
    if(cmd == NULL){
        char buf[MAX_BUFFER];
        char *buf2 = (char*)malloc(MAX_BUFFER);
        int num = 0;
	    while(fgets(buf, sizeof(buf), stdin)){
            buf2 = strcpy(buf2, buf);
            printf("%s", buf2);
        }
        rewind(stdin);
        return -1;
    }
    currentNode = dirTree->current;

    if(strcmp(cmd, ">") == 0){
        str = strtok(NULL, " ");
        if(str == NULL){
            printf("cat: 잘못된 연산자\n");
            printf("Try 'cat --help' for more information.\n");
            return -1;
        }
        strncpy(tmp, str, MAX_DIR);
        if(strstr(str, "/") == NULL){
            if(HasPermission(dirTree->current, 'w') != 0){
                printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", dirTree->current->name);
                return -1;
            }
            tmpNode = IsExistDir(dirTree, str, 'd');
            if(tmpNode != NULL){
                printf("cat: '%s': 디렉터리입니다\n", str);
                return -1;
            }
            else
                Concatenate(dirTree, str, 0);
        }
        else{
            strncpy(tmp2, getDir(str), MAX_DIR);
            val = MovePath(dirTree, tmp2);
            if(val != 0){
                printf("cat: '%s': 그런 파일이나 디렉터리가 없습니다\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while(str != NULL){
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            if(HasPermission(dirTree->current, 'w') != 0){
                printf("cat: '%s'파일을 만들 수 없음: 허가거부\n", dirTree->current->name);
                dirTree->current = currentNode;
                return -1;
            }
            tmpNode = IsExistDir(dirTree, tmp3, 'd');
            if(tmpNode != NULL){
                printf("cat: '%s': 디렉터리입니다\n", tmp3);
                dirTree->current = currentNode;
                return -1;
            }
            else
                Concatenate(dirTree, tmp3, 0);
            dirTree->current = currentNode;
        }
        return 0;
    }
    else if(cmd[0] == '-'){
        if(strcmp(cmd, "-n")== 0){
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("cat: Invalid option\n");
                printf("Try 'cat --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].option = 1;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
        else if(strcmp(cmd, "--help") == 0){
            printf("사용법: cat [<옵션>]... [<파일>]...\n");
            printf("  FILE(들)을 합쳐서 표준 출력으로 보낸다.\n\n");
            printf("  Options:\n");
            printf("    -n, --number         \t number all output line\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else{
            printf("Try 'cat --help' for more information.\n");
            return -1;
        }
    }
    else{
        if(strcmp(cmd, "/etc/passwd") == 0){
            Concatenate(dirTree, cmd, 3);
            return 0;
        }
        if (cmd == NULL){
            printf("Try 'rm --help' for more information.\n");
            return -1;
        }
        str = strtok(NULL, " ");
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = 0;
        threadTree[thread_cnt++].cmd = cmd;
        while (str != NULL) {
            threadTree[thread_cnt].threadTree = dirTree;
            threadTree[thread_cnt].option = 0;
            threadTree[thread_cnt++].cmd = str;
            str = strtok(NULL, " ");
        }
    }
    for (int i = 0; i < thread_cnt; i++) {
        pthread_create(&threadPool[i], NULL, cat_thread, (void*)&threadTree[i]);
        pthread_join(threadPool[i], NULL);
    }
    return 1;
}