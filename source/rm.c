#include "../include/main.h"

int rm(DirectoryTree* dirTree, char* cmd)
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
    int val, option = 0;

    if(cmd == NULL){
        printf("rm: missing operand\n");
        printf("Try 'rm --help' for more information.\n");
        return -1;
    }
    currentNode = dirTree->current;
    if(cmd[0] == '-'){
        if(strcmp(cmd, "-r") == 0){
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("rm: missing operand\n");
                printf("Try 'rm --help' for more information.\n");
                return -1;
            }
            option = 1;
        }
        else if(strcmp(cmd, "-f") == 0){
            str = strtok(NULL, " ");
            if(str == NULL){
                return -1;
            }
            option = 2;
        }
        else if(strcmp(cmd, "-rf") == 0 || strcmp(cmd, "-fr") == 0){
            str = strtok(NULL, " ");
            if(str == NULL){
                return -1;
            }
            option = 3;
        }
        else if(strcmp(cmd, "--help") == 0){
            printf("Usage: rm [OPTION]... [FILE]...\n");
            printf("Remove (unlink) the FILE(s).\n\n");
            
            printf("    -f, --force    \t ignore nonexistent files and arguments, never prompt\n");
            printf("    -r, -R, --recursive\t remove directories and their contents recursively\n");
            printf("        --help\t display this help and exit\n\n");
            
            printf("By default, rm does not remove directories. Use the --recursive (-r or -R)\n");
            printf("option to remove each listed directory, too, along with all of its contents.\n");
            printf("Note that if you use rm to remove a file, it might be possible to recover\n");
            printf("some of its contents are truly unrecoverable, consider using shred.\n\n");
            
            printf("GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n");
            printf("Report any translation bugs to <https://translationproject.org/team/>\n");
            printf("Full documentation <https://www.gnu.org/software/coreutils/mkdir>\n");
            printf("or available locally via: info '(coreutils) mkdir invocation'\n");

            return -1;
        }
        else{
            str = strtok(cmd, "-");
            if(str == NULL){
                printf("rm: cannot remove '-': No such file or directory\n");
                return -1;
            }
            else{
                printf("rm: invalid option -- '%s'\n", str);
                printf("Try 'rm —help' for more information.\n");
                return -1;
            }
        }
    }
    else{
        str = strtok(NULL, " ");
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = cmd;
    }
    while (str != NULL) {
        threadTree[thread_cnt].threadTree = dirTree;
        threadTree[thread_cnt].option = option;
        threadTree[thread_cnt++].cmd = str;
        str = strtok(NULL, " ");
    }
    for (int i = 0; i < thread_cnt; i++) {
        pthread_create(&threadArr[i], NULL, rm_thread, (void *)&threadTree[i]);
        pthread_join(threadArr[i], NULL);
    }
    return 0;
}

int RemoveDir(DirectoryTree* dirTree, char* dirName)
{
    DirectoryNode* DelNode = NULL;
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* prevNode = NULL;

    tmpNode = dirTree->current->LeftChild;

    if(tmpNode == NULL){
        printf("rm: cannot remove %s: No such file or directory\n", dirName);
        return -1;
    }

    if(strcmp(tmpNode->name, dirName) == 0){
        dirTree->current->LeftChild = tmpNode->RightSibling;
        DelNode = tmpNode;
        if(DelNode->LeftChild != NULL)
            DestroyDir(DelNode->LeftChild);
        remove(DelNode->name);
        DestroyNode(DelNode);
    }
    else{
        while(tmpNode != NULL){
            if(strcmp(tmpNode->name, dirName) == 0){
                DelNode = tmpNode;
                break;
            }
            prevNode = tmpNode;
            tmpNode = tmpNode->RightSibling;
        }
        if(DelNode != NULL){
            prevNode->RightSibling = DelNode->RightSibling;

            if(DelNode->LeftChild != NULL)
                DestroyDir(DelNode->LeftChild);
            remove(DelNode->name);
            DestroyNode(DelNode);
        }
        else{
            printf("rm: cannot remove %s: No such file or directory\n", dirName);
            return -1;
        }
    }
    return 0;
}

