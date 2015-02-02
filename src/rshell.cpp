#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]){
  char *line[] = {"ls", NULL};

  for(int i = 0; line[i] != NULL; i++){
    cout << line[i] << endl;
  }

  int pid = fork();

  if(-1 == pid){
   perror("fork failed");
   exit(1);
  }

  if(0 == pid){
    //something goes here
    cout << "child running process" << endl;
    if(-1 == execvp(line[0], line)) perror("exec failed");
    exit(0);
  }

  if(1 == pid){
    cout << "parent here" << endl;
    if(-1 == wait(0)) perror("child failed");
    exit(0);
  }
}
