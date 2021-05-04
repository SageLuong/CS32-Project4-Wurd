#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	class toUndo
	{
	public:
		toUndo(Action action, int row, int col, std::string str = "", int count = 1)
			:m_action(action), m_row(row), m_col(col), m_string(str), m_countChar(count) {}
		Action getAction() { return m_action; }
		int getRow() { return m_row; }
		int getCol() { return m_col; }
		void setCol(int decrement) { m_col += decrement; }
		std::string getStr() { return m_string; }
		void setStr(std::string str) { m_string = str; }
		int getCount() { return m_countChar; }
		void incCount(int count) { m_countChar += count; }
	private:
		Action m_action;
		int m_row;
		int m_col;
		std::string m_string;
		int m_countChar;
	};
	std::stack<toUndo*> undoStack;
};

#endif // STUDENTUNDO_H_
