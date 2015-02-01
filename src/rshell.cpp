#include <string>
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
  char *line[10]; 

  char *cmd = "ls";
  char *end = "NULL";

  line[0] = cmd;
  line[1] = end;

  for(int i = 0; line[i] != "NULL"; ++i){
    cout << line[i] << endl;
  }

  int pid = fork();

  if(-1 == pid)
   perror("fork failed");

  if(0 == pid){
    //something goes here
    cout << "child running process" << endl;
    if(-1 == execvp(line[0], line)) perror("exec failed");
  }

  if(1 == pid){
    if(-1 == wait(0)) perror("child failed");
    cout << "parent here" << endl;
  }
}
