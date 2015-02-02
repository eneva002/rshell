#include <iostream>
#include <wordexp.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sstream>

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

void printprompt(){
  char usrn[64];
  if(-1 == getlogin_r(usrn, sizeof(usrn)))
    perror("username acquisition failed");

  char host[64];
  if(-1 == gethostname(host, sizeof(host))) 
    perror("hostname acquisition failed");
  
  char *hostn;
  hostn = strtok(host, ".");
  cout << usrn << "@" << hostn << " --> ";
}

int main(int argc, char* argv[]){

  string cmd;
  wordexp_t runme;

  //int runcmd(wordexp_t result)
  //int getcmd(string cmd, wordexp_t &result)
 
  printprompt(); 
  getline(cin, cmd); 
  if(cmd == "exit") exit(0);

  if(-1 == getcmd(cmd, runme)){
    perror("cmd parse failed");
    exit(1);
  }

  if(-1 == runcmd(runme)){
    perror("runner failed");
    exit(1);
  }
 }
