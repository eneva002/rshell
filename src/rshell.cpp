#include <iostream>
#include <wordexp.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]){
  string cmd = "ls -l";
  const char *ccmd = cmd.c_str();

  wordexp_t result;
  pid_t pid;
  int status, i;

  switch(wordexp(ccmd, &result, 0)){
    case 0:
      break; //success
    case WRDE_NOSPACE: 
      wordfree(&result); //ran out of space
    default:
      return -1; //failure 
  }

  /*
  for(i = 0; options[i] != NULL; i++){
    if(wordexp(options[i], &result, WRDE_APPEND)){
      wordfree(&result);
      return -1;
    }
  }
  */

  pid = fork();
  if(pid == 0){
    if(-1 == execvp(result.we_wordv[0], result.we_wordv))
      perror("exec failed");
  }
  else if(pid < 0)
    status = -1; 
  else{
    if(waitpid(pid, &status, 0) != pid){
      status = -1;
      perror("wait failed");
    }
  }
}
