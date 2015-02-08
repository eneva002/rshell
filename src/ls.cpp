#include <algorithm>
#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <string>
#include <unistd.h>
#include <iostream>
#include <errno.h>

using namespace std;

bool compare_nocase(const string &first, const string &second){
  unsigned int i = 0;
  while((i<first.length()) && (i<second.length())){
    if (tolower(first[i]) < tolower(second[i])) return true;
    else if (tolower(first[i])>tolower(second[i])) return false;
    ++i;
  }
  return (first.length() < second.length());
}

int main(int argc, char **argv){

  size_t sz = 256;
  char cur[sz]; 
  if(NULL == getcwd(cur, sz)) perror("getcwd failed");
  cout << "current dir: " << cur << endl;

  list<string> files;
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
  files.sort(compare_nocase);

  int aflag = 0;
  int lflag = 0;
  int Rflag = 0;
  char *xpath = NULL;

  int k;
  opterr = 0;

  while((k = getopt(argc, argv, "alR")) != -1){
    switch(k){
      case 'a':
        aflag = 1;
        break;
      case 'l':
        lflag = 1;
        break;
      case 'R':
        lflag = 1;
        break;
      case '?':
        perror("getopt failed");
        return -1;
      default:
        abort();
    }
  }
  
  for(list<string>::iterator i = files.begin(); i != files.end(); ++i){
    if(!aflag){
      while(*((*i).begin()) == '.') ++i;
    }
      
    cout << *i << "  ";
  }
  cout << endl;

  closedir(dirp);

  
  return 0;
}
