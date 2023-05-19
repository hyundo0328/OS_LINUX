#include "../include/main.h"

//ls
int ListDir(DirectoryTree* dirTree, int a, int l)
{
    //variables
    DirectoryNode* tmpNode = NULL;
    DirectoryNode* tmpNode2 = NULL;
    char type;
    int cnt;

    tmpNode = dirTree->current->LeftChild;

    if(HasPermission(dirTree->current, 'r') != 0){
        printf("ls: '%s: Permission denied", dirTree->current->name);
        return -1;
    }

    if(l == 0){
        if(a == 0){
            if(tmpNode == NULL){
                return -1;
            }
        }
        if(a == 1){
	        BOLD;BLUE;
            printf(".       ");
	        DEFAULT;
            if(dirTree->current != dirTree->root){
	     	    BOLD;BLUE;
                printf("..      ");
		        DEFAULT;
            }
        }
        while(tmpNode != NULL){
            if(a == 0){
                if(strncmp(tmpNode->name,".",1) == 0){
                    tmpNode = tmpNode->RightSibling;
                    continue;
                }
            }
	        if(tmpNode->type == 'd'){
	    	    BOLD;BLUE;
         	    printf("%s      ", tmpNode->name);
	 	        DEFAULT;
	        }
	        else
         	    printf("%s      ", tmpNode->name);
            tmpNode = tmpNode->RightSibling;
        }
        printf("\n");
    }
    else{
        if(a == 0){
            if(tmpNode == NULL){
                printf("total: 0\n");
                return -1;
            }
        }
        if(a == 1){
            tmpNode2 = dirTree->current->LeftChild;
            if(tmpNode2 == NULL){
                cnt = 2;
            }
            else{
                if(tmpNode2->type == 'd')
                    cnt = 3;
                else
                    cnt = 2;

                while(tmpNode2->RightSibling != NULL){
                    tmpNode2 = tmpNode2->RightSibling;
                    if(tmpNode2->type == 'd')
                        cnt = cnt + 1;
                }
            }

            printf("%c", dirTree->current->type);
            PrintPermission(dirTree->current);
            printf("%3d", cnt);
            printf("   ");
            printf("%-5s%-5s", GetUID(dirTree->current), GetGID(dirTree->current));
            printf("%5d ", dirTree->current->SIZE);
            printf("%d월 %2d %02d:%02d ",dirTree->current->month, dirTree->current->day, dirTree->current->hour, dirTree->current->minute);
            BOLD;BLUE;
            printf(".\n");
	        DEFAULT;

            if(dirTree->current != dirTree->root){
                tmpNode2 = dirTree->current->Parent->LeftChild;
                if(tmpNode2 == NULL){
                    cnt = 2;
                }
                else{
                    if(tmpNode2->type == 'd')
                        cnt = 3;
                    else
                        cnt = 2;

                    while(tmpNode2->RightSibling != NULL){
                        tmpNode2 = tmpNode2->RightSibling;
                        if(tmpNode2->type == 'd')
                            cnt = cnt + 1;
                    }
                }
                printf("%c", dirTree->current->Parent->type);
                PrintPermission(dirTree->current->Parent);
                printf("%3d", cnt);
                printf("   ");
                printf("%-5s%-5s", GetUID(dirTree->current->Parent), GetGID(dirTree->current->Parent));
                printf("%5d ", dirTree->current->SIZE);
                printf("%d월 %2d %02d:%02d ",dirTree->current->Parent->month, dirTree->current->Parent->day, dirTree->current->Parent->hour, dirTree->current->Parent->minute);
                BOLD;BLUE;
                printf("..\n");
		DEFAULT;
            }
        }

        while(tmpNode != NULL){
            if(a == 0){
                if(strncmp(tmpNode->name,".",1) == 0){
                    tmpNode = tmpNode->RightSibling;
                    continue;
                }
            }
            tmpNode2 = tmpNode->LeftChild;
            if(tmpNode2 == NULL){
                if(tmpNode->type == 'd')
                    cnt = 2;
                else
                    cnt = 1;
            }
            else{
                if(tmpNode2->type == 'd')
                    cnt = 3;
                else
                    cnt = 2;

                while(tmpNode2->RightSibling != NULL){
                    tmpNode2 = tmpNode2->RightSibling;
                    if(tmpNode2->type == 'd')
                        cnt = cnt + 1;
                }
            }
            if(tmpNode->type == 'd')
                type = 'd';
            else if(tmpNode->type == 'f')
                type = '-';
            printf("%c", type);
            PrintPermission(tmpNode);
            printf("%3d", cnt);
            printf("   ");
            printf("%-5s%-5s", GetUID(tmpNode), GetGID(tmpNode));
            printf("%5d ", tmpNode->SIZE);
            printf("%d월 %2d %02d:%02d ",tmpNode->month, tmpNode->day, tmpNode->hour, tmpNode->minute);

            if(tmpNode->type == 'd'){
	    	BOLD;BLUE;
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

int ls(DirectoryTree* dirTree, char* cmd)         //완료
{
    DirectoryNode* tmpNode = NULL;
    char* str;
    int val;

    if(cmd == NULL){
        ListDir(dirTree, 0, 0);
        return 0;
    }

    if(cmd[0] == '-'){
        if(strcmp(cmd, "-al") == 0 || strcmp(cmd, "-la") == 0){
            str = strtok(NULL, " ");
            if(str != NULL){
                tmpNode = dirTree->current;
                val = MovePath(dirTree, str);
                if(val != 0)
                    return -1;
            }
            ListDir(dirTree, 1, 1);
        }
        else if(strcmp(cmd, "-l") == 0){
            str = strtok(NULL, " ");
            if(str != NULL){
                tmpNode = dirTree->current;
                val = MovePath(dirTree, str);
                if(val != 0)
                    return -1;
            }
            ListDir(dirTree, 0, 1);
        }
        else if(strcmp(cmd, "-a") == 0){
            str = strtok(NULL, " ");
            if(str != NULL){
                tmpNode = dirTree->current;
                val = MovePath(dirTree, str);
                if(val != 0)
                    return -1;
            }
            ListDir(dirTree, 1, 0);
        }
        else if(strcmp(cmd, "--help") == 0){
            printf("사용법: ls [<옵션>]... [<파일>]...\n");
            printf("  List information about the FILEs (the current directory by default).\n\n");
            printf("  Options:\n");
            printf("    -a, --all\t do not ignore entries starting with .\n");
            printf("    -l       \t use a long listing format\n");
            printf("        --help\t 이 도움말을 표시하고 끝냅니다\n");
            return -1;
        }
        else{
            str = strtok(cmd, "-");
            if(str == NULL){
                printf("Try 'ls --help' for more information.\n");
                return -1;
            }
            else{
            printf("Try 'ls --help' for more information.\n");
            return -1;
            }
        }
    }
    else{
        tmpNode = dirTree->current;
        val = MovePath(dirTree, cmd);
        if(val != 0)
            return -1;
        ListDir(dirTree, 0, 0);
        dirTree->current = tmpNode;
    }

    if(str != NULL)
        dirTree->current = tmpNode;

    return 0;
}
