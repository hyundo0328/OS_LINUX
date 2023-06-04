#include "../include/main.h"

UserList* InitializeUser()
{
    UserList* returnList = (UserList*)malloc(sizeof(UserList));
    UserNode* tmpNode = (UserNode*)malloc(sizeof(UserNode));

    time(&ltime);
    today = localtime(&ltime);

    strncpy(tmpNode->name, "root", MAX_NAME);
    strncpy(tmpNode->dir, "/", MAX_NAME);
    tmpNode->UID = 0;
    tmpNode->GID = 0;
    tmpNode->year = today->tm_year+1900;
    tmpNode->month = today->tm_mon+1;
    tmpNode->wday = today->tm_wday;
    tmpNode->day = today->tm_mday;
    tmpNode->hour = today->tm_hour;
    tmpNode->minute = today->tm_min;
    tmpNode->sec = today->tm_sec;
    tmpNode->LinkNode = NULL;

    returnList->head = tmpNode;
    returnList->tail = tmpNode;
    returnList->current = tmpNode;
    returnList->topUID = 0;
    returnList->topGID = 0;

    return returnList;
}

void WriteUser(UserList* userList, UserNode* userNode)
{
    time(&ltime);
    today = localtime(&ltime);

    userList->current->year = today->tm_year+1900;
    userList->current->month = today->tm_mon+1;
    userList->current->wday = today->tm_wday;
    userList->current->day = today->tm_mday;
    userList->current->hour = today->tm_hour;
    userList->current->minute = today->tm_min;
    userList->current->sec =today->tm_sec;

    fprintf(User, "%s %d %d %d %d %d %d %d %d %d %s\n", userNode->name, userNode->UID, userNode->GID, userNode->year, userNode->month, userNode->wday, userNode->day, userNode->hour, userNode->minute, userNode->sec, userNode->dir);

    if(userNode->LinkNode != NULL){
        WriteUser(userList, userNode->LinkNode);
    }

}

void SaveUserList(UserList* userList)
{
    User = fopen("User.txt", "w");

    WriteUser(userList, userList->head);

    fclose(Dir);
}

int ReadUser(UserList* userList, char* tmp)
{
    UserNode* tmpNode = (UserNode*)malloc(sizeof(UserNode));
    char* str;

    tmpNode->LinkNode = NULL;

    str = strtok(tmp, " ");
    strncpy(tmpNode->name, str, MAX_NAME);
    str = strtok(NULL, " ");
    tmpNode->UID = atoi(str);
    str = strtok(NULL, " ");
    tmpNode->GID = atoi(str);
    str = strtok(NULL, " ");
    tmpNode->year = atoi(str);
    str = strtok(NULL, " ");
    tmpNode->month = atoi(str);
    str = strtok(NULL, " ");
    tmpNode->wday = atoi(str);
    str = strtok(NULL, " ");
    tmpNode->day = atoi(str);
    str = strtok(NULL, " ");
    tmpNode->hour = atoi(str);
    str = strtok(NULL, " ");
    tmpNode->minute = atoi(str);
    str = strtok(NULL, " ");
    tmpNode->sec = atoi(str);
    str = strtok(NULL, " ");
    str[strlen(str)-1] = '\0';
    strncpy(tmpNode->dir, str, MAX_DIR);

    if(strcasecmp(tmpNode->name, "root") == 0){
        userList->head = tmpNode;
        userList->tail = tmpNode;
    }
    else{
        userList->tail->LinkNode = tmpNode;
        userList->tail = tmpNode;
    }
    return 0;
}

UserList* LoadUserList()
{
    UserList* userList = (UserList*)malloc(sizeof(UserList));
    char tmp[MAX_LENGTH];

    User = fopen("User.txt", "r");

    while(fgets(tmp, MAX_LENGTH, User) != NULL){
        ReadUser(userList, tmp);
    }

    fclose(User);

    userList->current = NULL;

    return userList;
}

UserNode* IsExistUser(UserList* userList, char* userName)
{
    UserNode* returnUser = NULL;

    returnUser = userList->head;

    while(returnUser != NULL){
        if(strcasecmp(returnUser->name, userName) == 0)
            break;
        returnUser = returnUser->LinkNode;
    }

    return returnUser;
}

char* GetUID(DirectoryNode* dirNode)
{
    UserNode* tmpNode = NULL;

    tmpNode = usrList->head;
    while(tmpNode != NULL){
        if(tmpNode->UID == dirNode->UID)
            break;
        tmpNode = tmpNode->LinkNode;
    }
    return tmpNode->name;
}

char* GetGID(DirectoryNode* dirNode)
{
    UserNode* tmpNode = NULL;

    tmpNode = usrList->head;
    while(tmpNode != NULL){
        if(tmpNode->GID == dirNode->GID)
            break;
        tmpNode = tmpNode->LinkNode;
    }
    return tmpNode->name;
}

void Login(UserList* userList, DirectoryTree* dirTree)
{
    UserNode* tmpNode = NULL;
    char userName[MAX_NAME];
    char tmp[MAX_DIR];

    tmpNode= userList->head;

    printf("Users: ");
    while(tmpNode != NULL){
        printf("%s ", tmpNode->name);
        tmpNode = tmpNode->LinkNode;
    }
    printf("\n");

    while(1){
        printf("Login : ");
        fgets(userName, sizeof(userName), stdin);
        userName[strlen(userName)-1] = '\0';
        if(strcmp(userName, "exit") == 0){
            exit(0);
        }
        tmpNode = IsExistUser(userList, userName);
        if(tmpNode != NULL){
            userList->current = tmpNode;
            break;
        }
        printf("'%s' User does not exists\n", userName);
    }

    strcpy(tmp, userList->current->dir);
    MovePath(dirTree, tmp);
}
