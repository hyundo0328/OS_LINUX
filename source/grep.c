#include "../include/main.h"

int grep(DirectoryTree* dirTree, char* cmd)
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
    int val, option = 0;

    if (cmd == NULL) {
        printf("Usage: grep [OPTION]... PATTERNS [FILE]\n");
        printf("Try 'grep --help' for more information.\n");
        return -1;
    }
    currentNode = dirTree->current;
    if (cmd[0] == '-') {
        if (strcmp(cmd, "-n") == 0)
            option = 1;
        else if (strcmp(cmd, "-v") == 0)
            option = 2;
        else if (strcmp(cmd, "-i") == 0)
            option = 3;
        else if (strcmp(cmd, "-nv") == 0 || strcmp(cmd, "-vn") == 0)
            option = 4;
        else if (strcmp(cmd, "-ni") == 0 || strcmp(cmd, "-in") == 0)
            option = 5;
        else if (strcmp(cmd, "-vi") == 0 || strcmp(cmd, "-iv") == 0)
            option = 6;
        else if (strcmp(cmd, "-inv") == 0 || strcmp(cmd, "-ivn") == 0 || strcmp(cmd, "-niv") == 0 || strcmp(cmd, "-nvi") == 0 || strcmp(cmd, "-vin") == 0 || strcmp(cmd, "-vni") == 0)
            option = 7;
        else if (strcmp(cmd, "--help") == 0) {
            printf("Usage: grep [OPTION]... PATTERNS [FILE]...\n");
            printf("Search for PATTERNS in each FILE.\n");
            printf("Example: grep -i 'hello world' menu.h main.c\n");
            printf("PATTERNS can contain multiple patterns separated by newlines.\n\n");

            printf("Pattern selection and interpretation:\n");
            printf("    -v, --invert-match\t select non-matching lines\n");
            printf("    -i, -ignore-case\t ignore case distinctions in patterns and data\n");
            printf("        --no-ignore-case do not ignore case distinctions (default)\n\n");

            printf("Miscellaneous:\n");
            printf("        --help\t display this help text and exit\n\n");

            printf("Output controls\n");
            printf("    -n, --line-number\t print line number with output lines\n");
            printf("        --line-buffered\t flush output on every line\n\n");
            
            printf("When FILE is '-', read standard input. With no FILE, read '.' if\n");
            printf("recursive, '-' otherwise. With fewer than two FILEs, assume -h.\n");
            printf("Exit status is 0 if any line is selected, 1 otherwise;\n");
            printf("if any error occurs and -q is not gibven, the exit status is 2.\n\n");

            printf("Report bugs to: bug-grep@gnu.org\n");
            printf("GNU grep home page: <https://www.gnu.org/software/grep/>\n");
            printf("General help using GNU software: <https://www.gnu.org/gethelp/>\n");
            return -1;
        }
        else {
            printf("grep: inrecognized option '%s'\n", cmd);
            printf("Usage: grep [OPTION]... PATTERNS [FILE]\n");
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
        str = strtok(NULL, " ");
        strncpy(con, str, MAX_BUFFER);
        str = strtok(NULL, " ");
        if (str == NULL) {
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
    }
    else {
        strncpy(con, cmd, MAX_BUFFER);
        str = strtok(NULL, " ");
        if (str == NULL) {
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
    }
    while (str != NULL) {
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt].content = con;
        threadTree[thread_cnt++].cmd = str;
        str = strtok(NULL, " ");
    }
    for (int i = 0; i < thread_cnt; i++) {
        pthread_create(&threadPool[i], NULL, grep_thread, (void*)&threadTree[i]);
        pthread_join(threadPool[i], NULL);
    }
    return 1;
}

int grep_print(DirectoryTree* dirTree, char* search, char* fName, int o)
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
    tmpNode = IsExistDir(dirTree, fName, 'f');

    if (tmpNode == NULL) {
        return -1;
    }
    fp = fopen(fName, "r");

    while (feof(fp) == 0) {
        fgets(buf, sizeof(buf), fp);
        if (feof(fp) != 0) {
            break;
        }
        else if (o == 0)
        {
            if (strstr(buf, search) != NULL)
                printf("%s:%s", fName, buf);
        }
        else if (o == 1)
        {
            if (strstr(buf, search) != NULL)
                printf("%s:%d:%s", fName, cnt, buf);
        }
        else if (o == 2)
        {
            if (strstr(buf, search) == NULL)
                printf("%s:%s", fName, buf);
        }
        else if (o == 3)
        {
            if (strcasestr(buf, search) != NULL)
                printf("%s:%s", fName, buf);
        }
        else if (o == 4)
        {
            if (strstr(buf, search) == NULL)
                printf("%s:%d:%s", fName, cnt, buf);
        }
        else if (o == 5)
        {
            if (strcasestr(buf, search) != NULL)
                printf("%s:%d:%s", fName, cnt, buf);
        }
        else if (o == 6)
        {
            if (strcasestr(buf, search) == NULL)
                printf("%s:%s", fName, buf);
        }
        else if (o == 7) {
            if (strcasestr(buf, search) == NULL)
                printf("%s:%d:%s", fName, cnt, buf);
        }
        cnt++;
    }
    fclose(fp);
    return 0;
}

void* grep_thread(void* arg) {
    ThreadTree* threadTree = (ThreadTree*)arg;
    DirectoryTree* dirTree = threadTree->threadTree;
    char* cmd = threadTree->cmd;
    char* con = threadTree->content;
    DirectoryNode* currentNode = dirTree->current;
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    char* str;
    int option = threadTree->option;
    int val;

    strncpy(tmp, cmd, MAX_DIR);

    if (strstr(tmp, "/") == NULL) {
        tmpNode = IsExistDir(dirTree, cmd, 'd');
        tmpNode2 = IsExistDir(dirTree, cmd, 'f');
        if (tmpNode == NULL && tmpNode2 == NULL) {
            printf("grep: '%s': No such file or directory.\n", cmd);
            return NULL;
        }
        else if (tmpNode != NULL && tmpNode2 == NULL) {
            printf("grep: '%s': Is a directory\n", cmd);
            return NULL;
        }
        else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
            printf("grep: Can not open file '%s': Permission denied\n", tmpNode2->name);
            return NULL;
        }
        else
            grep_print(dirTree, con, cmd, option);
    }
    else {
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
        if (tmpNode == NULL && tmpNode2 == NULL) {
            printf("grep: '%s': No such file or directory.\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        }
        else if (tmpNode != NULL && tmpNode2 == NULL) {
            printf("grep: '%s': Is a directory\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        }
        else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {
            printf("grep: Can not open file '%s': Permission denied\n", tmpNode2->name);
            dirTree->current = currentNode;
            return NULL;
        }
        else
            grep_print(dirTree, con, cmd, option);
        dirTree->current = currentNode;
    }
    pthread_exit(NULL);
}