#include "../include/main.h"

void DestroyNode(DirectoryNode* dirNode)
{
    free(dirNode);
}

void getPath(DirectoryTree* dirTree, DirectoryNode* dirNode, Stack* dirStack)
{
    DirectoryNode* tmpNode = NULL;
    char tmp[MAX_DIR] = "";

    tmpNode = dirNode->Parent;

    if(tmpNode == dirTree->root){
        strcpy(tmp, "/");
    }
    else{
        while(tmpNode->Parent != NULL){
            Push(dirStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        while(IsEmpty(dirStack) == 0){
                strcat(tmp, "/");
                strcat(tmp ,Pop(dirStack));
        }
    }
    fprintf(Dir, " %s\n", tmp);
}

void WriteNode(DirectoryTree* dirTree, DirectoryNode* dirNode, Stack* dirStack)
{
    fprintf(Dir, "%s %c %d ", dirNode->name, dirNode->type, dirNode->mode);
    fprintf(Dir, "%d %d %d %d %d %d %d", dirNode->SIZE, dirNode->UID, dirNode->GID, dirNode->month, dirNode->day, dirNode->hour, dirNode->minute);

    if(dirNode == dirTree->root)
        fprintf(Dir, "\n");
    else
        getPath(dirTree, dirNode, dirStack);

    if(dirNode->RightSibling != NULL){
        WriteNode(dirTree, dirNode->RightSibling, dirStack);
    }
    if(dirNode->LeftChild != NULL){
        WriteNode(dirTree, dirNode->LeftChild, dirStack);
    }
}

int ReadNode(DirectoryTree* dirTree, char* tmp)
{
    DirectoryNode* NewNode = (DirectoryNode*)malloc(sizeof(DirectoryNode));
    DirectoryNode* tmpNode = NULL;
    char* str;

    NewNode->LeftChild = NULL;
    NewNode->RightSibling = NULL;
    NewNode->Parent = NULL;

    str = strtok(tmp, " ");
    strncpy(NewNode->name, str, MAX_NAME);
    str = strtok(NULL, " ");
    NewNode->type = str[0];
    str = strtok(NULL, " ");
    NewNode->mode = atoi(str);
    Mode2Permission(NewNode);
    str = strtok(NULL, " ");
    NewNode->SIZE = atoi(str);
    str = strtok(NULL, " ");
    NewNode->UID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->GID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->month = atoi(str);
    str = strtok(NULL, " ");
    NewNode->day = atoi(str);
    str = strtok(NULL, " ");
    NewNode->hour = atoi(str);
    str = strtok(NULL, " ");
    NewNode->minute = atoi(str);

    str = strtok(NULL, " ");
    if(str != NULL){
        str[strlen(str)-1] = '\0';
        MovePath(dirTree, str);
        NewNode->Parent = dirTree->current;

        if(dirTree->current->LeftChild == NULL){
            dirTree->current->LeftChild = NewNode;
        }
        else{
            tmpNode = dirTree->current->LeftChild;

            while(tmpNode->RightSibling != NULL)
                tmpNode = tmpNode->RightSibling;

            tmpNode->RightSibling = NewNode;
        }
    }
    else{
        dirTree->root = NewNode;
        dirTree->current = dirTree->root;
    }

    return 0;
}

void GetMonth(int i)
{
    switch(i){
    case 1:
        printf("Jan ");
        break;
    case 2:
        printf("Feb ");
        break;
    case 3:
        printf("Mar ");
        break;
    case 4:
        printf("Apr ");
        break;
    case 5:
        printf("May ");
        break;
    case 6:
        printf("Jun ");
        break;
    case 7:
        printf("Jul ");
        break;
    case 8:
        printf("Aug ");
        break;
    case 9:
        printf("Sep ");
        break;
    case 10:
        printf("Oct ");
        break;
    case 11:
        printf("Nov ");
        break;
    case 12:
        printf("Dec ");
        break;
    default:
        break;
    }
}

void GetWeek(int i)
{
    switch(i){
    case 0:
        printf("Sun ");
        break;
    case 1:
        printf("Mon ");
        break;
    case 2:
        printf("Tue ");
        break;
    case 3:
        printf("Wed ");
        break;
    case 4:
        printf("Thu ");
        break;
    case 5:
        printf("Fri ");
        break;
    case 6:
        printf("Sat ");
        break;
    default:
        break;
    }
}

void Instruction(DirectoryTree* dirTree, char* cmd)
{
    char* str;
    int val;
    if(strcmp(cmd, "") == 0 || cmd[0] == ' '){
        return;
    }
    str = strtok(cmd, " ");

    if(strcasecmp(str, "mkdir") == 0){
        str = strtok(NULL, " ");
        val = mkdir(dirTree, str);
        if(val == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(str, "rm") == 0){
        str = strtok(NULL, " ");
        val = rm(dirTree, str);
        if(val == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(str, "cd") == 0){
        str = strtok(NULL, " ");
        cd(dirTree, str);
    }
    else if(strcasecmp(str, "ls") == 0){
        str = strtok(NULL, " ");
        ls(dirTree, str);
    }
    else if(strcasecmp(str, "cat") == 0){
        str = strtok(NULL, " ");
        val = cat(dirTree, str);
        if(val == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(str, "grep") == 0){
        str = strtok(NULL, " ");
        grep(dirTree, str);
    }
    else if(strcasecmp(str, "chmod") == 0){
        str = strtok(NULL, " ");
        val = chmod(dirTree, str);
        if(val == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(str, "chown") == 0){
        str = strtok(NULL, " ");
        val = ft_chown(dirTree, str);
        if(val == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(str, "pwd") == 0){
        str = strtok(NULL, " ");
        pwd(dirTree, dStack, str);
    }
    else if(strcasecmp(str, "find") == 0){
        str = strtok(NULL, " ");
        find(dirTree, str);
    }
    else if(strcasecmp(cmd, "exit") == 0){
            printf("logout\n");
            exit(0);
    }
    else{
        printf("command not found: %s\n", cmd);
    }
    return;
}

void PrintStart()
{
    printf("Last login: ");
    GetWeek(usrList->current->wday);
    GetMonth(usrList->current->month);
    printf("%d %02d:%02d:%02d %d\n", usrList->current->day, usrList->current->hour, usrList->current->minute, usrList->current->sec, usrList->current->year);
}

void PrintHead(DirectoryTree* dirTree, Stack* dirStack)
{
    //variables
    DirectoryNode* tmpNode = NULL;
    char tmp[MAX_DIR] = "";
    char tmp2[MAX_DIR] = "";
    char usr;

    if(usrList->current == usrList->head)
        usr = '#';
    else
        usr = '$';

    BOLD;GREEN;
    printf("%s@5-os-linux", usrList->current->name);
    DEFAULT;
    printf(":");
    tmpNode = dirTree->current;

    if(tmpNode == dirTree->root){
        strcpy(tmp, "/");
    }
    else{
        while(tmpNode->Parent != NULL){
            Push(dirStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        while(IsEmpty(dirStack) == 0){
                strcat(tmp, "/");
                strcat(tmp ,Pop(dirStack));
        }
    }

    strncpy(tmp2, tmp, strlen(usrList->current->dir));

    if(usrList->current == usrList->head){
        BOLD;BLUE;
        printf("%s", tmp);
    }
    else if(strcmp(usrList->current->dir, tmp2) != 0){
	BOLD;BLUE;
        printf("%s", tmp);
    }
    else{
        tmpNode = dirTree->current;
        while(tmpNode->Parent != NULL){
            Push(dirStack, tmpNode->name);
            tmpNode = tmpNode->Parent;
        }
        Pop(dirStack);
        Pop(dirStack);
	    BOLD;BLUE;
        printf("~");
        while(IsEmpty(dirStack) == 0){
            printf("/");
            printf("%s",Pop(dirStack));
        }
    }
    DEFAULT;
    printf("%c ", usr);
}
