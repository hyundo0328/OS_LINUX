#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#ifndef MAIN_H_
#define MAIN_H_
#define DEFAULT printf("%c[%dm", 0x1B, 0)
#define BOLD printf("%c[%dm", 0x1B, 1)
#define WHITE printf("\x1b[37m")
#define BLUE printf("\x1b[34m")
#define GREEN printf("\x1b[32m")

#define MAX_BUFFER 512
#define MAX_LENGTH 200
#define MAX_DIR 50
#define MAX_NAME 20
#define MAX_THREAD 50


//User
typedef struct tagUserNode{
    char name[MAX_NAME];
    char dir[MAX_DIR];
    int UID;
    int GID;
    int year;
    int month;
    int wday;
    int day;
    int hour;
    int minute;
    int sec;
    struct tagUserNode* LinkNode;
}UserNode;

typedef struct tagUser{
    int topUID;
    int topGID;
    UserNode* head;
    UserNode* tail;
    UserNode* current;
}UserList;


//Directory
typedef struct tagDirectoryNode{
    char name[MAX_NAME];
    char type;
    int mode;
    int permission[9];
    int SIZE;
    int UID;
	int GID;
	int month;
    int day;
    int hour;
    int minute;
	struct tagDirectoryNode* Parent;
	struct tagDirectoryNode* LeftChild;
	struct tagDirectoryNode* RightSibling;
}DirectoryNode;

typedef struct tagDirectoryTree{
	DirectoryNode* root;
	DirectoryNode* current;
}DirectoryTree;

//stack using linked list
typedef struct tagStackNode{
	char name[MAX_NAME];
	struct tagStackNode* LinkNode;
}StackNode;

typedef struct tagStack{
	StackNode* TopNode;
	int cnt;
}Stack;

typedef struct threadTree {
    DirectoryTree *threadTree;
    DirectoryTree *name;
    char *cmd;
    char *username;
    int mode;
    int option;
} ThreadTree;

//time
time_t ltime;
struct tm *today;

//mkdir.c
int mkdir(DirectoryTree* dirTree, char* cmd);
DirectoryTree* InitializeTree();
int MakeDir(DirectoryTree* dirTree, char* dirName, char type);

//rm.c
int rm(DirectoryTree* dirTree, char* cmd);
int RemoveDir(DirectoryTree* dirTree, char* dirName);

//cd.c
int cd(DirectoryTree* dirTree, char* cmd);
int Movecurrent(DirectoryTree* dirTree, char* dirPath);
int MovePath(DirectoryTree* dirTree, char* dirPath);

//pwd.c
int pwd(DirectoryTree* dirTree, Stack* dirStack, char* cmd);
int PrintPath(DirectoryTree* dirTree, Stack* dirStack);

//ls.c
int ls(DirectoryTree* dirTree, char* cmd);
int ListDir(DirectoryTree* dirTree, int a, int l);

//cat.c
int cat(DirectoryTree* dirTree, char* cmd);
int Concatenate(DirectoryTree* dirTree, char* fName, int o);

//chmod.c
int chmod(DirectoryTree* dirTree, char* cmd);
int ChangeMode(DirectoryTree* dirTree, int mode, char* dirName);

//chown.c
int chown_(DirectoryTree* dirTree, char* cmd);
int ChangeOwner(DirectoryTree* dirTree, char* userName, char* dirName, int flag);

//find.c
int find_(DirectoryTree* dirTree, char* cmd);
int ReadDir(DirectoryTree* dirTree, char* tmp, char* dirName,int o);
void FindDir(DirectoryTree* dirTree, char* dirName, int o);

//utility.c
int Mode2Permission(DirectoryNode* dirNode);
void PrintPermission(DirectoryNode* dirNode);
void DestroyNode(DirectoryNode* dirNode);
void getPath(DirectoryTree* dirTree, DirectoryNode* dirNode, Stack* dirStack);
void WriteNode(DirectoryTree* dirTree, DirectoryNode* dirNode, Stack* dirStack);
int ReadNode(DirectoryTree* dirTree, char* tmp);
void GetMonth(int i);
void GetWeekday(int i);
void Instruction(DirectoryTree* dirTree, char* cmd);
void PrintStart();
void PrintHead(DirectoryTree* dirTree, Stack* dirStack);


//directory.c
void DestroyDir(DirectoryNode* dirNode);
DirectoryNode* IsExistDir(DirectoryTree* dirTree, char* dirName, char type);
char* getDir(char* dirPath);
void SaveDir(DirectoryTree* dirTree, Stack* dirStack);
DirectoryTree* LoadDir();

//user.c
UserList* InitializeUser();
void WriteUser(UserList* userList, UserNode* userNode);
void SaveUserList(UserList* userList);
int ReadUser(UserList* userList, char* tmp);
UserList* LoadUserList();
UserNode* IsExistUser(UserList* userList, char* userName);
char* GetUID(DirectoryNode* dirNode);
char* GetGID(DirectoryNode* dirNode);
int HasPermission(DirectoryNode* dirNode, char o);
void Login(UserList* userList, DirectoryTree* dirTree);

//stack.c
int IsEmpty(Stack* dirStack);
Stack* InitializeStack();
int Push(Stack* dirStack, char* dirName);
char* Pop(Stack* dirStack);

//global variable
DirectoryTree* Linux;
Stack* dStack;
UserList* usrList;
FILE* Dir;
FILE* User;
sem_t semp;

#endif // MAIN_H_
