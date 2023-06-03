#include "../include/main.h"

int find(DirectoryTree* dirTree, char* cmd)
{
    char* str;
    pthread_t threadPool[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];

    int thread_cnt = 0;

    if(cmd == NULL){
        FindDir(dirTree, dirTree->current->name);
        return 0;
    }
    else if(cmd[0] == '-'){
        if(strcmp(cmd, "--help") == 0){
            printf("Usage: find [-H] [-L] [-P] [-Olevel] [-D debugopts] [path...] [expression]...\n\n");
            
            printf("default path is the current directory; default expression is -print\n");
            printf("expression may consist of: operators, options, tests, and actions:\n");
            printf("operators (decreasing precedence; -and is implicit where no others are given):\n");
            printf("      ( EXPR )   ! EXPR   -not EXPR   EXPR1 -a EXPR2   EXPR1 -and EXPR2)\n");
            printf("      EXPR1 -o EXPR2   EXPR1 -or EXPR2   EXPR1 , EXPR2\n");
            printf("positinal options (always true): -daystart -follow -regextype\n\n");
            
            printf("normal options (always true, specified before other expressions):\n");
            printf("      -depth --help -maxdepth LEVELS -mindepth LEVELS -mount -noleaf\n");
            printf("      --version -xdev -ignore_readdir_race -noignore_readdir_race\n");
            printf("tests (N can be +N or -N or N): -amin N -anewer FILE -atime N -cmin N\n");
            printf("      -cnewer FILE -ctime N -empty -flase -fstype TYPE -gid N -group NAME\n");
            printf("      -ilname PATTERN -iname PATTERN -inum N -iwholename PATTERN -iregex PATTERN\n");
            printf("      -links N -lname PATTERN -mmin N -mtime N -name PATTERN -newer FILE\n");
            printf("      -nouser -nogroup -path PATTERN -perm [-/]MODE -regex PATTERN\n");
            printf("      -readbale -writable -executable\n");
            printf("      -wholename PATTERN -size N[bcwkMG] -true -type [bcdpflsD] -uid N\n");
            printf("      -used N -user NAME -xtype [bcdpfls]      -context CONTEXT\n\n");
            
            printf("actions: -delete -print0 -printf FORMAT -fprintf FILE FORMAT -print\n");
            printf("      -fprint0 FILE -fprint FILE -ls -fls FILE -prune -quit\n");
            printf("      -exec COMMAND ; -exec COMMAND {} + -ok COMMAND ;\n");
            printf("      -execdir COMMAND ; -exerdir COMMAND {} + -okdir COMMAND ;\n\n");
            
            printf("Valid arguments for -D:\n");
            printf("exec, opt, rates, search, stat, time, tree, all, help\n");
            printf("Use '-D help' for a description of the options, or see find(1)\n\n");
            
            printf("Please see also the documentation at https://www.gnu.org/software/findutils/.\n");
            printf("You can report (and track progress on fixing) bugs in the \"find\"\n");
            printf("program via the GNU findutils bug-reporting page at\n");
            printf("https://savannah.gnu.org/bugs/?group=findutils or, if\n");
            printf("you have no web access, by sending email to <bug-findutils@gnu.org>.\n");
            return -1;
        }
        else{
            cmd = strtok(cmd, "-");
            printf("find: invalid option -- '%s'\n", cmd);
            printf("Try 'find --help' for more information.\n");
            return -1;
        }
    }
    else
    {
        str = strtok(NULL, " ");
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt++].cmd = cmd;
    }
    while (str != NULL) {   //멀티스레드 작업을 위해 파일이름마다 스레드배열안에 정보를 저장
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt++].cmd = str;
        str = strtok(NULL, " ");
    }
    for (int i = 0; i < thread_cnt; i++) {      //pthread생성 후 cat_thread로 처리, 마지막으로 join
        pthread_create(&threadPool[i], NULL, find_thread, (void*)&threadTree[i]);
        pthread_join(threadPool[i], NULL);
    }
    return 0;
}

void *find_thread(void *arg) {     //파일마다 스레드로 실행되는 함수
    ThreadTree *threadTree = (ThreadTree *) arg;
    DirectoryNode* currentNode = NULL;
    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;
    DirectoryTree *dirTree = threadTree->threadTree;
    char *str;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    char tmp4[MAX_DIR];
    char *cmd = threadTree->cmd;
    int val;

    strncpy(tmp, cmd, MAX_DIR);
    currentNode = dirTree->current;
    if (strstr(tmp, "/") == NULL) {     //위치한 디렉토리 안에 있는 파일일 경우
        tmpNode = IsExistDir(dirTree, cmd, 'd');
        tmpNode2 = IsExistDir(dirTree, cmd, 'f');
        if (tmpNode == NULL && tmpNode2 == NULL) {
            printf("find: '%s': No such file or direcory.\n", cmd);
            return NULL;
        }
    }
    else {      //그 외에 다른 디렉토리 안에 있는 파일 불러올 경우
        strncpy(tmp2, getDir(tmp), MAX_DIR);
        strncpy(tmp4, cmd, MAX_DIR);
        val = MovePath(dirTree, tmp2);
        if (val) {      //파일을 가지는 디렉토리가 존재하지 않을 경우
            printf("find: '%s': No such file or directory.\n", tmp2);
            dirTree->current = currentNode;
            return NULL;
        }
        str = strtok(tmp4, "/");
        while (str != NULL) {
            strncpy(tmp3, str, MAX_NAME);
            str = strtok(NULL, "/");
        }
        tmpNode = IsExistDir(dirTree, tmp3, 'd');
        tmpNode2 = IsExistDir(dirTree, tmp3, 'f');
        if(tmpNode == NULL && tmpNode2 == NULL) {       //파일이 존재하지 않을 경우
            printf("find: '%s': No such file or directory.\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        } 
        dirTree->current = currentNode;
    }
    printf("%s\n", cmd);    //자기자신 출력해주기
    FindDir(dirTree, cmd);
    pthread_exit(NULL);
}

int ReadDir(DirectoryTree* dirTree, char* tmp, char* dirName)       //찾은 경로 출력하기
{
    char* str;
    char str2[MAX_NAME];
    char* str3;
    str = strtok(tmp, " ");
    strcpy(str2, str);
    for(int i=0;i<10;i++){
        str = strtok(NULL, " ");
    }
    if(str != NULL){
        str3 = strcasestr(str, dirName);
        if(str3 != NULL){
            str3[strlen(str)-1] = '\0';
            str[strlen(str)-1] = '\0';
            if(strcmp(str, "/") == 0)
                printf("/%s\n", str2);
            else
                 printf("%s/%s\n", str3, str2);
        }
    }
    return 0;
}

void FindDir(DirectoryTree* dirTree, char* dirName)     //find한 디렉토리 directory.txt에서 찾기
{
    char tmp[MAX_LENGTH];
    Dir = fopen("Directory.txt", "r");
    while(fgets(tmp, MAX_LENGTH, Dir) != NULL){
        ReadDir(dirTree, tmp, dirName);
    }
    fclose(Dir);
}