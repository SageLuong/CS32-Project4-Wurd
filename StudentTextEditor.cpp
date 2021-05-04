#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	editorCol = 0;
	editorRow = 0;
	m_strings.push_back("");
	editorIT = m_strings.begin();
}

StudentTextEditor::~StudentTextEditor()
{
	list <string>::iterator it;
	it = m_strings.begin();

	while (!m_strings.empty())	//clear text editor contents
	{
		it = m_strings.erase(it);
	}
}

bool StudentTextEditor::load(std::string file) {
	ifstream input(file);
	if (!input)
		return false;
	else
	{
		reset();	//reset text editor's content and editor position
		m_strings.push_back("");
		editorIT = m_strings.begin();
		list <string>::iterator it = m_strings.begin();
		
		char tempChar;
		while (input.get(tempChar))
		{
			if (tempChar != '\r')	//do not include carriage retunr character
			{
				if (tempChar == '\n')
				{
					m_strings.push_back("");	//start on new line
					it++;
				}
				else
					(*it).push_back(tempChar);	//append each character
			}
		}
		return true;
	}
}

bool StudentTextEditor::save(std::string file) {
	ofstream output(file);
	if (!output)
		return false;
	else
	{
		list <string>::iterator it = m_strings.begin();
		while (it != m_strings.end())
		{
			output << (*it) + '\n' << endl;	//save each line of text and append newline character 
			it++;	//move to next line in text editor
		}
		return true;
	}
}

