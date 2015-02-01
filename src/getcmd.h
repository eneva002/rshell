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

  ifstream fin("tests.txt");
  if(!fin.is_open()){
    cout << "file failed to open" << endl;
    exit(1);
  }

  string temp;
  int i = 0;
  
  while(getline(fin, temp)){ 
    if(temp.length() > 0 && temp.at(0) != '#'){
      char *temp2 = new char [temp.length()+1];
      std::strcpy(temp2, temp.c_str());

      cout << "========== line " << ++i << " ==========\n";
      char *token = strtok(temp2, "#"); 
      cout <<  token << endl;
      
      delete temp2;
      cout << "-----------------------------\n\n";
    }
  }
}
