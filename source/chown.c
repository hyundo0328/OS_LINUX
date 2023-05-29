#include "../include/main.h"

int ft_chown(DirectoryTree* dirTree, char* cmd)      //완료
{
    DirectoryNode* tmpNode = NULL;
    UserNode* tmpUser = NULL;
    pthread_t threadPool[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];

    int thread_cnt = 0;
    char* str;
    char tmp[MAX_NAME];

    if(cmd == NULL){
        printf("chown: missing operand\n");
        printf("Try 'chown --help' for more information.\n");
        return -1;
    }
    if(cmd[0] == '-'){
        if(strcmp(cmd, "--help") == 0){
            printf("Usage: chown [OPTION]... [OWNER][:[GROUP]] FILE...\n");
            printf("  or:  chown [OPTION]... --reference=RFILE FILE\n");
            printf("Change the owner and/or group of each FILE to OWNER and/or GROUP.\n");
            printf("With --reference, change the mode of each FILE to that of RFILE.\n\n");
            
            printf("    -R, --recursive\t operate on files and directories recursively\n\n");
            
            printf("The following options modify how a hierarchy is traversed when the -R\n");
            printf("option is also spectified. If more than one is specified, only the final\n");
            printf("one takes effect.\n\n");
            
            printf("        --help\t display this help and exit\n\n");
            
            printf("Owner is unchanged if missing. Group is unchanged if missing, but changed\n");
            printf("to login group if implied by a ':' following a symbolic OWNER.\n");
            printf("OWNER and GROUP may be numeric as well as symbolic\n\n");
            
            printf("Examples:\n");
            printf("  chown root /u\t Change the owner of /u to \"root\".\n");
            printf("  chown root:staff /u\t Likewise, but also change its group to \"staff\".\n");
            printf("  chown -hR root /u\t Change the owner of /u and subfiles to \"root\".\n\n");
            
            printf("GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n");
            printf("Report any translation bugs to <https://translationproject.org/team/>\n");
            printf("Full documentation <https://www.gnu.org/software/coreutils/mkdir>\n");
            printf("or available locally via: info '(coreutils) mkdir invocation'\n");
        }
        else {
            printf("chown: unrecognized option '%s'\n", cmd);
            printf("Try 'chown --help' for more information\n");
        }
        return -1;
    }
    else{
        strncpy(tmp, cmd, MAX_NAME);
        str = strtok(NULL, " ");
        if(str == NULL){
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
        else{
            while (str) {
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].username = tmp;
                threadTree[thread_cnt++].cmd = str;
                str = strtok(NULL, " ");
            }
        }
    }
    for (int i = 0; i < thread_cnt; i++) {
        pthread_create(&threadPool[i], NULL, chown_thread, (void*)&threadTree[i]);
        pthread_join(threadPool[i], NULL);
    }
    return 0;
}

int ChangeOwner(DirectoryTree* dirTree, char* userName, char* dirName, int flag)
{
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    UserNode* tmpUser = NULL;

    tmpNode = IsExistDir(dirTree, dirName, 'd');
    tmpNode2 = IsExistDir(dirTree, dirName, 'f');


    if(tmpNode != NULL){
        if(HasPermission(tmpNode, 'w') != 0){
            printf("chown: changing ownership of '%s': Operation not permitted\n", dirName);
            return -1;
        }
        tmpUser = IsExistUser(usrList, userName);
        if(tmpUser != NULL){
            if (flag == 0)
                tmpNode->UID = tmpUser->UID;
            else
                tmpNode->GID = tmpUser->GID;
        }
        else{
            printf("chown: missing operand after '%s'\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else if(tmpNode2 != NULL){
        if(HasPermission(tmpNode2, 'w') != 0){
            printf("chown: changing ownership of '%s': Operation not permitted\n", dirName);
            return -1;
        }
        tmpUser = IsExistUser(usrList, userName);
        if(tmpUser != NULL){
            if (flag == 0)
                tmpNode->UID = tmpUser->UID;
            else
                tmpNode->GID = tmpUser->GID;
        }
        else{
            printf("chown: missing operand after '%s'\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else{
        printf("chown: invalid user: %s\n", dirName);
        return -1;
    }

    return 0;
}

void *chown_thread(void *arg) {
    ThreadTree *threadTree = (ThreadTree *)arg;
    DirectoryTree *dirTree = threadTree->threadTree;
    char *cmd = threadTree->cmd;
    char *tmp = threadTree->username;
    char *str;

    if(!strstr(tmp, ":")) 
        ChangeOwner(dirTree, tmp, cmd, 0);
    else {
        char tmp2[MAX_NAME];
        strncpy(tmp2, tmp, MAX_NAME);
        char *str2 = strtok(tmp, ":");
        if (str2 != NULL && strcmp(tmp, tmp2) != 0){
            ChangeOwner(dirTree, str2, cmd, 0);
            str2 = strtok(NULL, " ");
            if (str2 != NULL)
                ChangeOwner(dirTree, str2, cmd, 1);
        }
        else if (str2 != NULL && strcmp(tmp, tmp2) == 0)
            ChangeOwner(dirTree, str2, cmd, 1);
    }
    pthread_exit(NULL);
}
