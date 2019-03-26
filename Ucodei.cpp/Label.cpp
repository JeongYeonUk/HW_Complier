#include "Label.h"
#include "BASE.h"
using namespace std;

void Label::addFix(fixUpList *, int)
{
}

void Label::insertLabel(char label[], int value)
{
	fixUpList *ptr;
	int index;
	for (index = 0; (index <= labelCnt) && strcmp(labelTable[index].labelName, label); ++index);
	labelTable[index].address = value;
	if (index > labelCnt)
	{
		strcpy(labelTable[index].labelName, label);
		labelCnt = index;
		labelTable[index].instrList = NULL;
	}
	else
	{
		ptr = labelTable[index].instrList;
		labelTable[index].instrList = NULL;
		while (ptr)
		{
			instrBuf[ptr->instrAddress].value1 = value;
			ptr = ptr->next;
		}
	}
}

void Label::findLabel(char label[], int instr)
{
	fixUpList* ptr;
	int index;
	for (index = 0; (index <= labelCnt) && strcmp(label, labelTable[index].labelName); ++index);
	if (index > labelCnt)
	{
		strcpy(labelTable[index].labelName, label);
		labelCnt = index;
		ptr = new fixUpList;
		if (ptr == NULL) errmsg("findLabel()", "Out of Memory -- new");
		labelTable[index].instrList = ptr;
		ptr->instrAddress = instr;
		ptr->next = NULL;
	}
	else
	{
		ptr = labelTable[index].instrList;
		if (ptr) addFix(ptr, instr);
		else instrBuf[instr].value1 = labelTable[index].address;
	}
}

void Label::checkUndefinedLabel()
{
}

Label::Label()
{
	labelCnt = 2;
	strcpy(labelTable[0].labelName, "read");
	labelTable[0].address = READPROC;
	labelTable[0].instrList = NULL;
	strcpy(labelTable[1].labelName, "write");
	labelTable[1].address = WRITEPROC;
	labelTable[1].instrList = NULL; 
	strcpy(labelTable[2].labelName, "lf");
	labelTable[2].address = LFPROC;
	labelTable[2].instrList = NULL;
	for (int index = 3; index < MAXLABELS; ++index)
		labelTable[index].address = UNDEFINED;
}


Label::~Label()
{
}
