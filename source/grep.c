#include "../include/main.h"
int cmdcnt = 0;
int grep(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* currentNode = NULL;
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    pthread_t threadArr[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];

    int thread_cnt = 0;
    char* str;
    char con[MAX_BUFFER];
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    int val, option = 0;

    if (cmd == NULL) {      //grep 뒤에 아무것도 안적었을 경우
        printf("Usage: grep [OPTION]... PATTERNS [FILE]\n");
        printf("Try 'grep --help' for more information.\n");
        return -1;
    }
    currentNode = dirTree->current;
    if (cmd[0] == '-') {    //옵션이 입력되었을 경우
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
        else {      //나머지 옵션들 에러처리
            printf("grep: inrecognized option '%s'\n", cmd);
            printf("Usage: grep [OPTION]... PATTERNS [FILE]\n");
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
        str = strtok(NULL, " ");
        if (str == NULL) {      //옵션 뒤에 찾을 내용이 없을 경우
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
        strncpy(con, str, MAX_BUFFER);
        str = strtok(NULL, " ");
        if (str == NULL) {      //옵션 뒤에 명령할 파일이 없을 경우
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
    }
    else {      //옵션이 없을 경우
        strncpy(con, cmd, MAX_BUFFER);
        str = strtok(NULL, " ");
        if (str == NULL) {      //명령할 파일이 입력되지 않을 경우
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
    }
    while (str != NULL) {       //멀티스레드 작업을 위해 파일이름마다 스레드배열안에 정보를 저장
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt].content = con;
        threadTree[thread_cnt++].cmd = str;
        str = strtok(NULL, " ");
        cmdcnt++;
    }
    for (int i = 0; i < thread_cnt; i++) {       //pthread생성 후 cat_thread로 처리, 마지막으로 join
        pthread_create(&threadArr[i], NULL, grep_thread, (void*)&threadTree[i]);
        pthread_join(threadArr[i], NULL);
    }
    cmdcnt = 0;
    return 1;
}

int grep_print(DirectoryTree* dirTree, char* search, char* fName, int o)
{
    FILE* fp;
    char buf[MAX_BUFFER];
    int cnt = 1;

    fp = fopen(fName, "r");
    while (feof(fp) == 0) {
        fgets(buf, sizeof(buf), fp);
        if (feof(fp) != 0) {
            break;
        }   //옵션에 따라 프린트
        else if (o == 0)    //옵션 x
        {
            if (strstr(buf, search) != NULL)
            {
                if (cmdcnt > 1)
                    printf("%s:", fName);
                printf("%s", buf);
            }
        }
        else if (o == 1)    //n 옵션
        {
            if (strstr(buf, search) != NULL)
            {
                if (cmdcnt > 1)
                    printf("%s:", fName);
                printf("%d:%s", cnt, buf);
            }
        }
        else if (o == 2)    //v 옵션
        {
            if (strstr(buf, search) == NULL)
            {
                if (cmdcnt > 1)
                    printf("%s:", fName);
                printf("%s", buf);
            }
        }
        else if (o == 3)    //i 옵션
        {
            if (strcasestr(buf, search) != NULL)
            {
                if (cmdcnt > 1)
                    printf("%s:", fName);
                printf("%s", buf);
            }
        }
        else if (o == 4)    //nv, vn 옵션
        {
            if (strstr(buf, search) == NULL)
            {
                if (cmdcnt > 1)
                    printf("%s:", fName);
                printf("%d:%s", cnt, buf);
            }
        }
        else if (o == 5)    // ni, in 옵션
        {
            if (strcasestr(buf, search) != NULL)
            {
                if (cmdcnt > 1)
                    printf("%s:", fName);
                printf("%d:%s", cnt, buf);
            }
        }
        else if (o == 6)    // vi, iv 옵션
        {
            if (strcasestr(buf, search) == NULL)
            {
                if (cmdcnt > 1)
                    printf("%s:", fName);
                printf("%s", buf);
            }
        }
        else if (o == 7) {      // nvi, niv, vin, vni, ivn, inv 옵션
            if (strcasestr(buf, search) == NULL)
            {
                if (cmdcnt > 1)
                    printf("%s:", fName);
                printf("%d:%s", cnt, buf);
            }
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

    if (strstr(tmp, "/") == NULL) {     //현재 디렉토리 안에 있는 파일 또는 디렉토리일 경우
        tmpNode = IsExistDir(dirTree, cmd, 'd');
        tmpNode2 = IsExistDir(dirTree, cmd, 'f');
        if (tmpNode == NULL && tmpNode2 == NULL) {      //파일 또는 디렉토리가 없을 경우
            printf("grep: '%s': No such file or directory.\n", cmd);
            return NULL;
        }
        else if (tmpNode != NULL && tmpNode2 == NULL) {     //디렉토리일 경우
            printf("grep: '%s': Is a directory\n", cmd);
            return NULL;
        }
        else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {       //허가권한이 거부될 경우
            printf("grep: Can not open file '%s': Permission denied\n", tmpNode2->name);
            return NULL;
        }
        else
            grep_print(dirTree, con, cmd, option);
    }
    else {      //다른 디렉토리의 파일이나 디렉토리일 경우
        strncpy(tmp2, getDir(tmp), MAX_DIR);
        val = MovePath(dirTree, tmp2);
        if (val) {
            printf("grep: '%s': No such file or directory.\n", tmp2);
            return NULL;
        }
        str = strtok(cmd, "/");
        while (str != NULL) {
            strncpy(tmp3, str, MAX_NAME);
            str = strtok(NULL, "/");
        }
        tmpNode = IsExistDir(dirTree, tmp3, 'd');
        tmpNode2 = IsExistDir(dirTree, tmp3, 'f');
        if (tmpNode == NULL && tmpNode2 == NULL) {      //파일 또는 디렉토리가 존재하지 않을 경우
            printf("grep: '%s': No such file or directory.\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        }
        else if (tmpNode != NULL && tmpNode2 == NULL) {     //디렉토리일 경우
            printf("grep: '%s': Is a directory\n", tmp3);
            dirTree->current = currentNode;
            return NULL;
        }
        else if (tmpNode2 != NULL && HasPermission(tmpNode2, 'r') != 0) {       //허가권한이 거부될 경우
            printf("grep: Can not open file '%s': Permission denied\n", tmpNode2->name);
            dirTree->current = currentNode;
            return NULL;
        }
        else
            grep_print(dirTree, con, tmp3, option);
        dirTree->current = currentNode;
    }
    pthread_exit(NULL);
}