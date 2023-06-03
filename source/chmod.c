#include "../include/main.h"

int chmod(DirectoryTree* dirTree, char* cmd)
{
    DirectoryNode* tmpNode = NULL;
    pthread_t threadPool[MAX_THREAD];
    ThreadTree threadTree[MAX_THREAD];

    int thread_cnt = 0;
    char* str;
    int tmp;

    if(cmd == NULL){    //chmod 외에 아무것도 적히지 않았을 때
        printf("chmod: missing operand\n");
        printf("Try 'chmod --help' for more information.\n");
        return -1;
    }
    if(cmd[0] == '-'){      //옵션 있을 경우
        if(strcmp(cmd, "--help") == 0){     //--help 입력했을 때
            printf("Usage: chmod [OPTION]... MODE[,MODE]... FILE...\n");
            printf("  or:  chmod [OPTION]... OCTAL-MODE FILE...\n");
            printf("  or:  chmod [OPTION]... --reference=RFILE FILE...\n");
            printf("Change the mode of each FILE to MODE.\n");
            printf("With --reference, change the mode of each FILE to that of RFILE.\n\n");
            
            printf("    -R, --recursive\t change files and directories recursively\n");
            printf("        --help\t display this help and exit\n\n");
            
            printf("Each MODE is of the form '[ugoa]*([-+=]([rwxXst]*|[ugo]))+|[-+=][0-7]+'.\n\n");
            
            printf("Each MODE is of the form '[ugoa]*[-+=]([rwxXst]*|[ugo]))+|[-+=][0-7]+'.\n\n");
            printf("GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n");
            printf("Report any translation bugs to <https://translationproject.org/team/>\n");
            printf("Full documentation <https://www.gnu.org/software/coreutils/mkdir>\n");
            printf("or available locally via: info '(coreutils) mkdir invocation'\n");
        }
        return -1;
    }
    else{       //옵션 없을 경우
        if(cmd[0]-'0'<8 && cmd[1]-'0'<8 && cmd[2]-'0'<8 && strlen(cmd)==3){     //숫자로 표현된 권한을 넣어주기 위해(0-7까지)
            tmp = atoi(cmd);
            str = strtok(NULL, " ");
            if(str == NULL){
                printf("Try 'chmod --help' for more information.\n");
                return -1;
            }
            while (str) {       //멀티스레드 작업을 위해 파일이름마다 스레드배열안에 정보를 저장
                threadTree[thread_cnt].threadTree = dirTree;
                threadTree[thread_cnt].cmd = str;
                threadTree[thread_cnt++].mode = tmp;
                str = strtok(NULL, " ");
            }
        }
        else{
            printf("chmod: missing operand after '%s'\n", cmd);
            printf("Try 'chmod --help' for more information.\n");
            return -1;
        }
        for (int i = 0; i < thread_cnt; i++) {      //pthread생성 후 chmod_thread로 처리, 마지막으로 join
            pthread_create(&threadPool[i], NULL, chmod_thread, (void*)&threadTree[i]);
            pthread_join(threadPool[i], NULL);
        }
    }
    return 0;
}

int ChangeMode(DirectoryTree* dirTree, int mode, char* dirName)     //권한 바꿔주는 함수
{
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;

    tmpNode = IsExistDir(dirTree, dirName, 'd');
    tmpNode2 = IsExistDir(dirTree, dirName, 'f');

    if(tmpNode != NULL){
        if(HasPermission(tmpNode, 'w') != 0){   //허가권한이 거부되었을 때
            printf("chmod: changing permissions of '%s': Operation not permitted\n", dirName);
            return -1;
        }
        tmpNode->mode = mode;
        Mode2Permission(tmpNode);
    }
    else if(tmpNode2 != NULL){
        if(HasPermission(tmpNode2, 'w') != 0){      //허가권한이 거부되었을 때
            printf("chmod: changing permissions of '%s': Operation not permitted\n", dirName);
            return -1;
        }
        tmpNode2->mode = mode;
        Mode2Permission(tmpNode2);
    }
    else if ((tmpNode == NULL) && (tmpNode2 == NULL)){      //파일 또는 디렉토리가 없을 때
        printf("chmod: cannot access '%s': No such file or directory\n", dirName);
        return -1;
    }
    return 0;
}

void *chmod_thread(void *arg) {
    ThreadTree *threadTree = (ThreadTree *) arg;
    DirectoryTree *dirTree = threadTree->threadTree;
    int mode = threadTree->mode;
    char *cmd = threadTree->cmd;
    
    ChangeMode(dirTree, mode, cmd);
    pthread_exit(NULL);
}