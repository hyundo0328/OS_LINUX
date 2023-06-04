#include "../include/main.h"

int cat(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* currentNode = NULL;
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    pthread_t threadArr[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];

    int thread_cnt = 0;
    char* str;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int val, option = 1;

    if(cmd == NULL){    //cat 이외에 옵션, 파일이름 모두 입력 안했을 때
        char buf[MAX_BUFFER];
        char *buf2 = (char*)malloc(MAX_BUFFER);
        int num = 0;
       while(fgets(buf, sizeof(buf), stdin)){   //표준입력을 받아주며 한 줄을 입력할 때 마다 출력해줌
            buf2 = strcpy(buf2, buf);
            printf("%s", buf2);
        }
        rewind(stdin);  //ctrl+d를 누르면 탈출
        return -1;
    }
    currentNode = dirTree->current;

    if(strcmp(cmd, ">") == 0){  // > 옵션일 때
        str = strtok(NULL, " ");
        if(str == NULL){
            printf("bash: sysntax error near unexpected token 'newline'\n");
            return -1;
        }
        strncpy(tmp, str, MAX_DIR);
        if(strstr(str, "/") == NULL){       //현재 디렉토리 안의 파일 불러올 경우
            if(HasPermission(dirTree->current, 'w') != 0){
                printf("cat: cannot create directory: '%s': Permission denied\n", dirTree->current->name);
                return -1;
            }
            tmpNode = IsExistDir(dirTree, str, 'd');    //디렉토리일 경우
            if(tmpNode != NULL || strcmp(str, ".") == 0 || strcmp(str, "..") == 0){
                printf("cat: cannot create directory: '%s': Is a directory\n", str);
                return -1;
            }
            else
                cat_print(dirTree, str, 0);
        }
        else{       //그 외에 다른 디렉토리에서 파일을 불러올 경우
            if (strcmp(str, "/") == 0)
            {
                printf("cat: cannot create directory: '/': Is a directory\n");
                return -1;
            }
            strncpy(tmp2, getDir(str), MAX_DIR);
            val = MovePath(dirTree, tmp2);
            if(val != 0){
                printf("cat: '%s': No such file or directry\n", tmp2);
                return -1;
            }
            str = strtok(tmp, "/");
            while(str != NULL){
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            if(HasPermission(dirTree->current, 'w') != 0){
                printf("cat: cannot create directory: '%s': Permission denied\n", dirTree->current->name);
                dirTree->current = currentNode;
                return -1;
            }
            tmpNode = IsExistDir(dirTree, tmp3, 'd');
            if(tmpNode != NULL){
                printf("cat: '%s': Is a directory\n", tmp3);
                dirTree->current = currentNode;
                return -1;
            }
            else
                cat_print(dirTree, tmp3, 0);
            dirTree->current = currentNode;
        }
        return 0;
    }
    else if(cmd[0] == '-'){     //옵션이 있을 때
        if(strcmp(cmd, "-n")== 0){      //n 옵션일 때
            str = strtok(NULL, " ");
            if (str == NULL){
                printf("cat: missing operand\n");
                printf("Try 'cat --help' for more information.\n");
                return -1;
            }
            option = 2;
        }
        else if(strcmp(cmd, "--help") == 0){    //--help 입력시
            printf("Usage: cat [OPTION]... [FILE]...\n");
            printf("cat_print FILE(s) to standard output.\n\n");
  
            printf("With no FILE, or when FILS is -, read standard input.\n\n");
    
            printf("  -n, --number         \t number all output line\n");
            printf("        --help\t display this help and exit\n\n");
            
            printf("Examples:\n");
            printf("  cat f - g Output f's contents, then standard input, then g's contents\n");
            printf("  cat\t Copy standard input to standard output.\n\n");
            
            printf("GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n");
            printf("Report any translation bugs to <https://translationproject.org/team/>\n");
            printf("Full documentation <https://www.gnu.org/software/coreutils/mkdir>\n");
            printf("or available locally via: info '(coreutils) mkdir invocation'\n");
            return -1;
        }
        else{   //그 외의 옵션을 입력했을 때 에러문
            cmd = strtok(cmd, "-");
            printf("cat: invalid option -- '%s'\n", cmd);
            printf("Try 'cat --help' for more information.\n");
            return -1;
        }
    }
    else{   //옵션이 없이 사용했을 때
        if(strcmp(cmd, "/etc/passwd") == 0){
            cat_print(dirTree, cmd, 3);
            return 0;
        }
        str = strtok(NULL, " ");
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = cmd;
    }
    while (str != NULL) {   //멀티스레드 작업을 위해 파일이름마다 스레드배열안에 정보를 저장
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = str;
        str = strtok(NULL, " ");
    }
    for (int i = 0; i < thread_cnt; i++) {      //pthread생성 후 cat_thread로 처리, 마지막으로 join
        pthread_create(&threadArr[i], NULL, cat_thread, (void*)&threadTree[i]);
        pthread_join(threadArr[i], NULL);
    }
    return 1;
}

void *cat_thread(void *arg) {   //파일마다 스레드로 실행되는 함수
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
    if (strstr(tmp, "/") == NULL) {     //위치한 디렉토리 안에 있는 파일일 경우
        if (HasPermission(dirTree->current, 'w')) {
            printf("cat: Can not create file '%s': Permission denied\n", dirTree->current->name);
            return NULL;
        }
        tmpNode = IsExistDir(dirTree, cmd, 'd');
        tmpNode2 = IsExistDir(dirTree, cmd, 'f');
        if ((tmpNode != NULL && tmpNode2 == NULL) || strcmp(tmp, ".") == 0 || strcmp(tmp, "..") == 0) {
            printf("cat: '%s': Is a directory\n", cmd);
            return NULL;
        }
        else if (tmpNode == NULL && tmpNode2 == NULL) {
            printf("cat: '%s': No such file or direcory.\n", cmd);
            return NULL;
        }
        else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
            printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
            return NULL;
        } else 
            cat_print(dirTree, cmd, option);
    }
    else {      //그 외에 다른 디렉토리 안에 있는 파일 불러올 경우
        strncpy(tmp2, getDir(tmp), MAX_DIR);
        val = MovePath(dirTree, tmp2);
        if (val) {      //파일 경로가 존재하지 않을 경우
            printf("cat: '%s': No such file or directory.\n", tmp2);
            return NULL;
        }
        str = strtok(cmd, "/");
        if (str == NULL)
        {
            printf("cat: '/': Is a directory.\n");
            return NULL;
        }
        while (str != NULL) {
            strncpy(tmp3, str, MAX_NAME);
            str = strtok(NULL, "/");
        }
        tmpNode = IsExistDir(dirTree, tmp3, 'd');
        tmpNode2 = IsExistDir(dirTree, tmp3, 'f');
        if(tmpNode == NULL && tmpNode2 == NULL) {       //파일이 존재하지 않을 경우
            printf("cat: '%s': No such file or directory.\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        } 
        else if (tmpNode != NULL && tmpNode2 == NULL) {     //디렉토리일 경우
            printf("cat: '%s': Is a directory\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        } 
        else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {       //권한때문에 허가거부된 경우
            printf("cat: Can not open file '%s': Permission denied\n", tmpNode2->name);
            dirTree->current = currentNode;
            return NULL;
        } 
        else 
            cat_print(dirTree, tmp3, option);
        dirTree->current = currentNode;
    }
    pthread_exit(NULL);     //스레드 실행 끝
}
//cat
int cat_print(DirectoryTree* dirTree, char* fName, int o)     //cat명령어 수행을 본격적으로 해주는 함수
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
        if(o == 3){     // /etc/passwd에 들어갈 경우
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
            if(o == 2){     // n 옵션일 경우
                if(buf[strlen(buf)-1] == '\n'){
                    printf("     %d\t",cnt);
                    cnt++;
                }
            }
            fputs(buf, stdout);
        }

        fclose(fp);
    }
    else{       // > 옵션일 때
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
        else{       //파일 없을 경우 생성
            MakeDir(dirTree, fName, 'f');
        }
        //write size
        tmpNode = IsExistDir(dirTree, fName, 'f');
        tmpNode->SIZE = tmpSIZE;
    }
    return 0;
}