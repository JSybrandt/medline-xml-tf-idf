
#include<iostream>
#include<dirent.h>
#include<vector>
#include<unordered_map>
#include<fstream>
#include<regex>
#include<algorithm>
#include<functional>
#include<cctype>
#include<locale>
#include<sstream>
#include <sys/wait.h>
#include"omp.h"
#include "constants.h"

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

//Returns PMID, AbstractText pairs
//second param are all the values we already have
map<string,string> parseXML(string fileName, const unordered_map<string,string>& backup){

    map<string,string> pmid2abstract;
    regex abstractRegex(ABSTRACT_REGEX,regex_constants::ECMAScript);
    regex pmidRegex(PMID_REGEX,regex_constants::ECMAScript);
    fstream fin(fileName, ios::in);
    string line;
    string lastFoundPMID = "NULL";
    string lastFoundAbstract = "";

    while(getline(fin,line)){
        if(regex_search(line,pmidRegex)){
            lastFoundAbstract = trim(lastFoundAbstract);
            if(backup.find(lastFoundPMID) == backup.end() && lastFoundPMID != "NULL" && lastFoundAbstract != ""){
                pmid2abstract[lastFoundPMID] = lastFoundAbstract;
            }
            lastFoundPMID = regex_replace(line,pmidRegex,"");
            lastFoundAbstract = "";
        }
        if(regex_search(line,abstractRegex)){
            lastFoundAbstract += regex_replace(line,abstractRegex,"") + " ";
        }
    }
    //Need extra check at end
    if(lastFoundPMID != "NULL" && lastFoundAbstract != "")
        pmid2abstract[lastFoundPMID] = lastFoundAbstract;

    fin.close();
    return pmid2abstract;
}

void parseMedline(unordered_map<string,string>& pmid2abstract, string dirPath, fstream& lout){
    vector<string> xmlPaths = getFilesInDir(dirPath);
    int completeCount = 0;
#pragma omp parallel  for
    for(int i = 0 ; i < xmlPaths.size();i++){

#pragma omp critical (LOGGER)
      lout << "Parsing:" << xmlPaths[i] << endl;

        map<string,string> tmp = parseXML(xmlPaths[i],pmid2abstract);

        #pragma omp critical (INSERT_ABSTRACT)
            pmid2abstract.insert(tmp.begin(),tmp.end());
        }

        #pragma omp critical (LOGGER)
        {
            completeCount++;
            lout << (completeCount / (double) xmlPaths.size()) * 100 << "%" << endl;
        }
    }

}

void loadOldCanon(unordered_map<string,string>& pmid2canon){
    vector<string> xmlPaths = getFilesInDir(BACKUP_FILES_DIR);

    #pragma omp parallel  for
    for(int i = 0 ; i < xmlPaths.size();i++){
        unordered_map<string,string> tmpMap;
        fstream res(xmlPaths[i].c_str(), ios::in);
        string line;
        while(getline(res,line)){
            stringstream s;
            s << line;
            string pmid;
            s >> pmid;
            string data = s.str();
            tmpMap[pmid] = data;
        }
#pragma omp critical (LOAD_CANON_LOCK)
        pmid2canon.insert(tmpMap.begin(),tmpMap.end());

        res.close();
    }
}

int main(int argc, char** argv) {

    fstream lout(LOG_FILE,ios::out);
    lout<<"Started"<<endl;

    unordered_map<string,string> pmid2abstract;
    unordered_map<string,int> globalWordCount;
    //pmid -> (keyword -> count)
    unordered_map<string, unordered_map<string,int> > documentCounts;
    vector<string> pmids;
    unordered_map<string, int> multiWordFlag;

    parseMedline(pmid2abstract, MEDLINE_XML_DIR, lout);

    fstream keyIn(KEYWORD_FILE, ios::in);
    string line;
    for(getline(keyIn, line)){
      line = trim(line);
      //if multi-word key
      if(std::count(line.begin(),line.end(),' ') > 0){
        string word = line.substring(0,line.find(' '));
        if(multiWordFlag[word] < line.size())
          multiWordFlag[word] = line.size();
      }
      globalWordCount[line] = 0;
    }
    keyIn.close();

#pragma omp parallel for
    for(auto abstractRecord : pmid2abstract){
      unordered_map<string, int> docCount;
      stringstream s;
      s << abstractRecord.second;
      string word;
      while(s >> word){
        //if we possibly have a multi-word
        if(multiWordMap.find(word) != multiWordMap.end()){
          string copy = s.str();
          stringstream tmpStream;
          tmpStream << copy;
          string searchTerm = word;
          string tmpString;
          while(searchTerm.size() < multiWordFlag[word] && tmpStream >> tmpString){
            searchTerm += " " + tmpString;
            if(documentCounts.find(searchTerm) != documentCounts.end()){
              documentCounts[searchTerm]++;
            }

          }
        }
        //if we have a keyword
        if(documentCounts.find(word)!= documentCounts.end()){
          documentCounts[word]++;
          docCount[word]++;
        }
      }
    }

    lout.close();
    return 0;
}