void StudentTextEditor::reset() {
	list <string>::iterator it = m_strings.begin();

	while (!m_strings.empty())	//clears text editor's content
		it = m_strings.erase(it);

	editorRow = 0;	//reset editing position at top of file
	editorCol = 0;
	editorIT = m_strings.begin();

	getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {	
	int curRow, curCol;
	getPos(curRow, curCol);

	switch (dir)
	{
		case UP:
		{
			if (curRow != 0)
			{
				editorRow--;	//move up 1 row
				editorIT--;
			}
			break;
		}
		case DOWN:
		{
			if (curRow != m_strings.size() - 1)
			{
				editorRow++;	//move down 1 row
				editorIT++;
			}
			break;
		}
		case LEFT:
		{
			if (editorCol != 0)
				editorCol--;	//move left 1 column
			else if (editorCol == 0)
			{
				if (editorRow != 0)
				{
					editorRow--;	//move to the end of the row above
					editorIT--;
					editorCol = (*editorIT).size();
				}
			}
			break;
		}
		case RIGHT:
		{
			if (editorCol != (*editorIT).size())
				editorCol++;	//move to the right 1 column
			else
			{
				if (editorRow != m_strings.size() - 1)
				{
					editorRow++;	//move to the beginning of the row below
					editorIT++;
					editorCol = 0;
				}
			}
			break;
		}
		case HOME:
		{
			editorRow = 0;
			editorCol = 0;
			editorIT = m_strings.begin();
			break;
		}
		case END:
		{
			editorIT = m_strings.end();
			editorIT--;
			editorRow = m_strings.size() - 1;
			editorCol = (*editorIT).size();
			break;
		}
	}
}

void StudentTextEditor::del() {	
	char deletedChar;

	if (editorCol != (*editorIT).size())
	{
		deletedChar = (*editorIT).at(editorCol);
		(*editorIT).erase(editorCol, 1);	//delete char at current editing position
		getUndo()->submit(Undo::Action::DELETE, editorRow, editorCol, deletedChar);	//keep track of deleted char
	}
	else
	{
		if (editorRow != m_strings.size() - 1)
		{
			editorIT++;
			string tempString = (*editorIT);	//copy text from line below
			editorIT = m_strings.erase(editorIT);
			editorIT--;

			(*editorIT) += tempString;	//merge current line with the text from the line below	
			getUndo()->submit(Undo::Action::JOIN, editorRow, editorCol);	//keep track that current line merged with line below
		}
	}

}

void StudentTextEditor::backspace() {	
	char deletedChar;

	if (editorCol > 0)
	{
		deletedChar = (*editorIT).at(editorCol - 1);
		(*editorIT).erase(editorCol - 1, 1);	//delete char just before current editing position
		editorCol--;
		getUndo()->submit(Undo::Action::DELETE, editorRow, editorCol, deletedChar);	//keep track of deleted char
	}
	else
	{
		if (editorRow != 0)
		{
			string tempString = (*editorIT);	//copy text from current line
			editorIT = m_strings.erase(editorIT);
			editorIT--;
			editorRow--;

			editorCol = (*editorIT).size();
			(*editorIT) += tempString;	//merge current line with the text from the line above
			getUndo()->submit(Undo::Action::JOIN, editorRow, editorCol);	//keep track that current line merged with line above
		}
	}
}

void StudentTextEditor::insert(char ch) {	
	if (ch == '\t')
	{
		ch = ' ';
		for (int i = 0; i < 4; i++)
		{
			(*editorIT).insert(editorCol, 1, ch);	//insert four spaces for tab character
			editorCol++;
			getUndo()->submit(Undo::Action::INSERT, editorRow, editorCol, ch);	//keep track of inserted tab
		}
	}
	else
	{
		(*editorIT).insert(editorCol, 1, ch);	//insert char at current editing position
		editorCol += 1;
		getUndo()->submit(Undo::Action::INSERT, editorRow, editorCol, ch);	//keep track of inserted char
	}

	
}

void StudentTextEditor::enter() {	
	if (editorRow == m_strings.size() - 1 && editorCol == (*editorIT).size())
	{
		getUndo()->submit(Undo::Action::SPLIT, editorRow, editorCol);	//keep track of line break
		m_strings.push_back("");
		editorRow++;	//insert new empty line after current row
		editorCol = 0;
		editorIT++;
	}
	else
	{
		getUndo()->submit(Undo::Action::SPLIT, editorRow, editorCol);	//keep track of line break

		string tempString = (*editorIT).substr(editorCol, (*editorIT).size() - editorCol);	//copy text that will be split up
		(*editorIT).erase(editorCol, (*editorIT).size() - editorCol);

		editorIT++;
		m_strings.insert(editorIT, tempString);	//insert new line with the split up text
		editorIT--;
		editorRow++;
		editorCol = 0;
	}
}

void StudentTextEditor::getPos(int& row, int& col) const {	//return current editing/cursor position
	row = editorRow;
	col = editorCol;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {	
	if (startRow < 0 || numRows < 0 || startRow >= m_strings.size())
		return -1;

	list <string>::const_iterator it = m_strings.begin();

	lines.clear();	//clears previous contents
	int lineStartCounter = 0;

	while (lineStartCounter != startRow)	//begin at the designated line in text editor
	{
		lineStartCounter++;
		it++;
	}

	while (lineStartCounter != startRow + numRows)	//retrieve specified number of rows from text editor
	{
		if (it != m_strings.end())
		{
			lines.push_back((*it));
			lineStartCounter++;
			it++;
		}
		else
			lineStartCounter++;
	}

	return lines.size();
}

void StudentTextEditor::undo() {	
	int undoRow, undoCol, undoCount;
	string undoText;
	Undo::Action undoAction = getUndo()->get(undoRow, undoCol, undoCount, undoText);	//get most recent action

	//move current editing position to operation position
	while (editorRow < undoRow)
	{
		editorIT++;
		editorRow++;
	}
	while (editorRow > undoRow)
	{
		editorIT--;
		editorRow--;
	}
	while (editorCol < undoCol)
	{
		editorCol++;
	}
	while (editorCol > undoCol)
	{
		editorCol--;
	}

	if (undoAction == Undo::Action::INSERT)	//undoes a deletion
	{
		(*editorIT).insert(editorCol, undoText, 0, undoText.size());	//insert deleted text
	}
	else if (undoAction == Undo::Action::DELETE)	//undoes an insert
	{
		editorRow = undoRow;
		editorCol = undoCol;
		(*editorIT).erase(undoCol, undoCount);	//deletes inserted text
	}
	else if (undoAction == Undo::Action::SPLIT)	//undoes a merge either from deletion or backspace
	{
		editorRow = undoRow;
		editorCol = undoCol;

		string tempString = (*editorIT).substr(undoCol, (*editorIT).size() - undoCol);	//copies part of text that was merged
		(*editorIT).erase(undoCol, (*editorIT).size() - undoCol);

		editorIT++;
		editorIT = m_strings.insert(editorIT, tempString);	//insert text in a new line below
		editorIT--;	//move editor cursor back to the line above
	}
	else if (undoAction == Undo::Action::JOIN)	//undoes a line break
	{
		editorRow = undoRow;
		editorCol = undoCol;

		editorIT++;	//copy the text in the line below which is the text that was split up
		string tempString = (*editorIT);	
		editorIT = m_strings.erase(editorIT);
		editorIT--;
		(*editorIT) += tempString;	//append split-up text to line above
	}
	else if (undoAction == Undo::Action::ERROR)
	{
		return;
	}
}
