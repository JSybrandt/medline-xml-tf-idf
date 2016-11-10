#include "SubStrMap.h"

SubStrMap::SubStrMap()
{
  root = shared_ptr<LetterNode>(&rootNode);
}

void SubStrMap::add(string key, string value)
{
	shared_ptr<LetterNode> currNode = this->root;
	for (char c : key) {
    if(currNode->children.find(c) == currNode->children.end())
      currNode->children[c] = shared_ptr<LetterNode>(new LetterNode);
		currNode = currNode->children[c];
	}
	currNode->ids.insert(value);
}

unordered_map<string,int> SubStrMap::query(string queryString)
{
	unordered_map<string,int> res;

	while (queryString.size() > 0) {
		set<string> ids;
	  shared_ptr<LetterNode> currNode = this->root;
		for (char c : queryString) {
			//if currNode has a valid child
			if (currNode->children.find(c) != currNode->children.end()) {
				//traverse
				currNode = currNode->children[c];
				ids.insert(currNode->ids.begin(), currNode->ids.end());
			}
			else {//no match
				break;
			}
		}
		for (string id : ids)
			res[id]++;
		queryString = queryString.substr(1);
	}
	return res;
}

