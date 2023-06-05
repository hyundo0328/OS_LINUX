#include "../include/main.h"

int mkdir(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* tmpNode = NULL;
    pthread_t threadArr[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];
    char* command;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int thread_cnt = 0;
    int option = 0;
    int tmpMode;

    tmpNode = dirTree->current;
    if (cmd[0] == '-') { //옵션 있을 경우
        if (strcmp(cmd, "-p") == 0) {
            command = strtok(NULL, " ");
            if (command == NULL) {
                printf("mkdir: missing operand\n");
                printf("Try 'mkdir --help' for more information.\n");
                return -1;
            }
            option = 1;
        }
        else if (strcmp(cmd, "--help") == 0) {
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
        else {
            printf("mkdir: invalid option '%s'\n", cmd);
            printf("Try 'mkdir --help' for more information.\n");
            return -1;
        }
    }
    else { //옵션 없을 경우
        command = strtok(NULL, " ");
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = cmd;
    }
    while (command != NULL) {
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = command;
        command = strtok(NULL, " ");
    }
    for (int i = 0; i < thread_cnt; i++) {
        pthread_create(&threadArr[i], NULL, mkdir_thread, (void*)&threadTree[i]);
        pthread_join(threadArr[i], NULL);
    }
    return 0;
}

int MakeDir(DirectoryTree* dirTree, char* dirName, char type)
{
    DirectoryNode* newNode = (DirectoryNode*)malloc(sizeof(DirectoryNode));
    DirectoryNode* tmpNode = NULL;

    if (HasPermission(dirTree->current, 'w') != 0) {
        printf("mkdir: %s: Permission denied\n", dirName);
        free(newNode);
        return -1;
    }
    if (strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0) {
        printf("mkdir: cannot create directory '%s': File exists\n", dirName);
        free(newNode);
        return -1;
    }
    tmpNode = IsExistDir(dirTree, dirName, type);
    if (tmpNode != NULL && tmpNode->type == 'd') {
        printf("mkdir: cannot create directory '%s': File exists\n", dirName);
        free(newNode);
        return -1;
    }
    time(&ltime);
    today = localtime(&ltime);

    newNode->LeftChild = NULL;
    newNode->RightSibling = NULL;

    strncpy(newNode->name, dirName, MAX_NAME);
    if (dirName[0] == '.') {
        newNode->type = 'd';
        newNode->mode = 700;
        newNode->SIZE = 4096;
    }
    else if (type == 'd') {
        newNode->type = 'd';
        newNode->mode = 755;
        newNode->SIZE = 4096;
    }
    else {
        newNode->type = 'f';
        newNode->mode = 644;
        newNode->SIZE = 0;
    }
    Atoi_permission(newNode);
    newNode->UID = usrList->current->UID;
    newNode->GID = usrList->current->GID;
    newNode->month = today->tm_mon + 1;
    newNode->day = today->tm_mday;
    newNode->hour = today->tm_hour;
    newNode->minute = today->tm_min;
    newNode->Parent = dirTree->current;

    if (dirTree->current->LeftChild == NULL) {
        dirTree->current->LeftChild = newNode;
    }
    else {
        tmpNode = dirTree->current->LeftChild;

        while (tmpNode->RightSibling != NULL) {
            tmpNode = tmpNode->RightSibling;
        }
        tmpNode->RightSibling = newNode;
    }

    return 0;
}

void* mkdir_thread(void* arg) {
    ThreadTree* threadTree = ((ThreadTree*)arg);
    DirectoryTree* dirTree = threadTree->threadTree;
    char* command = threadTree->cmd;

    DirectoryNode* tmpNode = dirTree->current;
    char tmp[MAX_DIR];
    char str[MAX_DIR];
    char tmpstr[MAX_DIR];
    char name[MAX_DIR];
    int nameLength = 0;
    int check_exist;

    strncpy(tmp, command, MAX_DIR);

    if (strstr(command, "/") == NULL) {
        MakeDir(dirTree, command, 'd');
    }
    else if (threadTree->option == 1) {
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
            name[nameLength++] = tmp[i];
            if (tmp[i] == '/') {
                name[--nameLength] = 0;
                strncpy(tmpstr, str, i - 1);
                check_exist = Movecurrent(dirTree, name);
                if (check_exist == -1) {
                    MakeDir(dirTree, name, 'd');
                    check_exist = Movecurrent(dirTree, name);
                }
                nameLength = 0;
            }
        }
        name[nameLength] = 0;
        MakeDir(dirTree, name, 'd');
        dirTree->current = tmpNode;
    }
    else {
        char* p_get_directory = getDir(command);
        check_exist = MovePath(dirTree, p_get_directory);
        if (check_exist != 0) {
            printf("mkdir: '%s': No such file or directory.\n", p_get_directory);
        }
        else {
            char* command = strtok(tmp, "/");
            char* p_directory_name;
            while (command != NULL) {
                p_directory_name = command;
                command = strtok(NULL, "/");
            }
            MakeDir(dirTree, p_directory_name, 'd');
            dirTree->current = tmpNode;
        }
    }
    pthread_exit(NULL);
}