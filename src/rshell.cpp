#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char** argv){
   int pid = fork();
   if(pid == -1){
      perror("fork");
      exit(1);
   }

   else if(pid == 0){
       cout << "This is the child process\n ";
       char temp;
       cin >> temp;
       exit(1);
   }
   
   else if(pid > 0){
      if( -1 == wait(0)) perror("there was an error with wait().");
   }

}

