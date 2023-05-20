#include "../include/main.h"

int grep_print(DirectoryTree* dirTree, char *search, char* fName, int o)
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
        else if (o == 0)
        {
            if (strstr(buf, search) != NULL)
                printf("%s", buf);
        }
        else if (o == 1)
        {
            if (strstr(buf, search) != NULL)
                printf("%d:%s ", cnt, buf);
        }
        else if (o == 2)
        {
            if (strstr(buf, search) == NULL)
                printf("%s", buf);
        }
        else if (o == 3)
        {
            if (strcasestr(buf, search) != NULL)
                printf("%s", buf);
        }
        else if (o == 4)
        {
            if (strstr(buf, search) == NULL)
                printf("%s", buf);
            else
                printf("\n");
        }
        else if (o == 5)
        {
            if (strcasestr(buf, search) != NULL)
                printf("%d:%s", cnt, buf);
        }
        else if (o == 6)
        {
            if (strcasestr(buf, search) == NULL)
                printf("%s", buf);
        }
        else if(o == 7){
            if (strcasestr(buf, search) == NULL)
                printf("%d:%s", cnt, buf);
        }
        cnt++;
    }
    fclose(fp);
    return 0;
}

void *grep_thread(void *arg){
    ThreadTree *threadTree = (ThreadTree *)arg;
    DirectoryTree *dirTree = threadTree->threadTree;
    char *cmd = threadTree->cmd;
    char *con = threadTree->content;
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

    if (strstr(tmp, "/") == NULL) {
        tmpNode = IsExistDir(dirTree, cmd, 'd');
        tmpNode2 = IsExistDir(dirTree, cmd, 'f');
        if (tmpNode == NULL && tmpNode2 == NULL) {
            printf("grep: '%s': No such file or directory.\n", cmd);
            return NULL;
        } else if (tmpNode != NULL && tmpNode2 == NULL) {
            printf("grep: '%s': Is a directory\n", cmd);
            return NULL;
        } else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
            printf("grep: Can not open file '%s': Permission denied\n", tmpNode2->name);
            return NULL;
        } else 
            grep_print(dirTree, con, cmd, option);
    } else {
        strncpy(tmp2, getDir(tmp), MAX_DIR);
        val = MovePath(dirTree, tmp2);
        if (val) {
            printf("grep: '%s': No such file or directory.\n", tmp2);
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
            printf("grep: '%s': No such file or directory.\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        } else if (tmpNode != NULL && tmpNode2 == NULL) {
            printf("grep: '%s': Is a directory\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        } else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
            printf("grep: Can not open file '%s': Permission denied\n", tmpNode2->name);
            dirTree->current = currentNode;
            return NULL;
        } else 
            grep_print(dirTree, con, cmd, option);
        dirTree->current = currentNode;
    }
    pthread_exit(NULL);
}


int grep(DirectoryTree* dirTree, char* cmd)        //완료
{
    DirectoryNode* currentNode = NULL;
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    pthread_t threadPool[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];

    int thread_cnt = 0;
    char* str;
    char con[MAX_BUFFER];
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int val;

    if(cmd == NULL){
        printf("Try 'grep --help' for more information.\n");
        return -1;
    }
    currentNode = dirTree->current;
    if(cmd[0] == '-'){
        if(strcmp(cmd, "-n")== 0){
            str = strtok(NULL, " ");
            strncpy(con, str, MAX_BUFFER);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("Try 'grep --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].option = 1;
                threadTree[thread_cnt].content = con;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
        else if(strcmp(cmd, "-v")== 0){
            str = strtok(NULL, " ");
            strncpy(con, str, MAX_BUFFER);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("Try 'grep --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].option = 2;
                threadTree[thread_cnt].content = con;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
        else if(strcmp(cmd, "-i")== 0){
            str = strtok(NULL, " ");
            strncpy(con, str, MAX_BUFFER);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("Try 'grep --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].option = 3;
                threadTree[thread_cnt].content = con;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
        else if(strcmp(cmd, "-nv")== 0 || strcmp(cmd, "-vn") == 0){
            str = strtok(NULL, " ");
            strncpy(con, str, MAX_BUFFER);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("Try 'grep --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].option = 4;
                threadTree[thread_cnt].content = con;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
        else if(strcmp(cmd, "-ni")== 0 || strcmp(cmd, "-in") == 0){
            str = strtok(NULL, " ");
            strncpy(con, str, MAX_BUFFER);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("Try 'grep --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].option = 5;
                threadTree[thread_cnt].content = con;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
        else if(strcmp(cmd, "-vi")== 0 || strcmp(cmd, "-iv") == 0){
            str = strtok(NULL, " ");
            strncpy(con, str, MAX_BUFFER);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("Try 'grep --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].option = 6;
                threadTree[thread_cnt].content = con;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
        else if(strcmp(cmd, "-inv")== 0 || strcmp(cmd, "-ivn") == 0 || strcmp(cmd, "-niv") == 0 || strcmp(cmd, "-nvi") == 0 || strcmp(cmd, "-vin") == 0 || strcmp(cmd, "-vni") == 0){
            str = strtok(NULL, " ");
            strncpy(con, str, MAX_BUFFER);
            str = strtok(NULL, " ");
            if (str == NULL) {
                printf("Try 'grep --help' for more information.\n");
                return -1;
            }
            while (str != NULL) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].option = 7;
                threadTree[thread_cnt].content = con;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
        else if(strcmp(cmd, "--help") == 0){
            printf("사용법: grep [<옵션>]... [<파일>]...\n");
            printf("  FILE(들)을 합쳐서 표준 출력으로 보낸다.\n\n");
            printf("  Options:\n");
            printf("    -n, --number         \t number all output line.\n");
            printf("    -v, --number         \t Selected lines are those not matching any of the specified patterns.\n");
            printf("    -i, --number         \t Perform case insensitive matching.\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else{
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
    }
    else{
        strncpy(con, cmd, MAX_BUFFER);
        str = strtok(NULL, " ");
        if (str == NULL){
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
        while (str != NULL) {
            threadTree[thread_cnt].threadTree = dirTree;
            threadTree[thread_cnt].option = 0;
            threadTree[thread_cnt].content = con;
            threadTree[thread_cnt++].cmd = str;
            str = strtok(NULL, " ");
        }
    }
    for (int i = 0; i < thread_cnt; i++) {
        pthread_create(&threadPool[i], NULL, grep_thread, (void*)&threadTree[i]);
        pthread_join(threadPool[i], NULL);
    }
    return 1;
}