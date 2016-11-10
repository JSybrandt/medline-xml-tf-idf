#pragma once

#include<string>
#include<unordered_map>
#include<set>
#include<memory>

using std::string;
using std::unordered_map;
using std::set;
using std::shared_ptr;

class SubStrMap;

struct LetterNode {
    unordered_map<char, shared_ptr<LetterNode> > children;
    set<string> ids;
};

class SubStrMap {
  public:
  SubStrMap();
  void add(string key, string value);
  unordered_map<string, int> query(string substr);

  private:
  LetterNode rootNode;
  shared_ptr<LetterNode> root;
};
