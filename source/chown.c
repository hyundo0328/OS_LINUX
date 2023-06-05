#include "../include/main.h"

int ft_chown(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* tmpNode = NULL;
    UserNode* tmpUser = NULL;
    pthread_t threadPool[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];

    int thread_count = 0;
    char* command;
    char tmp[MAX_NAME];

    if(cmd == NULL){        //chown 외에 아무것도 적지 않았을 경우
        printf("chown: missing operand\n");
        printf("Try 'chown --help' for more information.\n");
        return -1;
    }
    if(cmd[0] == '-'){      //옵션이 있을 경우
        if(strcmp(cmd, "--help") == 0){     //--help를 입력했을 때
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
        else {      //그 외의 옵션 에러처리
            printf("chown: invalid option '%s'\n", cmd);
            printf("Try 'chown --help' for more information\n");
        }
        return -1;
    }
    else{
        strncpy(tmp, cmd, MAX_NAME);
        command = strtok(NULL, " ");
        if(command == NULL){        //파일 또는 디렉토리를 적지 않았을 경우
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
        else{
            while (command) {        //멀티스레드 작업을 위해 파일이름마다 스레드배열안에 정보를 저장
                threadTree[thread_count].threadTree = dirTree;
                threadTree[thread_count].username = tmp;
                threadTree[thread_count++].cmd = command;
                command = strtok(NULL, " ");
            }
        }
    }
    for (int i = 0; i < thread_count; i++) {      //pthread생성 후 chmod_thread로 처리, 마지막으로 join
        pthread_create(&threadPool[i], NULL, chown_thread, (void*)&threadTree[i]);
        pthread_join(threadPool[i], NULL);
    }
    return 0;
}

int ChangeOwner(DirectoryTree* dirTree, char* userName, char* dirName, int flag)        //허가권한 바꿔주는 함수
{
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    UserNode* tmpUser = NULL;

    tmpNode = IsExistDir(dirTree, dirName, 'd');
    tmpNode2 = IsExistDir(dirTree, dirName, 'f');


    if(tmpNode != NULL){
        if(HasPermission(tmpNode, 'w') != 0){       //허가권한이 거부되었을 때
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
        else{       //존재하지 않는 유저를 적었을 경우
            printf("chown: missing operand after '%s'\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else if(tmpNode2 != NULL){
        if(HasPermission(tmpNode2, 'w') != 0){      //허가권한이 거부되었을 때
            printf("chown: changing ownership of '%s': Operation not permitted\n", dirName);
            return -1;
        }
        tmpUser = IsExistUser(usrList, userName);
        if(tmpUser != NULL){
            if (flag == 0)
                tmpNode2->UID = tmpUser->UID;
            else
                tmpNode2->GID = tmpUser->GID;
        }
        else{       //존재하지 않는 유저를 적었을 경우
            printf("chown: missing operand after '%s'\n", userName);
            printf("Try 'chown --help' for more information.\n");
            return -1;
        }
    }
    else{
        printf("chown: cannot access '%s': No such file or directory\n", dirName);   //파일 또는 디렉토리가 없을 경우
        return -1;
    }

    return 0;
}

void *chown_thread(void *arg) {     //파일마다 스레드로 실행되는 함수
    ThreadTree *threadTree = (ThreadTree *)arg;
    DirectoryTree *dirTree = threadTree->threadTree;
    char *dirName = threadTree->cmd;
    char *tmp = threadTree->username;
    char *change_id;
    char tmp2[MAX_NAME];

    if(!strstr(tmp, ":"))
        ChangeOwner(dirTree, tmp, dirName, 0);      //userid 바꿔주기
    else {
        strncpy(tmp2, tmp, MAX_NAME);
        change_id = strtok(tmp, ":");
        if (change_id != NULL && strcmp(tmp, tmp2) != 0){
            ChangeOwner(dirTree, change_id, dirName, 0);     //userid 바꿔주기
            change_id = strtok(NULL, " ");
            if (change_id != NULL)
                ChangeOwner(dirTree, change_id, dirName, 1);     //groupid 바꿔주기
        }
        else if (change_id != NULL && strcmp(tmp, tmp2) == 0)
            ChangeOwner(dirTree, change_id, dirName, 1);     //groupid 바꿔주기
    }
    pthread_exit(NULL);
}
