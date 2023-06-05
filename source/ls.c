#include "../include/main.h"
int ls_cnt = 1;
int ls(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* tmpNode = NULL;
    pthread_t threadArr[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];
    char* command;
    int option = 0;
    int thread_cnt = 0;

    if (cmd == NULL) {
        ListDir(dirTree, 0);
        return 0;
    }
    if (cmd[0] == '-') {    //옵션이 있을 경우
        if (strcmp(cmd, "-l") == 0) {      //옵션이 -l일 경우
            option = 2;
            command = strtok(NULL, " ");
            if (command == NULL) {
                ListDir(dirTree, 2);
            }
        }
        else if (strcmp(cmd, "-a") == 0) {      //옵션이 -a일 경우
            option = 1;
            command = strtok(NULL, " ");
            if (command == NULL) {
                ListDir(dirTree, 1);
            }
        }
        else if (strcmp(cmd, "-al") == 0 || strcmp(cmd, "-la") == 0) {       //옵션이 -al, -la일 경우
            option = 3;
            command = strtok(NULL, " ");
            if (command == NULL) {
                ListDir(dirTree, 3);
            }
        }
        else if (strcmp(cmd, "--help") == 0) {
            printf("Usage: ls [OPTION]... [FILE]...\n");
            printf("List information about the FILEs (the current directory by default).\n");
            printf("Sort entries alphabetically if none of -cftuvSUX nor --sort is specified.\n\n");
            
            printf("Mandatory arguments to long [+options are mandatory for short options too.\n");
            printf("  -a, --all\t do not ignore entries starting with .\n");
            printf("  -l       \t use a long listing format\n");
            printf("      --help\t display this help and exit\n\n");
            
            printf("The SIZE argument is an integer and optional unit (example: 10K is 10*1024).\n");
            printf("Units are K,M,G,T,P,E,Z,Y (powers of 1024) or KB,MB,... (powers of 1000).\n");
            printf("Binary prefixes can be used, too: Kib=K, MiB=M, and so on.\n\n");
            
            printf("The TIME_STYLE argument can be full-iso, long-isp, iso, locale, or +FORMAT.\n");
            printf("FORMAT is interpreted like in date(1). If FORMAT is FORMAT1<newline>FORMAT2,\n");
            printf("then FORMAT1 applies to non-recent files and FORNAT2 to recent files.\n");
            printf("TIME_STYLE prefixed with 'posix-' takes effect only outside the POSIX locale.\n");
            printf("Also the TIME_STYLE environment variable sets the default style to use.\n\n");
            
            printf("Using color to distinguish file types is diasbled both by default and\n");
            printf("with --color=never. With --color=autp, ls emits color codes only when\n");
            printf("standard output is connected to a terminal. The LS_COLORS environmnet\n");
            printf("variable can change the settings. User the dircolors command to set it\n\n");
            
            printf("Exit status:\n");
            printf(" 0 if OK,\n");
            printf(" 1 if minor problems (e.g., cannot access subdirectory),\n");
            printf(" 2 if serious trouble (e.g., cannot access command-line argument).\n\n");
            
            printf("GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n");
            printf("Report any translation bugs to <https://translationproject.org/team/>\n");
            printf("Full documentation <https://www.gnu.org/software/coreutils/mkdir>\n");
            printf("or available locally via: info '(coreutils) mkdir invocation'\n");
            return -1;
        }
        else {      //다른 옵션일 경우 에러
            command = strtok(cmd, "-");
            printf("ls: invalid option -- '%s'\n", command);
            printf("Try 'ls --help' for more information.\n");
            return -1;
        }
    }
    else {
        command = strtok(NULL, " ");
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = cmd;
    }
    while(command != NULL){
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = command;
        command = strtok(NULL, " ");
        ls_cnt++;
    }
    for (int i = 0; i < thread_cnt; i++) {
        pthread_create(&threadArr[i], NULL, ls_thread, (void *)&threadTree[i]);
        pthread_join(threadArr[i], NULL);
    }
    ls_cnt = 1;
    return 0;
}

void* ls_thread(void *arg) {
    ThreadTree *threadTree = (ThreadTree *)arg;
    DirectoryTree *dirTree = threadTree->threadTree;
    char *cmd = threadTree->cmd;
    DirectoryNode *tmpNode = NULL;
    int option = threadTree->option;
    int check_exist;

    tmpNode = dirTree->current;
    if (ls_cnt > 1)
        printf("%s:\n", cmd);
    check_exist = MovePath(dirTree, cmd);
    if (check_exist)
    {
        printf("ls: '%s': No such file or direcory.\n", cmd);
        return -1;
    }
    ListDir(dirTree, option);
    dirTree->current = tmpNode;
    pthread_exit(NULL);     //스레드 실행 끝
}

