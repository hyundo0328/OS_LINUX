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
    fprintf(Directory, " %s\n", tmp);
}

void WriteNode(DirectoryTree* dirTree, DirectoryNode* dirNode, Stack* dirStack)
{
    fprintf(Directory, "%s %c %d ", dirNode->name, dirNode->type, dirNode->mode);
    fprintf(Directory, "%d %d %d %d %d %d %d", dirNode->SIZE, dirNode->UID, dirNode->GID, dirNode->month, dirNode->day, dirNode->hour, dirNode->minute);

    if(dirNode == dirTree->root)
        fprintf(Directory, "\n");
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
    char* command;

    NewNode->LeftChild = NULL;
    NewNode->RightSibling = NULL;
    NewNode->Parent = NULL;

    command = strtok(tmp, " ");
    strncpy(NewNode->name, command, MAX_NAME);
    command = strtok(NULL, " ");
    NewNode->type = command[0];
    command = strtok(NULL, " ");
    NewNode->mode = atoi(command);
    Atoi_permission(NewNode);
    command = strtok(NULL, " ");
    NewNode->SIZE = atoi(command);
    command = strtok(NULL, " ");
    NewNode->UID = atoi(command);
    command = strtok(NULL, " ");
    NewNode->GID = atoi(command);
    command = strtok(NULL, " ");
    NewNode->month = atoi(command);
    command = strtok(NULL, " ");
    NewNode->day = atoi(command);
    command = strtok(NULL, " ");
    NewNode->hour = atoi(command);
    command = strtok(NULL, " ");
    NewNode->minute = atoi(command);

    command = strtok(NULL, " ");
    if(command != NULL){
        command[strlen(command)-1] = '\0';
        MovePath(dirTree, command);
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

void GetMonth(int month)
{
    switch(month){
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

void GetWeek(int week)
{
    switch(week){
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
    char* command;
    int check_correct;
    if(strcmp(cmd, "") == 0 || cmd[0] == ' '){
        return;
    }
    command = strtok(cmd, " ");
    if(strcasecmp(command, "cat") == 0){
        command = strtok(NULL, " ");
        check_correct = cat(dirTree, command);
        if(check_correct == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(command, "cd") == 0){
        command = strtok(NULL, " ");
        cd(dirTree, command);
    }
    else if(strcasecmp(command, "chmod") == 0){
        command = strtok(NULL, " ");
        check_correct = chmod(dirTree, command);
        if(check_correct == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(command, "chown") == 0){
        command = strtok(NULL, " ");
        check_correct = ft_chown(dirTree, command);
        if(check_correct == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(command, "find") == 0){
        command = strtok(NULL, " ");
        find(dirTree, command);
    }
    else if(strcasecmp(command, "grep") == 0){
        command = strtok(NULL, " ");
        grep(dirTree, command);
    }
    else if(strcasecmp(command, "ls") == 0){
        command = strtok(NULL, " ");
        ls(dirTree, command);
    }
    else if(strcasecmp(command, "mkdir") == 0){
        command = strtok(NULL, " ");
        check_correct = mkdir(dirTree, command);
        if(check_correct == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(command, "pwd") == 0){
        command = strtok(NULL, " ");
        pwd(dirTree, dStack, command);
    }
    else if(strcasecmp(command, "rm") == 0){
        command = strtok(NULL, " ");
        check_correct = rm(dirTree, command);
        if(check_correct == 0){
            SaveDir(dirTree, dStack);
        }
    }
    else if(strcasecmp(cmd, "exit") == 0){
            printf("Logout\n");
            exit(0);
    }
    else{
        printf("command not found: %s\n", cmd);
    }
    return;
}

void Start()
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
    char user;

    if(usrList->current == usrList->head)
        user = '#';
    else
        user = '$';

    BOLD;GREEN;
    printf("%s@2-os-linux", usrList->current->name);
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
    printf("%c ", user);
}
