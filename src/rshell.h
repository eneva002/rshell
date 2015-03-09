#include <iostream>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <array>
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
#include <pwd.h>
#include <vector>

//i know this is bad practice but it's too much work to 
//go back and fix it at the moment
using namespace std;

//determine username and hostname, then print prompt
void printprompt()
{
  char wd[1024];
  if(NULL == getcwd(wd, 1024)) perror("getenv failed");
  uid_t usrID = getuid();
  struct passwd *pwd;
  pwd = getpwuid(usrID);
  char *usrn = pwd->pw_name;

  char host[64];
  if(-1 == gethostname(host, sizeof(host))) 
    perror("gethostname failed");
  
  char *hostn;
  hostn = strtok(host, ".-");
  cout << usrn << "@" << hostn << "-" << wd << "$ ";
}

pid_t ppid;
//signal handler
void handler(int i)
{
  if(i == SIGINT){
    pid_t curpid;

    if(-1 == (curpid = getpid())) perror("getpid");

    if(curpid != ppid){
      if( -1 == kill(curpid, SIGINT)) perror("kill");
    } 
  }
  char nl[] = "\n";
  if(-1 == write(STDIN_FILENO,nl,strlen(nl))) perror("write");
  return;
}

//parse string into wordexp_t
int getcmd(const string &cmd, wordexp_t &result)
{
  const char *ccmd = cmd.c_str();

  //wordexp converts a string into a char **
  switch(wordexp(ccmd, &result, 0)){
    case 0:
      break; //success
    case WRDE_NOSPACE: 
      wordfree(&result); //ran out of space
    default:
      perror("wordexp failed"); //failure 
      return -1;
  }
}

int myexec(const wordexp_t &result)
{
  int status;
  char *cpth;
  if(NULL == (cpth = getenv("PATH"))) perror("getenv failed");
  string spth = cpth;
  string pth;
  size_t start = 0;
  size_t loc = spth.find(':', start);

  execv(result.we_wordv[0], result.we_wordv);
  while(loc != string::npos){
    pth = spth.substr(start, loc-start) + "/" + (string)result.we_wordv[0];
    start = loc+1;
    loc = spth.find(':',start);
    execv(pth.c_str(), result.we_wordv);
  }
  pth = spth.substr(start, loc-start) + "/" + (string)result.we_wordv[0];
  if(-1 == (status = execv(pth.c_str(), result.we_wordv))){
    perror("exec failed");
    return -1;
  }
  return 0;
}

//execute command, taking a wordexp_t type as an input
int runcmd(const wordexp_t &result)
{
  int status;
  //cast the char* to a string because it doesn't work otherwise
  if((string)result.we_wordv[0] == "cd"){
    if(result.we_wordc != 2)
    {
      cerr << "requires 2 arguments" << endl;
      return -1;
    }
    else if(-1 == chdir(result.we_wordv[1])) {
      perror("chdir failed");
      return -1;
    }
    else return 1;
  }

  int pid = fork();

  if(pid == 0){
    signal(SIGINT, SIG_DFL);
    status  = myexec(result);
    return status;
  }

  else if(pid < 0){
    perror("fork failed");
    exit(1); 
  }
  else{
    if(waitpid(pid, &status, 0) != pid){
      perror("wait failed");
    }
  }
  return status;
}

//unary function to sort priority_queue in the popq funct.
typedef pair<int, int> P;
struct Order{ 
  bool operator()(P const& a, P const& b) const{ 
    return a.first > b.first;
  }
};

