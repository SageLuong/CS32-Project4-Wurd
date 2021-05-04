#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
	Trie* current = rootPtr;
	deleteNode(current);
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	deleteNode(rootPtr);	//clear old dictionary
	rootPtr = insertNode();	//initialize and empty trie

	ifstream inputDict(dictionaryFile);
	if (!inputDict)
		return false;
	else
	{
		string tempString;
		while (getline(inputDict, tempString))
		{
			insertKeyWord(tempString);	//insert each string as a dictionary entry
		}
		return true;
	}
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	string tempString;
	string testString;

	for (int i = 0; i < word.size(); i++)
	{
		if (isalpha(word.at(i)) || word.at(i) == '\'')	//only append letters and apostrophe
		{
			if(isalpha(word.at(i)) && isupper(word.at(i)))	//convert all letters to lowercase
				word.at(i) = tolower(word.at(i));
			tempString.push_back(word.at(i));
		}
	}
	if (!findWord(tempString))	//word is not found in dictionary
	{
		suggestions.clear();
		int foundSuggestions = 0;
		testString = tempString;

		for (int j = 0; j < testString.size(); j++)	//find spelling suggestions if any
		{
			if (foundSuggestions == max_suggestions)
				break;

			for (int k = 0; k < ALPHABET_SIZE; k++)
			{
				testString = tempString;
				if (k == ALPHABET_SIZE - 1)
					testString.at(j) = '\'';
				else
					testString.at(j) = 'a' + k;	//for all letters in a word, try to replace each letter with every letter of the alphabet and search for this new word in the dictionary
				
				if (findWord(testString))
				{
					suggestions.push_back(testString);	//add new word as a spelling suggestion
					foundSuggestions++;
				}
				if (foundSuggestions == max_suggestions)
					break;
			}
		}	
		return false;
	}
	else
		return true;	//word is found in dictionary, no spelling suggestions
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	int startingPos = 0;
	string tempString;

	for (int i = 0; i < line.size(); i++)
	{
		if (isalpha(line.at(i)) || line.at(i) == '\'')	//only append letters and apostrophe
		{
			tempString.push_back(line.at(i));
			if (i == 0 || (!isalpha(line.at(i - 1)) && line.at(i - 1) != '\''))	//designate starting position for each word
				startingPos = i;
		}
		else   //found a separator so word ends
		{
			if (!findWord(tempString) && !tempString.empty())	//word is not found
			{
				Position misspelledPos = { startingPos, startingPos + tempString.size() - 1 };	//insert position of misspelled word
				problems.push_back(misspelledPos);
				tempString.clear();
				
			}
			else
				tempString.clear();
		}
		int j = i;
		if (j == line.size() - 1 && !findWord(tempString) && !tempString.empty())	//for words with no separators like "apple" or apple in the string "banana_apple"
		{
			Position misspelledPos = { startingPos, startingPos + tempString.size() - 1 };	//insert position of misspelled word
			problems.push_back(misspelledPos);
			tempString.clear();
		}
	}
}

StudentSpellCheck::Trie* StudentSpellCheck::insertNode()
{
	Trie* ptrToNode = new Trie;

	ptrToNode->wordEnded = false;

	for (int i = 0; i < ALPHABET_SIZE; i++)
		ptrToNode->nextNode[i] = nullptr;	//initialize empty pointer array to other tries

	return ptrToNode;
}

void StudentSpellCheck::insertKeyWord(string keyWord)
{
	Trie* trieTraversal = rootPtr;

	for (int i = 0; i < keyWord.size(); i++)
	{
		int nodePos;
		if (keyWord.at(i) == '\'')
			nodePos = 26;
		else
			nodePos = keyWord.at(i) - 'a';

		if (trieTraversal->nextNode[nodePos] == nullptr)	//if a letter of a word is not in the dictionary, insert a new Trie node to represent that letter in the word until the entire word is added
		{
			trieTraversal->nextNode[nodePos] = insertNode();
		}
		trieTraversal = trieTraversal->nextNode[nodePos];	//move to the next letter in the word
	}
	trieTraversal->wordEnded = true;	//designate end of an entry
}

bool StudentSpellCheck::findWord(string keyWord)
{
	Trie* trieTraversal = rootPtr;

	for (int i = 0; i < keyWord.length(); i++)
	{
		int nodePos;
		if (keyWord.at(i) == '\'')
			nodePos = 26;
		else
		{
			keyWord.at(i) = tolower(keyWord.at(i));
			nodePos = keyWord.at(i) - 'a';
		}

		if (trieTraversal->nextNode[nodePos] == nullptr)	//starting at this letter in the word, there are no entries that match this word, so this word is not in the dictionary
		{
			return false;
		}
		trieTraversal = trieTraversal->nextNode[nodePos];	//moves to next letter of the string in earching the dictionary 
	}
	
	if (trieTraversal->wordEnded == true && trieTraversal != nullptr)
		return true;
	else
		return false;
}

void StudentSpellCheck::deleteNode(Trie* cur)
{
	if (cur == nullptr)
		return;

	for (int i = 0; i < ALPHABET_SIZE; i++)
	{
		deleteNode(cur->nextNode[i]);
		delete cur->nextNode[i];
	}
}
