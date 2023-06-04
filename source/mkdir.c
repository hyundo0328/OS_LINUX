#include "../include/main.h"

int mkdir(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* tmpNode = NULL;
    char* str;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int val, option = 0;
    int tmpMode;
    if(cmd == NULL){ // 아무 method를 입력하지 않았을 때
        printf("mkdir: missing operand\n");
        printf("Try 'mkdir --help' for more information.\n");
        return -1;
    }
    
    int thread_cnt = 0;
    pthread_t threadArr[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];
    tmpNode = dirTree->current;
    if(cmd[0] == '-'){ //옵션 있을 경우
        if(strcmp(cmd, "-p") == 0){
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("mkdir: missing operand\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            option = 1;
        }
        else if(strcmp(cmd, "--help") == 0){
            printf("Usage: mkdir [OPTION]... DIRECTORY...\n");
            printf("Create the DIRECTORY(ies), if they do not already exists.\n\n");
            
            printf("Mandatory arguments to long options are mandatory for short options too.\n");
            printf("    -m, --mode=MODE\t set file mode (as in chmod)\n");
            printf("    -p, --parents  \t no error if existing, make parent directories as needed\n");
            printf("        --help\t display this help and exit\n\n");
            
            printf("GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n");
            printf("Report any translation bugs to <https://translationproject.org/team/>\n");
            printf("Full documentation <https://www.gnu.org/software/coreutils/mkdir>\n");
            printf("or available locally via: info '(coreutils) mkdir invocation'\n");
            return -1;
        }
        else{
            printf("mkdir: unrecognized option '%s'\n",cmd);
            printf("Try 'mkdir --help' for more information.\n");
            return -1;
        }
    }
    else{ //옵션 없을 경우
        str = strtok(NULL, " ");
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = cmd;
    }
    while(str != NULL){
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = str;
        str = strtok(NULL, " ");
    }
    for (int i = 0; i < thread_cnt; i++) {
        pthread_create(&threadArr[i], NULL, mkdir_thread, (void *)&threadTree[i]);
        pthread_join(threadArr[i], NULL);
    }
    return 0;
}

int MakeDir(DirectoryTree* dirTree, char* dirName, char type)
{
    DirectoryNode* NewNode = (DirectoryNode*)malloc(sizeof(DirectoryNode));
    DirectoryNode* tmpNode = NULL;

    if(HasPermission(dirTree->current, 'w') != 0){
        printf("mkdir: %s: Permission denied\n", dirName);
        free(NewNode);
        return -1;
    }
    if(strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0){
        printf("mkdir: cannot create directory '%s': File exists\n", dirName);
        free(NewNode);
        return -1;
    }
    tmpNode = IsExistDir(dirTree, dirName, type);
    if(tmpNode != NULL && tmpNode->type == 'd'){
        printf("mkdir: cannot create directory '%s': File exists\n", dirName);
        free(NewNode);
        return -1;
    }
    time(&ltime);
    today = localtime(&ltime);

    NewNode->LeftChild = NULL;
    NewNode->RightSibling = NULL;

    strncpy(NewNode->name, dirName, MAX_NAME);
    if(dirName[0] == '.'){
	    NewNode->type = 'd';
        NewNode->mode = 700;
        NewNode->SIZE = 4096;
    }
    else if(type == 'd'){
        NewNode->type = 'd';
        NewNode->mode = 755;
        NewNode->SIZE = 4096;
    }
    else{
        NewNode->type = 'f';
        NewNode->mode = 644;
        NewNode->SIZE = 0;
    }
    Atoi_permission(NewNode);
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

void *mkdir_thread(void *arg) {
    ThreadTree *threadTree = ((ThreadTree *)arg);
    DirectoryTree *dirTree = threadTree->threadTree;
    char *cmd = threadTree->cmd;

    DirectoryNode *tmpNode = dirTree->current;
    char tmp[MAX_DIR];
    char str[MAX_DIR];
    char tmpstr[MAX_DIR];
    char name[MAX_DIR];
    int namelen = 0;
    int val;

    strncpy(tmp, cmd, MAX_DIR);

    if (strstr(cmd, "/") == NULL) {
        MakeDir(dirTree, cmd, 'd');
    } else if (threadTree->option == 1) {
        int tmplen = strlen(tmp), i = 0;
        if (tmp[0] == '/') {
            dirTree->current = dirTree->root;
            i = 1;
        }
        if (tmp[tmplen - 1] == '/') {
            tmplen -= 1;
        }
        for (; i < tmplen; i++) {
            str[i] = tmp[i];
            str[i + 1] = 0;
            name[namelen++] = tmp[i];
            if (tmp[i] == '/') {
                name[--namelen] = 0;
                strncpy(tmpstr, str, i - 1);
                val = Movecurrent(dirTree, name);
                if (val == -1) {
                    MakeDir(dirTree, name, 'd');
                    val = Movecurrent(dirTree, name);
                }
                namelen = 0;
            }
        }
        name[namelen] = 0;
        MakeDir(dirTree, name, 'd');
        dirTree->current = tmpNode;
    } else {
        char *p_get_directory = getDir(cmd);
        val = MovePath(dirTree, p_get_directory);
        if (val != 0) {
            printf("mkdir: '%s': No such file or directory.\n", p_get_directory);
        } else {
            char *str = strtok(tmp, "/");
            char *p_directory_name;
            while (str != NULL) {
                p_directory_name = str;
                str = strtok(NULL, "/");
            }
            MakeDir(dirTree, p_directory_name, 'd');
            dirTree->current = tmpNode;
        }
    }
    pthread_exit(NULL);
}