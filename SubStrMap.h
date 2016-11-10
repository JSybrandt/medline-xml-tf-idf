#pragma once

#include<string>
#include<unordered_map>
#include<set>

using std::string;
using std::unordered_map;
using std::set;


class SubStrMap {
  public:
  SubStrMap();
  void add(string key, string value);
  unordered_map<string, int> query(string substr);
  bool contains(string substr);

  private:
  struct LetterNode {
      unordered_map<char, LetterNode> children;
      set<string> ids;
    };
  LetterNode root;
  LetterNode* findNode(string substr);
};
