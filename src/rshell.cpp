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
#include <queue>
#include <vector>

using namespace std;

int getcmd(const string &cmd, wordexp_t &result){
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

int runcmd(const wordexp_t &result){
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
    perror("getlogin failed");

  char host[64];
  if(-1 == gethostname(host, sizeof(host))) 
    perror("gethostname failed");
  
  char *hostn;
  hostn = strtok(host, ".-");
  cout << usrn << "@" << hostn << " --> ";
}

typedef pair<int, int> P;
struct Order{ 
  bool operator()(P const& a, P const& b) const{ 
    return a.first > b.first;
  }
};

void popq(const string &cmd, queue< pair<string,int> > &cmdq){
  priority_queue<P,vector<P>,Order> list;

  int i = 0;
  while(i != string::npos){
    // find everything!
  }
  while(i != string::npos){
   // i = 
  }
}

int main(int argc, char* argv[]){

  string cmd;
  wordexp_t runme;
  queue< pair<string,int> > cmdq;

  //int runcmd(wordexp_t result)
  //int getcmd(string cmd, wordexp_t &result)
  while(1){ 
    printprompt(); 
    getline(cin, cmd); 

    //get rid of comments
    int cmnt = cmd.find("#");
    if(cmnt != string::npos) cmd = cmd.substr(0,cmnt);

    //handle exit
    if(cmd == "exit") exit(0);

    if(-1 == getcmd(cmd, runme)){
      perror("getcmd failed");
      exit(1);
    }

    if(-1 == runcmd(runme)){
      perror("runcmd failed");
      exit(1);
    }
    wordfree(&runme);
  }
 }
