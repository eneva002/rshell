#include <iostream>
#include <wordexp.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

int getcmd(string cmd, wordexp_t &result){
  const char *ccmd = cmd.c_str();

  switch(wordexp(ccmd, &result, 0)){
    case 0:
      break; //success
    case WRDE_NOSPACE: 
      wordfree(&result); //ran out of space
    default:
      return -1; //failure 
  }

  for(int j = 0; result.we_wordv[j] != NULL; j++){
    cout << result.we_wordv[j] << endl;
  }
}

int runcmd(wordexp_t result){
  int status;
  int pid = fork();
  if(pid == 0){
    if(-1 == execvp(result.we_wordv[0], result.we_wordv))
      perror("exec failed");
  }
  else if(pid < 0){
    status = -1; 
    perror("fork failed");
  }
  else{
    if(waitpid(pid, &status, 0) != pid){
      status = -1;
      perror("wait failed");
    }
  }
  return status;
}

int main(int argc, char* argv[]){
  string cmd;
  wordexp_t runme;

  cmd = "ls";

  if(-1 == getcmd(cmd, runme)){
    perror("cmd parse failed");
    exit(1);
  }

  if(-1 == runcmd(runme)){
    perror("runner failed");
    exit(1);
  }
 }
