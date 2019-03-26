#pragma once

class Label
{
private:
	typedef struct fixUpList
	{
		int instrAddress;
		fixUpList *next;
	}fixUpList;
	typedef struct labelEntry
	{
		char labelName[10];
		int address;
		fixUpList *instrList;
	}labelEntry;
	labelEntry labelTable[300];
	int labelCnt;
	void addFix(fixUpList*, int);
public:
	void insertLabel(char[], int);
	void findLabel(char[], int);
	void checkUndefinedLabel();
	Label();
	virtual ~Label();
};

