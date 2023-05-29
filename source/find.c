#include "../include/main.h"

int find(DirectoryTree* dirTree, char* cmd)
{
    char* str;
    if(cmd == NULL){
        FindDir(dirTree, dirTree->current->name, 1);
        return 0;
    }
    else if(cmd[0] == '-'){
        if(strcmp(cmd, "-name") == 0){
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("find: missing argument to '-name'\n");
                return -1;
            }
            FindDir(dirTree, str, 0);
        }
        else if(strcmp(cmd, "--help") == 0){
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
            printf("Try 'find --help' for more information.\n");
            return -1;
        }
    }
    else{
        FindDir(dirTree, cmd, 1);
    }

    return 0;
}

int ReadDir(DirectoryTree* dirTree, char* tmp, char* dirName, int o)
{
    char* str;
    char str2[MAX_NAME];
    if(o == 0){
        str = strtok(tmp, " ");
        strcpy(str2, str);
        for(int i=0;i<10;i++){
            str = strtok(NULL, " ");
        }
        if(str != NULL){
            if(strstr(str2, dirName) != NULL){
                str[strlen(str)-1] = '\0';
                if(strcmp(str, "/") == 0)
                    printf("/%s\n", str2);
                else
                    printf("%s/%s\n", str, str2);
            }
        }
    }
    else{
        str = strtok(tmp, " ");
        strcpy(str2, str);
        for(int i=0;i<10;i++){
            str = strtok(NULL, " ");
        }
        if(str != NULL){
            if(strstr(str, dirName) != NULL){
                str[strlen(str)-1] = '\0';
                if(strcmp(str, "/") == 0)
                    printf("/%s\n", str2);
                else
                    printf("%s/%s\n", str, str2);
            }
        }
    }
    return 0;
}

void FindDir(DirectoryTree* dirTree, char* dirName, int o)
{
    char tmp[MAX_LENGTH];

    Dir = fopen("Directory.txt", "r");

    while(fgets(tmp, MAX_LENGTH, Dir) != NULL){
        ReadDir(dirTree, tmp, dirName, o);
    }

    fclose(Dir);
}