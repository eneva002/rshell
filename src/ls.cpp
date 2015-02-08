#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv){
  size_t sz = 256;
  char cur[sz]; 
  getcwd(cur, sz);

  cout << "current dir: " << cur << endl;

  dirent *CUR;
  
  return 0;
}
