#include "rshell.h"

using namespace std;

int main(int argc, char* argv[]){

  if(-1 == (ppid = getpid())) perror("getpid");

  string cmd;
  wordexp_t runme;
  queue< pair<string,int> > cmdq;

  signal(SIGINT, SIG_IGN);

  //int runcmd(wordexp_t result)
  //int getcmd(string cmd, wordexp_t &result)
  while(1)
  { 
  loopstart:
    printprompt(); 
    getline(cin, cmd); 

    if(cmd.size() > 0)
    {
      //search for first # and create substring for everything up till then
      int cmnt = cmd.find("#");
      if(cmnt != string::npos) cmd = cmd.substr(0,cmnt);
      if(cmd.size() < 1) continue;

      int status = 0;
      int prevstatus = 0;
      pair<string, int> o;
      popq(cmd, cmdq);
      string temp;

      while(!cmdq.empty())
      {
        //o is type pair<string,int>
        o = cmdq.front();
        cmdq.pop();
        
        //hardcoded exit
        //may consider going balls out and creating an exit executable
        if(o.first == "exit") exit(0);

        if(o.first.find("|") != string::npos)
        {
          piper(o.first);
        }
        else if(o.first.find("<") != string::npos 
             || o.first.find(">") != string::npos){
          redexec(o.first);
        }
        else
        {
          //hotfix for exit statuses
          if(prevstatus == -1) prevstatus = 256;
          else if(prevstatus == 1) prevstatus = 0;
          else if(prevstatus == 0) status = 0;

          //check if exit status matches that of the connector
          //if they match, execute
          //by default the first command will always execute
          if(status == prevstatus ){
            if(-1 == getcmd(o.first, runme))
              perror("getcmd failed");
            else if(-1 ==(status = runcmd(runme))){
              perror("generic runcmd failed");
              //may consider continuing if exit fails.
              //not sure if this exit is for the failed execution or for the shell
              //but it's working so i'm going to leave it
              if((string)runme.we_wordv[0] != "cd") exit(1);
            }
            else{
              wordfree(&runme);
            }
          }
          prevstatus = o.second;
        }
      }
    }
  }
}
