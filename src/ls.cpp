#include <algorithm>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <pwd.h>
#include <vector>
#include <list>
#include <string>
#include <unistd.h>
#include <iostream>
#include <grp.h>
#include <errno.h>
#include <iomanip>
#include <queue>

using namespace std;

bool compare_nocase(const string &first, const string &second)
{
  unsigned int i = 0;
  while((i<first.length()) && (i<second.length())){
    if (tolower(first[i]) < tolower(second[i])) return true;
    else if (tolower(first[i])>tolower(second[i])) return false;
    ++i;
  }
  return (first.length() < second.length());
}

int info(const string &dir)
{
  struct stat inf;
  if(-1 == stat(dir.c_str(), &inf)){
    perror("stat failed");
    return -1;
  }
  cout << left;

  int temp = inf.st_mode & S_IFMT;
  switch(temp){
    case S_IFDIR:
      cout << 'd'; break;
    case S_IFREG:
      cout << '-'; break;
    case S_IFLNK:
      cout << 'l'; break;
    default:
      cout << '*'; break;
  }
  temp = (inf.st_mode & S_IRUSR); if(temp) cout << 'r'; else cout << '-'; 
  temp = (inf.st_mode & S_IWUSR); if(temp) cout << 'w'; else cout << '-'; 
  temp = (inf.st_mode & S_IXUSR); if(temp) cout << 'x'; else cout << '-'; 
  temp = (inf.st_mode & S_IRGRP); if(temp) cout << 'r'; else cout << '-'; 
  temp = (inf.st_mode & S_IWGRP); if(temp) cout << 'w'; else cout << '-'; 
  temp = (inf.st_mode & S_IXGRP); if(temp) cout << 'x'; else cout << '-';
  temp = (inf.st_mode & S_IROTH); if(temp) cout << 'r'; else cout << '-'; 
  temp = (inf.st_mode & S_IWOTH); if(temp) cout << 'w'; else cout << '-'; 
  temp = (inf.st_mode & S_IXOTH); if(temp) cout << 'x'; else cout << '-';
  cout << right << ' '; 
  cout << inf.st_nlink; 
  struct passwd *pwd;
  if(NULL == (pwd = getpwuid(inf.st_uid))) perror("getpwuid failed");
  cout << ' ' << pwd->pw_name;

  struct group *grp;
  if(NULL == (grp = getgrgid(inf.st_gid))) perror("getgrgid failed");
  cout << ' ' << grp->gr_name;

  cout << setw(7);
  cout << inf.st_size;

  time_t raw = inf.st_mtime;
  struct tm *timeinfo;
  char buf[80];
  timeinfo = localtime(&raw);

  strftime(buf, 80, "%b %e %R", timeinfo);
  cout << ' ' << buf;

  cout << ' ' << dir << endl;
  return 0;
}

int main(int argc, char **argv)
{

  //parse options, valid options are -a -l and -R
  int aflag = 0;
  int lflag = 0;
  int Rflag = 0;

  //this is to hold directories with the -R flag
  char *xpath = NULL;

  //return values from the getopt function
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
        Rflag = 1;
        break;
      case '?':
        cout << "invalid option: " << (char)optopt << endl;
        return -1;
      default:
        abort();
    }
  }

  queue<string> paths;
  int idx = optind;

  //read current directory
  size_t sz = 256;
  char cur[sz]; 
  if(NULL == getcwd(cur, sz)) perror("getcwd failed");

  string curd = ".";

  if(idx != argc){
    for(; idx < argc; ++idx){
      paths.push(string(argv[idx]));
    }
  }
  else{
        paths.push(curd);
  }

  //file list
  list<string> files;
  char* temp;
  string cr;
  bool mult = false;

  if(paths.size() > 1 || Rflag) mult = true; 
  while(!paths.empty()){
    //cr == current file
    cr = paths.front();
    paths.pop();
    DIR *dirp;
    if((dirp = opendir(cr.c_str())) == NULL) perror("opendir failed");

    //populate file list
    dirent *direntp;
    errno = 0;

    while((direntp = readdir(dirp))){
      temp = direntp->d_name;
      files.push_back(string(temp));
    }
    if(errno != 0) {
      perror("readdir failed"); 
      if(-1 == closedir(dirp)) perror("closedir failed");
      return -1; 
    }

    

    //sort file list
    files.sort(compare_nocase);

    if(mult) cout <<  cr << ':' << endl;
    for(list<string>::iterator i = files.begin(); i != files.end(); ++i){
      if(!aflag){
        while(*((*i).begin()) == '.') ++i;
      }
      if(lflag){
        int j = info(*i);   
        if(-1 == j){
          if(-1 == closedir(dirp)) perror("closedir failed");
          return -1;
        }
      }
      else cout << *i << "  ";
      // RFLAG GOES HERE
      if(Rflag ){
        struct stat inf;
        string wtfamidoing;
        if(cr[0] != '/')  wtfamidoing = cr + '/' + *i;
        else wtfamidoing = cr + *i;
        if(-1 == stat(wtfamidoing.c_str(), &inf)){
          perror("stat failed");
          return -1;
        }
        if(*i != "." && *i != ".."){
          if((inf.st_mode & S_IFDIR) == S_IFDIR){
            paths.push(cr+'/'+(*i)); 
          }
        }
      }
    }
    if(!lflag) cout << endl;
    if(mult) cout << endl;

    files.clear();
    if(-1 == closedir(dirp)) perror("closedir failed");
  }

  
  return 0;
}
