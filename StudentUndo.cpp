#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {	
	if (undoStack.empty())
	{
		if(action == Action::DELETE)	//add the deleted char to string
			undoStack.push(new toUndo(action, row, col, "" + ch, 1));
		else   //every other action
			undoStack.push(new toUndo(action, row, col, "", 1));
	}
	else
	{
		if (action == Action::INSERT && undoStack.top()->getAction() == Action::INSERT
			&& row == undoStack.top()->getRow() && col == undoStack.top()->getCol() + undoStack.top()->getCount())	//if continuous/batched insert
		{
			if (ch == ' ')	//make a new insert entry for a new word
				undoStack.push(new toUndo(action, row, col, "", 1));
			else
				undoStack.top()->incCount(1);	//inc count of batched deleted chars
		}
		else if (action == Action::INSERT)	//not batched inserts
		{
			undoStack.push(new toUndo(action, row, col, "", 1));
		}

		if (action == Action::DELETE && undoStack.top()->getAction() == Action::DELETE 
			&& row == undoStack.top()->getRow() && col == undoStack.top()->getCol())	//if continuous/batched deletes
		{
			undoStack.top()->setStr(undoStack.top()->getStr() + ch);	//append newly deleted char to end of string
		}
		else if (action == Action::DELETE && undoStack.top()->getAction() == Action::DELETE 
			&& row == undoStack.top()->getRow() && col == undoStack.top()->getCol() - undoStack.top()->getCount())	//if continuous/batched backspace
		{
			std::string someString = "";
			someString += ch;
			undoStack.top()->setStr(undoStack.top()->getStr().insert(0, someString));
			undoStack.top()->setCol(-1);
		}
		else if (action == Action::DELETE)	//not batched delete or backspace
		{
			std::string someStr = "";
			someStr += ch;
			undoStack.push(new toUndo(action, row, col, someStr, 1));
		}

		if (action == Action::SPLIT || action == Action::JOIN)
			undoStack.push(new toUndo(action, row, col, "", 1));
	}
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {	
	Action actionHolder;

	if (undoStack.empty())
		return ERROR;

	if (undoStack.top()->getAction() == INSERT)	//determine undo action
		actionHolder = Action::DELETE;
	else if (undoStack.top()->getAction() == DELETE)
		actionHolder = Action::INSERT;
	else if (undoStack.top()->getAction() == SPLIT)
		actionHolder = Action::JOIN;
	else if (undoStack.top()->getAction() == JOIN)
		actionHolder = Action::SPLIT;
	else
		return Action::ERROR;

	switch (actionHolder)	//set new editor position
	{
		case INSERT:
		case JOIN:
		case SPLIT:
		{
			row = undoStack.top()->getRow();
			col = undoStack.top()->getCol();
			break;
		}
		case DELETE:
		{
			row = undoStack.top()->getRow();
			col = undoStack.top()->getCol() - 1;	//starting position where inserted chars need to be deleted
			break;
		}
		case ERROR:
			break;
	}

	switch (actionHolder)	//determine number of chars affected
	{
		case INSERT:
		case JOIN:
		case SPLIT:
			count = 1;
			break;
		case DELETE:
			count = undoStack.top()->getCount();	
			break;
		case ERROR:
			break;
	}

	switch (actionHolder)	//determine replacement text
	{
	case DELETE:
	case JOIN:
	case SPLIT:
		text.clear();
		break;
	case INSERT:
		text = undoStack.top()->getStr();
		break;	
	case ERROR:
		break;
	}

	undoStack.pop();	//remove undo operation from undo stack

	if (actionHolder == INSERT)
		return Action::INSERT;
	else if (actionHolder == DELETE)
		return Action::DELETE;
	else if (actionHolder == SPLIT)
		return Action::SPLIT;
	else if (actionHolder == JOIN)
		return Action::JOIN;
	else
		return Action::ERROR;
}

void StudentUndo::clear() {
	while (!undoStack.empty())
	{
		delete (undoStack.top());
		undoStack.pop();
	}
}