void *rm_thread(void *arg) {
    ThreadTree *threadTree = (ThreadTree *)arg;
    DirectoryTree *dirTree = threadTree->threadTree;

    DirectoryNode *tmpNode = NULL;
    DirectoryNode *tmpNode2 = NULL;

    char *cmd = threadTree->cmd;
    char tmp[MAX_DIR];
    char tmp2[MAX_DIR];
    char tmp3[MAX_DIR];
    DirectoryNode *currentNode = dirTree->current;

    strncpy(tmp, cmd, MAX_DIR);
    int option = threadTree->option;

    if (option == 0) {
        if (strstr(tmp, "/") == NULL) {
            tmpNode = IsExistDir(dirTree, tmp, 'f');
            tmpNode2 = IsExistDir(dirTree, tmp, 'd');
            if (tmpNode == NULL && tmpNode2 == NULL) {
                printf("rm: cannot remove '%s': No such file or directory.\n", tmp);
                return NULL;
            }
            if (tmpNode == NULL) {
                printf("rm: cannot remove '%s': No such file or directory.\n", cmd);
                return NULL;
            } else {
                if (HasPermission(dirTree->current, 'w') != 0 || HasPermission(tmpNode, 'w') != 0) {
                    printf("rm: cannot remove '%s': Permission denied.\n", cmd);
                    return NULL;
                }
                RemoveDir(dirTree, tmp);
            }
        } else {
            strncpy(tmp2, getDir(tmp), MAX_DIR);
            int val = MovePath(dirTree, tmp2);
            if (val != 0) {
                printf("rm: cannot remove '%s': No such file or directory.\n", tmp2);
                return NULL;
            }
            char *str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            tmpNode = IsExistDir(dirTree, tmp3, 'f');
            tmpNode2 = IsExistDir(dirTree, tmp3, 'd');
            if (tmpNode2 != NULL) {
                printf("rm: cannot remove '%s': Is a directory.\n", tmp3);
                dirTree->current = currentNode;
                return NULL;
            }
            if (tmpNode != NULL) {
                printf("rm: cannot remove '%s' No such file or directory.\n", tmp3);
                dirTree->current = currentNode;
                return NULL;
            } else {
                if (HasPermission(dirTree->current, 'w') != 0 || HasPermission(tmpNode, 'w') != 0) {
                    printf("rm: cannot remove '%s': Permission denied.\n", tmp3);
                    dirTree->current = currentNode;
                    return NULL;
                }
                RemoveDir(dirTree, tmp3);
            }
            dirTree->current = currentNode;
        }
    } else if (option == 1) {
        if (!strstr(tmp, "/")) {
            tmpNode = IsExistDir(dirTree, tmp, 'd');
            if (tmpNode == NULL) {
                printf("rm: cannot remove '%s': No such file or directory.\n", tmp);
                return NULL;
            } else {
                if (HasPermission(dirTree->current, 'w') != 0 || HasPermission(tmpNode, 'w') != 0) {
                    printf("rm: cannot remove '%s': Permission denied.", tmp);
                    return NULL;
                }
                RemoveDir(dirTree, tmp);
            }
        } else {
            strncpy(tmp2, getDir(tmp), MAX_DIR);
            int val = MovePath(dirTree, tmp2);
            if (val != 0) {
                printf("rm: cannot remove '%s': No such file or directory.\n", tmp2);
                return NULL;
            }
            char *str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            tmpNode = IsExistDir(dirTree, tmp3, 'f');
            tmpNode = IsExistDir(dirTree, tmp3, 'd') == NULL ? tmpNode : IsExistDir(dirTree, tmp3, 'd');
            if (tmpNode != NULL) {
                printf("rm: cannot remove '%s': No such file or directory.\n", tmp3);
                dirTree->current = currentNode;
                return NULL;
            } else {
                if (HasPermission(dirTree->current, 'w') != 0 || HasPermission(tmpNode, 'w') != 0) {
                    printf("rm: failed to remove '%s' Can not remove directory or file: Permission denied.\n", tmp3);
                    dirTree->current = currentNode;
                    return NULL;
                }
                RemoveDir(dirTree, tmp3);
            }
            dirTree->current = currentNode;
        }
    } else if (option == 2) {
        if (strstr(tmp, "/") == NULL) {
            tmpNode = IsExistDir(dirTree, tmp, 'f');
            if (!tmpNode)
                return NULL;
            else {
                if (HasPermission(dirTree->current, 'w') != 0 || HasPermission(tmpNode, 'w') != 0) 
                    return NULL;
                RemoveDir(dirTree, tmp);
            }
        } else {
            strncpy(tmp2, getDir(tmp), MAX_DIR);
            int val = MovePath(dirTree, tmp2);
            if (val != 0)
                return NULL;
            char *str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            tmpNode = IsExistDir(dirTree, tmp3, 'f');
            if (tmpNode == NULL) {
                dirTree->current = currentNode;
                return NULL;
            } else {
                if (HasPermission(dirTree->current, 'w') != 0 || HasPermission(tmpNode, 'w') != 0) {
                    dirTree->current = currentNode;
                    return NULL;
                }
                RemoveDir(dirTree, tmp3);
            }
            dirTree->current = currentNode;
        }
    } else if (option == 3) {
        if (strstr(tmp, "/") == NULL) {
            tmpNode = IsExistDir(dirTree, tmp, 'f');
            tmpNode = IsExistDir(dirTree, tmp, 'd') == NULL ? tmpNode : IsExistDir(dirTree, tmp, 'd');
            if (tmpNode == NULL)
                return NULL;
            else {
                if (HasPermission(dirTree->current, 'w') != 0 || HasPermission(tmpNode, 'w') != 0)
                    return NULL;
                RemoveDir(dirTree, tmp);
            }
        } else {
            strncpy(tmp2, getDir(tmp), MAX_DIR);
            int val = MovePath(dirTree, tmp2);
            if (val != 0)
                return NULL;
            char *str = strtok(tmp, "/");
            while (str != NULL) {
                strncpy(tmp3, str, MAX_NAME);
                str = strtok(NULL, "/");
            }
            tmpNode = IsExistDir(dirTree, tmp3, 'f');
            tmpNode = IsExistDir(dirTree, tmp3, 'd') == NULL ? tmpNode : IsExistDir(dirTree, tmp3, 'd');
            if (tmpNode == NULL) {
                dirTree->current = currentNode;
                return NULL;
            } else {
                if (HasPermission(dirTree->current, 'w') != 0 || HasPermission(tmpNode, 'w') != 0) {
                    dirTree->current = currentNode;
                    return NULL;
                }
                RemoveDir(dirTree, tmp3);
            }
            dirTree->current = currentNode;
        }
    }
    pthread_exit(NULL);
}