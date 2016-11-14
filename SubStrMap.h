#pragma once

#include<string>
#include<unordered_map>
#include<set>
#include<memory>
#include<sstream>
#include <algorithm>

using std::stringstream;
using std::string;
using std::wstring;
using std::unordered_map;
using std::set;
using std::shared_ptr;
using std::transform;

class SubStrMap;

struct WordNode {
    unordered_map<string, shared_ptr<WordNode> > children;
    set<string> ids;
	bool contains(string word) { return children.find(word) != children.end(); }
};

class SubStrMap {
  public:
  SubStrMap();
  void add(string key, string value);
  unordered_map<string, int> query(string substr);

  private:
  shared_ptr<WordNode> root;
  shared_ptr<WordNode> traverseWord(shared_ptr<WordNode> source, string word);
  string toLowerStripPunct(string str);
};
