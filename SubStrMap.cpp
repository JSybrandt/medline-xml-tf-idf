#include "SubStrMap.h"

SubStrMap::SubStrMap()
{
}

void SubStrMap::add(string key, string value)
{
	LetterNode* currNode = &(this->root);
	for (char c : key) {
		currNode = &(currNode->children[c]);
	}
	currNode->ids.insert(value);
}

unordered_map<string,int> SubStrMap::query(string queryString)
{
	unordered_map<string,int> res;

	while (queryString.size() > 0) {
		set<string> ids;
		LetterNode* currNode = &(this->root);
		for (char c : queryString) {
			//if currNode has a valid child
			if (currNode->children.find(c) != currNode->children.end()) {
				//traverse
				currNode = &(currNode->children[c]);
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

bool SubStrMap::contains(string substr)
{
	return (findNode(substr) != nullptr);
}

SubStrMap::LetterNode* SubStrMap::findNode(string substr)
{
	LetterNode* currNode = &(this->root);
	for (char c : substr) {
		//if currNode has a valid child
		if (currNode->children.find(c) != currNode->children.end()) {
			//traverse
			currNode = &(currNode->children[c]);
		}
		else {//no match
			return nullptr;
		}

	}
	return currNode;
}