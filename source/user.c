#include "../include/main.h"

UserList* InitializeUser()
{
    UserList* returnList = (UserList*)malloc(sizeof(UserList));
    UserNode* NewNode = (UserNode*)malloc(sizeof(UserNode));

    //get time
    time(&ltime);
    today = localtime(&ltime);

    strncpy(NewNode->name, "root", MAX_NAME);
    strncpy(NewNode->dir, "/", MAX_NAME);
    NewNode->UID = 0;
    NewNode->GID = 0;
    NewNode->year = today->tm_year+1900;
    NewNode->month = today->tm_mon+1;
    NewNode->wday = today->tm_wday;
    NewNode->day = today->tm_mday;
    NewNode->hour = today->tm_hour;
    NewNode->minute = today->tm_min;
    NewNode->sec = today->tm_sec;
    NewNode->LinkNode = NULL;

    returnList->head = NewNode;
    returnList->tail = NewNode;
    returnList->current = NewNode;
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
    UserNode* NewNode = (UserNode*)malloc(sizeof(UserNode));
    char* str;

    NewNode->LinkNode = NULL;

    str = strtok(tmp, " ");
    strncpy(NewNode->name, str, MAX_NAME);
    str = strtok(NULL, " ");
    NewNode->UID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->GID = atoi(str);
    str = strtok(NULL, " ");
    NewNode->year = atoi(str);
    str = strtok(NULL, " ");
    NewNode->month = atoi(str);
    str = strtok(NULL, " ");
    NewNode->wday = atoi(str);
    str = strtok(NULL, " ");
    NewNode->day = atoi(str);
    str = strtok(NULL, " ");
    NewNode->hour = atoi(str);
    str = strtok(NULL, " ");
    NewNode->minute = atoi(str);
    str = strtok(NULL, " ");
    NewNode->sec = atoi(str);
    str = strtok(NULL, " ");
    str[strlen(str)-1] = '\0';
    strncpy(NewNode->dir, str, MAX_DIR);

    if(strcasecmp(NewNode->name, "root") == 0){
        userList->head = NewNode;
        userList->tail = NewNode;
    }
    else{
        userList->tail->LinkNode = NewNode;
        userList->tail = NewNode;
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
    UserNode* tmpUser = NULL;
    char userName[MAX_NAME];
    char tmp[MAX_DIR];

    tmpUser= userList->head;

    printf("Users: ");
    while(tmpUser != NULL){
        printf("%s ", tmpUser->name);
        tmpUser = tmpUser->LinkNode;
    }
    printf("\n");

    while(1){
        printf("Login : ");
        fgets(userName, sizeof(userName), stdin);
        userName[strlen(userName)-1] = '\0';
        if(strcmp(userName, "exit") == 0){
            exit(0);
        }
        tmpUser = IsExistUser(userList, userName);
        if(tmpUser != NULL){
            userList->current = tmpUser;
            break;
        }
        printf("'%s' User does not exists\n", userName);
    }

    strcpy(tmp, userList->current->dir);
    MovePath(dirTree, tmp);
}
