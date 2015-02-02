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
#include <sys/types.h>
#include <signal.h>
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
      return -1;
  }
  else if(pid < 0){
    perror("fork failed");
    return -1; 
  }
  else{
    if(waitpid(pid, &status, 0) != pid){
      perror("wait failed");
      kill(pid, SIGKILL);
      return -1;
    }
  }
  return 1;
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
  string piece;
  P temp;
  vector<string> look;
  look.push_back(";");
  look.push_back("||");
  look.push_back("&&");

  for(int i = 0; i < 3; ++i){
    int j = 0;
    j = cmd.find(look[i], j);
    while(j != string::npos){ 
      temp.first = j;
      switch(i){
        case 0: 
          j+=1; break;
        case 1:
          j+=2; temp.second = -1; break;
        case 2:
          j+=2; temp.second = 1; break;
      }
      list.push(temp);
      j = cmd.find(look[i],j);
    }
  }

  int i = 0;
  while(!list.empty()){
    temp = list.top();
    list.pop();
    string temp2 = cmd.substr(i, temp.first-i);
    switch(temp.second){
      case -1:
        i = temp.first + 2; break;
      case 0:
        i = temp.first + 1; break;
      case 1:
        i = temp.first + 2; break;
    }
    cmdq.push(make_pair(temp2,temp.second));
  }
  cmdq.push(make_pair(cmd.substr(i, cmd.length()-i),0));
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
    
    int status = 0;
    int prevstatus = 0;
    pair<string, int> o;
    popq(cmd, cmdq);
    string temp;

    while(!cmdq.empty()){
      o = cmdq.front();
      cmdq.pop();
      //cout << "\"" << o.first << "\"" << " |:| " << o.second << endl;
      
      int cmnt = o.first.find("#");
      if(cmnt != string::npos) o.first = o.first.substr(0,cmnt);

      if(o.first == "exit") exit(0);

      if(status == prevstatus || prevstatus == 0){
        if(-1 == getcmd(o.first, runme))
          perror("getcmd failed");
        else if(-1 ==(status = runcmd(runme))){
          perror("runcmd failed");
        }
        else wordfree(&runme);
      }
      if(o.second) prevstatus = 0;
      prevstatus = o.second;
    }
  }

}