//function to pupulate a command list to be ran in the shell
void popq(const string &cmd, queue< pair<string,int> > &cmdq)
{
  //the priority queue is to maintain the order that commands are found
  priority_queue<P,vector<P>,Order> list;
  P temp;

  //vector of connectors to look for
  vector<string> look = {";", "||", "&&"};

  for(int i = 0; i < 3; ++i)
  {
    int j = 0;
    j = cmd.find(look[i], j);

    while(j != string::npos)
    {
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
  while(!list.empty())
  {
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

//pexec is a function to handle piped execution
int pexec(const string &cmd, const int inFD, const int outFD, int fd[2])
{
  
  cout << "PIPER" << endl;
  int saveout, savein;
  int outfd, infd;
  int pid;
  if(-1 == (pid = fork())){ perror("fork failed"); exit(-1); }
  
  if(pid == 0)
  {
    wordexp_t runme;
    signal(SIGINT, SIG_DFL);
    if(-1 == getcmd(cmd, runme)) perror("getcmd failed");

    //if there is an input FD then close default fd 0 and dup pipe input fd
    if(inFD > 0){
      if(-1 == (savein = dup(0))) perror("dup failed");
      if(-1 == dup2(inFD, 0)) perror("dup failed");
      //cout << "INSIDE " << runme.we_wordv[0] << " dup2(" << inFD << "," 
      //  << 0 << ")" << endl;
    }
    //otherwise close input pipe fd
    else{
      if(-1 == close(fd[0])) perror("failed pexec close2");
    }

    //if there is an output FD then close default fd 1 and dup pipe output fd
    if(outFD > 0){
      
      if(-1 == (saveout = dup(1))) perror("dup failed");
      if( -1 == dup2(outFD,1)) perror("dup failed");
      //cout << "INSIDE " << runme.we_wordv[0] << " dup2(" << outFD << "," 
      //  << 1 << ")" << endl;
    }
    //otherwise close output pipe fd;
    else{
      if(-1 == close(fd[1])) perror("failed pexec close4");
    }
    
    //cout << "MADE IT TO THE EXECVP" << endl;
    //cout << "EXECUTING ";
    //for(int e=0; e < runme.we_wordc; ++e) cout << runme.we_wordv[e] << " ";
    //cout << endl; 
    
    if(-1 == execvp(runme.we_wordv[0], runme.we_wordv)) perror("runcmd failed");
    wordfree(&runme);

    if(inFD > 0) {
      if(-1 == dup2(savein, infd)) perror("restore stdout failed");
      if(-1 == close(savein)) perror("close savin failed");
    }
    if(outFD > 0){
      if(-1 == dup2(saveout, outfd)) perror("restore stdout failed");
      if(-1 == close(saveout)) perror("close savout failed)");
    }
    cout << "EXECUTE SUCCESS" << endl;
    return 0;
  }
  else{
    int status;
    if(waitpid(pid, &status, 0) != pid){
      perror("wait failed");
    }
    return 0;
  }

}


//piper is the main function to handle pexec calls (forks)
int piper(const string &cmd)
{
  queue<string> cmdq;
  cout << "PIPER" << endl;

  //step1 parse string
  int prev = 0;
  int finder = 0;
  finder = cmd.find("|");
  while(finder != string::npos)
  {
    cmdq.push(cmd.substr(prev, finder-prev));
    prev = finder+1;
    finder = cmd.find("|", finder+1);
  }
  cmdq.push(cmd.substr(prev));

  //step2 setup pipes and such
  int fd[2];
  //waitcount to recall how many processes were spawned
  int waitct = 0;

  string nucmd;
  //cout << "CMDQ SIZE: " << cmdq.size() << endl;
  nucmd = cmdq.front();
  cmdq.pop();
  if(-1 == pipe(fd)) perror("pipe failed");
  //take no inputs
  //write out to fd[1]
  if(0 == pexec(nucmd, 0, fd[1], fd)) waitct++;
  
  int newfd[2];
  while(!cmdq.empty())
  {
    nucmd = cmdq.front();
    cmdq.pop();
    if(-1 == pipe(newfd)) perror("pipe failed");

    if(!cmdq.empty())
    {
      if( 0 == pexec(nucmd, fd[0], newfd[1], newfd)) waitct++;
    }
    else
    {
      if(0 == pexec(nucmd, fd[0], 0, fd)) waitct++; 
    }
    cout << " end loopp!!!!!!! " << endl;
  }

  //step4 wait like a mofo
  cout << "WAITING FOR " << waitct << " PROCESSES TO EXIT" << endl;
  
  return 0;

}

int redexec(string &cmd)
{
  pid_t pid = fork();
  //child
  if(pid == 0)
  {
    size_t index = cmd.find('<');
    wordexp_t wexp; 
    if(index != string::npos)
    {
      //open file, close 0, dup fd;
      string prg = cmd.substr(0, index);
      string file = cmd.substr(index+1);
      while(file[0] == ' ') file.erase(0,1);
      while(file[file.length()-1] == ' ') file.erase(file.length()-1,1);
      getcmd(prg, wexp);
      int fd = open(file.c_str(), O_RDONLY);
      if(fd < 0) perror("open");
      if (close(0) < 0) perror("close");
      if (-1 == (fd = dup(fd))) perror("dup");
      myexec(wexp);
      return 0;
    }
    else
    {
      bool appnd;
      mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
      size_t index = cmd.find('>');
      if(cmd[index+1] == '>') appnd = true;
      string prg = cmd.substr(0, index);
      string file;
      if(appnd) file = cmd.substr(index+2);
      else file = cmd.substr(index+1);
      while(file[0] == ' ') file.erase(0,1);
      while(file[file.length()-1] == ' ') file.erase(file.length()-1,1);
      getcmd(prg, wexp);
      int fd;
      if(appnd) fd = open(file.c_str(), O_WRONLY|O_CREAT|O_APPEND,mode);
      else fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC,mode);
      if(fd < 0) perror("open");
      if (close(1) < 0) perror("close");
      if (-1 == (fd = dup(fd))) perror("dup");
      myexec(wexp);
      return 0;
    }
    return 0;
  }
  //parent
  else if(pid > 0)
  {
    int status;
    if(waitpid(pid, &status, 0) != pid){
      perror("wait failed");
    }
    return 0;
  }
  else perror("fork");
}
    