int ListDir(DirectoryTree* dirTree, int option)
{
    //variables
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    char type;
    int ls_cnt;

    tmpNode = dirTree->current->LeftChild;

    if (HasPermission(dirTree->current, 'r') != 0) {
        printf("ls: '%s: Permission denied", dirTree->current->name);
        return -1;
    }
    if (option == 0 || option == 1) {
        if (option == 0) {
            if (tmpNode == NULL)
                return -1;
        }
        if (option == 1) {
            BOLD; BLUE;
            printf(".       ");
            DEFAULT;
            if (dirTree->current != dirTree->root) {
                BOLD; BLUE;
                printf("..      ");
                DEFAULT;
            }
        }
        while (tmpNode != NULL) {
            if (option == 0) {
                if (strncmp(tmpNode->name, ".", 1) == 0) {
                    tmpNode = tmpNode->RightSibling;
                    continue;
                }
            }
            if (tmpNode->type == 'd') {
                BOLD; BLUE;
                printf("%s      ", tmpNode->name);
                DEFAULT;
            }
            else
                printf("%s      ", tmpNode->name);
            tmpNode = tmpNode->RightSibling;
        }
        if (ls_cnt > 1)
            printf("\n");
    }
    else {
        if (option == 2) {
            if (tmpNode == NULL) {
                printf("total: 0\n");
                return -1;
            }
        }
        if (option == 3) {
            tmpNode2 = dirTree->current->LeftChild;
            if (tmpNode2 == NULL) {
                ls_cnt = 2;
            }
            else {
                if (tmpNode2->type == 'd')
                    ls_cnt = 3;
                else
                    ls_cnt = 2;

                while (tmpNode2->RightSibling != NULL) {
                    tmpNode2 = tmpNode2->RightSibling;
                    if (tmpNode2->type == 'd')
                        ls_cnt = ls_cnt + 1;
                }
            }

            printf("%c", dirTree->current->type);
            PrintPermission(dirTree->current);
            printf("%3d", ls_cnt);
            printf("   ");
            printf("%-5s%-5s", GetUID(dirTree->current), GetGID(dirTree->current));
            printf("%5d ", dirTree->current->SIZE);
            GetMonth(dirTree->current->month);
            printf(" %2d %02d:%02d ", dirTree->current->day, dirTree->current->hour, dirTree->current->minute);
            BOLD; BLUE;
            printf(".\n");
            DEFAULT;

            if (dirTree->current != dirTree->root) {
                tmpNode2 = dirTree->current->Parent->LeftChild;
                if (tmpNode2 == NULL)
                    ls_cnt = 2;
                else {
                    if (tmpNode2->type == 'd')
                        ls_cnt = 3;
                    else
                        ls_cnt = 2;

                    while (tmpNode2->RightSibling != NULL) {
                        tmpNode2 = tmpNode2->RightSibling;
                        if (tmpNode2->type == 'd')
                            ls_cnt = ls_cnt + 1;
                    }
                }
                printf("%c", dirTree->current->Parent->type);
                PrintPermission(dirTree->current->Parent);
                printf("%3d", ls_cnt);
                printf("   ");
                printf("%-5s%-5s", GetUID(dirTree->current->Parent), GetGID(dirTree->current->Parent));
                printf("%5d ", dirTree->current->SIZE);
                GetMonth(dirTree->current->Parent->month);
                printf(" %2d %02d:%02d ", dirTree->current->Parent->day, dirTree->current->Parent->hour, dirTree->current->Parent->minute);
                BOLD; BLUE;
                printf("..\n");
                DEFAULT;
            }
        }
        while (tmpNode != NULL) {
            if (option == 2) {
                if (strncmp(tmpNode->name, ".", 1) == 0) {
                    tmpNode = tmpNode->RightSibling;
                    continue;
                }
            }
            tmpNode2 = tmpNode->LeftChild;
            if (tmpNode2 == NULL) {
                if (tmpNode->type == 'd')
                    ls_cnt = 2;
                else
                    ls_cnt = 1;
            }
            else {
                if (tmpNode2->type == 'd')
                    ls_cnt = 3;
                else
                    ls_cnt = 2;

                while (tmpNode2->RightSibling != NULL) {
                    tmpNode2 = tmpNode2->RightSibling;
                    if (tmpNode2->type == 'd')
                        ls_cnt = ls_cnt + 1;
                }
            }
            if (tmpNode->type == 'd')
                type = 'd';
            else if (tmpNode->type == 'f')
                type = '-';
            printf("%c", type);
            PrintPermission(tmpNode);
            printf("%3d", ls_cnt);
            printf("   ");
            printf("%-9s%-9s", GetUID(tmpNode), GetGID(tmpNode));
            printf("%5d ", tmpNode->SIZE);
            GetMonth(tmpNode->month);
            printf(" %2d %02d:%02d ", tmpNode->day, tmpNode->hour, tmpNode->minute);

            if (tmpNode->type == 'd') {
                BOLD; BLUE;
                printf("%-15s\n", tmpNode->name);
                DEFAULT;
            }
            else
                printf("%-15s\n", tmpNode->name);
            tmpNode = tmpNode->RightSibling;
        }
    }
    printf("\n");
    return 0;
}