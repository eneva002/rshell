#include <algorithm>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <iostream>
#include <errno.h>

using namespace std;

int main(int argc, char **argv){

  size_t sz = 256;
  char cur[sz]; 
  if(NULL == getcwd(cur, sz)) perror("getcwd failed");
  cout << "current dir: " << cur << endl;

  vector<string> files;
  char* temp;

  DIR *dirp;
  if((dirp = opendir(cur)) == NULL) perror("opendir failed");

  dirent *direntp;
  while((direntp = readdir(dirp))){
    errno = 0;
    temp = direntp->d_name;
    files.push_back(string(temp));
    if(errno != 0) { perror("readdir failed"); closedir(dirp); return -1; }
  }

  sort(files.begin(), files.end());
  for(int i = 0; i < files.size(); ++i){
    cout << files[i] << endl;
  }

  closedir(dirp);

  
  return 0;
}
