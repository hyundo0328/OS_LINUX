#include "../include/main.h"

int ls(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* tmpNode = NULL;
    char* str;
    int val;

    if (cmd == NULL) {
        ListDir(dirTree, 0, 0);
        return 0;
    }

    if (cmd[0] == '-') {
        if (strcmp(cmd, "-al") == 0 || strcmp(cmd, "-la") == 0) {
            str = strtok(NULL, " ");
            if (str != NULL) {
                tmpNode = dirTree->current;
                val = MovePath(dirTree, str);
                if (val != 0)
                    return -1;
            }
            ListDir(dirTree, 1, 1);
        }
        else if (strcmp(cmd, "-l") == 0) {
            str = strtok(NULL, " ");
            if (str != NULL) {
                tmpNode = dirTree->current;
                val = MovePath(dirTree, str);
                if (val != 0)
                    return -1;
            }
            ListDir(dirTree, 0, 1);
        }
        else if (strcmp(cmd, "-a") == 0) {
            str = strtok(NULL, " ");
            if (str != NULL) {
                tmpNode = dirTree->current;
                val = MovePath(dirTree, str);
                if (val != 0)
                    return -1;
            }
            ListDir(dirTree, 1, 0);
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
        else {
            str = strtok(cmd, "-");
            if (str == NULL) {
                printf("ls: invalid option -- '%s'\n", cmd);
                printf("Try 'ls --help' for more information.\n");
                return -1;
            }
            else {
                printf("ls: invalid option -- '%s'\n", cmd);
                printf("Try 'ls --help' for more information.\n");
                return -1;
            }
        }
    }
    else {
        tmpNode = dirTree->current;
        val = MovePath(dirTree, cmd);
        if (val != 0)
            return -1;
        ListDir(dirTree, 0, 0);
        dirTree->current = tmpNode;
    }

    if (str != NULL)
        dirTree->current = tmpNode;

    return 0;
}

int ListDir(DirectoryTree* dirTree, int a, int l)
{
    //variables
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    char type;
    int cnt;

    tmpNode = dirTree->current->LeftChild;

    if (HasPermission(dirTree->current, 'r') != 0) {
        printf("ls: '%s: Permission denied", dirTree->current->name);
        return -1;
    }

    if (l == 0) {
        if (a == 0) {
            if (tmpNode == NULL) {
                return -1;
            }
        }
        if (a == 1) {
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
            if (a == 0) {
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
        printf("\n");
    }
    else {
        if (a == 0) {
            if (tmpNode == NULL) {
                printf("total: 0\n");
                return -1;
            }
        }
        if (a == 1) {
            tmpNode2 = dirTree->current->LeftChild;
            if (tmpNode2 == NULL) {
                cnt = 2;
            }
            else {
                if (tmpNode2->type == 'd')
                    cnt = 3;
                else
                    cnt = 2;

                while (tmpNode2->RightSibling != NULL) {
                    tmpNode2 = tmpNode2->RightSibling;
                    if (tmpNode2->type == 'd')
                        cnt = cnt + 1;
                }
            }

            printf("%c", dirTree->current->type);
            PrintPermission(dirTree->current);
            printf("%3d", cnt);
            printf("   ");
            printf("%-5s%-5s", GetUID(dirTree->current), GetGID(dirTree->current));
            printf("%5d ", dirTree->current->SIZE);
            printf("%d월 %2d %02d:%02d ", dirTree->current->month, dirTree->current->day, dirTree->current->hour, dirTree->current->minute);
            BOLD; BLUE;
            printf(".\n");
            DEFAULT;

            if (dirTree->current != dirTree->root) {
                tmpNode2 = dirTree->current->Parent->LeftChild;
                if (tmpNode2 == NULL)
                    cnt = 2;
                else {
                    if (tmpNode2->type == 'd')
                        cnt = 3;
                    else
                        cnt = 2;

                    while (tmpNode2->RightSibling != NULL) {
                        tmpNode2 = tmpNode2->RightSibling;
                        if (tmpNode2->type == 'd')
                            cnt = cnt + 1;
                    }
                }
                printf("%c", dirTree->current->Parent->type);
                PrintPermission(dirTree->current->Parent);
                printf("%3d", cnt);
                printf("   ");
                printf("%-5s%-5s", GetUID(dirTree->current->Parent), GetGID(dirTree->current->Parent));
                printf("%5d ", dirTree->current->SIZE);
                printf("%d월 %2d %02d:%02d ", dirTree->current->Parent->month, dirTree->current->Parent->day, dirTree->current->Parent->hour, dirTree->current->Parent->minute);
                BOLD; BLUE;
                printf("..\n");
                DEFAULT;
            }
        }

        while (tmpNode != NULL) {
            if (a == 0) {
                if (strncmp(tmpNode->name, ".", 1) == 0) {
                    tmpNode = tmpNode->RightSibling;
                    continue;
                }
            }
            tmpNode2 = tmpNode->LeftChild;
            if (tmpNode2 == NULL) {
                if (tmpNode->type == 'd')
                    cnt = 2;
                else
                    cnt = 1;
            }
            else {
                if (tmpNode2->type == 'd')
                    cnt = 3;
                else
                    cnt = 2;

                while (tmpNode2->RightSibling != NULL) {
                    tmpNode2 = tmpNode2->RightSibling;
                    if (tmpNode2->type == 'd')
                        cnt = cnt + 1;
                }
            }
            if (tmpNode->type == 'd')
                type = 'd';
            else if (tmpNode->type == 'f')
                type = '-';
            printf("%c", type);
            PrintPermission(tmpNode);
            printf("%3d", cnt);
            printf("   ");
            printf("%-5s%-5s", GetUID(tmpNode), GetGID(tmpNode));
            printf("%5d ", tmpNode->SIZE);
            printf("%d월 %2d %02d:%02d ", tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->minute);

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
    return 0;
}
