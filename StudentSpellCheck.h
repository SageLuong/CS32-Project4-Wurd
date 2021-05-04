#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

const int ALPHABET_SIZE = 27;	//all leters plus apostrophe

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() { }
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct Trie
	{
		Trie* nextNode[ALPHABET_SIZE];
		bool wordEnded;
	};
	Trie* rootPtr;
	Trie* insertNode();
	void insertKeyWord(std::string keyWord);
	bool findWord(std::string keyWord);
	void deleteNode(Trie* cur);

};

#endif  // STUDENTSPELLCHECK_H_
