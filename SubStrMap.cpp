#include "SubStrMap.h"
#include<iostream>

SubStrMap::SubStrMap()
{
  root = shared_ptr<WordNode>(new WordNode);
}

void SubStrMap::add(string key, string value)
{
	value = toLowerStripPunct(value);

	shared_ptr<WordNode> currNode = this->root;
	stringstream s;
	s << key;
	string word;
	
	while (s >> word) {
		if (currNode->contains(word))
			currNode = traverseWord(currNode, word);
		else {
			currNode->children[word] = shared_ptr<WordNode>(new WordNode);
			currNode = currNode->children[word];
		}
	}
	currNode->ids.insert(value);
}

unordered_map<string,int> SubStrMap::query(string queryString)
{
	queryString = toLowerStripPunct(queryString);

	unordered_map<string, int> res;
	stringstream s;
	s << queryString << " $"; //add terminator
	shared_ptr<WordNode> currNode = root;
	shared_ptr<WordNode> nextNode;
	string word;

	while (s >> word) {
		nextNode = traverseWord(currNode, word);
		if (nextNode == nullptr) {
			if (currNode != root) {
				for (string s : currNode->ids) {
					res[s]++;
				}
			}
			nextNode = traverseWord(root,word);
			if (nextNode == nullptr) nextNode = root;
		}
		currNode = nextNode;
	}

	return res;
}

shared_ptr<WordNode> SubStrMap::traverseWord(shared_ptr<WordNode> source, string word){
	if(source != nullptr && source->contains(word))
		return source->children[word]; 
    else
		return shared_ptr<WordNode>();
}

string SubStrMap::toLowerStripPunct(string str)
{
	for (char& c : str) {
		if (ispunct(c))
			c = ' ';
		else
			c = tolower(c);

	}
	return str;
}
