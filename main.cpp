#include <iostream>
#include <dirent.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <regex>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <sys/wait.h>
#include "omp.h"
#include "constants.h"
#include "SubStrMap.h"

using namespace std;

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}


vector<string> getFilesInDir(string dirPath){
    vector<string> res;
    DIR *dir;
    dirent *ent;
    if((dir = opendir(dirPath.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
            string name = ent->d_name;
            if(name != "." && name != "..")
                res.push_back(dirPath + "/" + ent->d_name);
        }
        closedir(dir);
    }else{
        cerr << "FAILED TO OPEN " << dirPath << endl;
    }
    return res;
}

string getResFile(){
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
  return RESULTS_DIR + "/" +  string(buffer);
}

//Returns PMID, AbstractText pairs
//second param are all the values we already have
void parseXML(string fileName, SubStrMap& ssm){

    regex abstractRegex(ABSTRACT_REGEX,regex_constants::ECMAScript);
    regex pmidRegex(PMID_REGEX,regex_constants::ECMAScript);
    fstream fin(fileName, ios::in);
    string line;
    string lastFoundPMID = "NULL";
    string lastFoundAbstract = "";

    fstream res(getResFile().c_str(),ios::out);

    while(getline(fin,line)){
        if(regex_search(line,pmidRegex)){
            lastFoundAbstract = trim(lastFoundAbstract);
            if(lastFoundPMID != "NULL" && lastFoundAbstract != ""){
                res << lastFoundPMID << " ";
                for(auto pair : ssm.query(lastFoundAbstract)){
                  res << pair.first << " " << pair.second << " ";
                }
                res << endl;
                //pmid2abstract[lastFoundPMID] = lastFoundAbstract;
            }
            lastFoundPMID = regex_replace(line,pmidRegex,"");
            lastFoundAbstract = "";
        }
        if(regex_search(line,abstractRegex)){
            lastFoundAbstract += regex_replace(line,abstractRegex,"") + " ";
        }
    }
    //Need extra check at end
    if(lastFoundPMID != "NULL" && lastFoundAbstract != ""){
        res << lastFoundPMID << " ";
        for(auto pair : ssm.query(lastFoundAbstract)){
          res << pair.first << " " << pair.second << " ";
        }
        res << endl;
    }

    res.close();
    fin.close();
}

void parseMedline(SubStrMap& ssm, string dirPath, fstream& lout){
    vector<string> xmlPaths = getFilesInDir(dirPath);
    int completeCount = 0;
#pragma omp parallel  for
    for(int i = 0 ; i < xmlPaths.size();i++){

#pragma omp critical (LOGGER)
      lout << "Parsing:" << xmlPaths[i] << endl;

        parseXML(xmlPaths[i],ssm);

        #pragma omp critical (LOGGER)
        {
            completeCount++;
            lout << (completeCount / (double) xmlPaths.size()) * 100 << "%" << endl;
        }
    }

}

void getWord2ID(SubStrMap& ssm){
  int numRows;
  string id;
  fstream reader(KEYWORD_FILE, ios::in);
  while(reader >> id >> numRows){
    cin.ignore();
    for(int i = 0 ; i < numRows; i++){
      string line;
      getline(reader,line);
      ssm.add(line,id);
    }
  }
  reader.close();
}

int main(int argc, char** argv) {

    fstream lout(LOG_FILE,ios::out);

    lout<<"Started"<<endl;

    SubStrMap ssm;
    getWord2ID(ssm);

    parseMedline(ssm, MEDLINE_XML_DIR, lout);


    lout.close();
    return 0;
}

