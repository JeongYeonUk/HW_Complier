#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cctype>
using namespace std;

const int MAXINSTR(2000);
const int MAXLABELS(300);
const int STACKSIZE(20000);
const int LABELSIZE(10);
const int NO_OPCODES(41);

std::ifstream inputFile;
std::ofstream outputFile;

enum opcode
{
	notop, neg, incop, decop, dup, swp, add, sub, mult, divop,
	modop, andop, orop, gt, lt, ge, le, eq, ne,
	lod, ldc, lda, ldi, ldp, str, sti, ujp, tjp, fjp,
	call, ret, retv, chkh, chkl, nop, proc, endop, bgn, sym,
	dump, none
};

const char* opcodeName[NO_OPCODES] = {
	"notop", "neg", "inc", "dec", "dup", "swp", "add", "sub",
	"mult", "div", "mod", "and", "or", "gt", "lt", "ge", "le",
	"eq", "ne", "lod", "ldc", "lda", "ldi", "ldp", "str", "sti",
	"ujp", "tjp", "fjp", "call", "ret", "retv", "chkh", "chkl", "nop",
	"proc", "end", "bgn", "sym", "dump", "none"
};

int executable[NO_OPCODES] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 0,
	1, 0, 0, 0, 1, 0
};

int opcodeCycle[NO_OPCODES] = {
	5, 5, 1, 1, 5, 10, 10, 10,
	50, 100, 100, 10, 10, 20, 20,
	20, 20, 20, 20, 5, 5, 5, 10,
	10, 5, 10, 10, 10, 10, 30, 30,
	30, 5, 5, 0, 30, 0, 0, 0, 100, 0
};

int staticCnt[NO_OPCODES], dynamicCnt[NO_OPCODES];

enum
{
	FALSE,
	TRUE
};

enum procIndex
{
	READPROC = -1,
	WRITEPROC = -2,
	LFPROC = -3,
	UNDEFINED = -1000
};

typedef struct
{
	int opcode;
	int value1;
	int value2;
	int value3;
}Instruction;

Instruction instrBuf[MAXINSTR];

void errmsg(const char* s, const char* s2 = "")
{
	std::cerr << "error !!! " << s << ": " << s2 << "\n";
	exit(1);
}

class UcodeiStack
{
private:
	int size;
	int sp;
	int* stackArray;
public:
	void push(int);
	int pop();
	int top() { return sp; }
	void spSet(int n) { sp = n; }
	void dump();
	int& operator[](int);
	UcodeiStack(int);
	~UcodeiStack() { delete[] stackArray; }
};

void UcodeiStack::push(int value)
{
	if (sp == STACKSIZE) errmsg("push()", "Stack Overflow...");
	stackArray[++sp] = value;
}

int UcodeiStack::pop()
{
	if (sp == 0) errmsg("pop()", "Stack Underflow...");
	return stackArray[sp--];
}

void UcodeiStack::dump()
{
	cout << "stack dump : (address : value)\n";
	for (int i = sp - 10; i <= sp; ++i)
		cout << ' ' << i << " : " << stackArray[i] << '\n';
	cout << '\n';
}

int & UcodeiStack::operator[](int index)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return stackArray[index];
}

UcodeiStack::UcodeiStack(int size)
{
	stackArray = new int[size];
	sp = -1;
	push(-1); push(-1); push(-1); push(0);
	push(0); push(0); push(-1); push(1);
}

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

void Label::addFix(fixUpList * prev, int instr)
{
	fixUpList* succ;

	while (prev->next) prev = prev->next;
	succ = new fixUpList;
	if (succ == NULL) errmsg("addFix()", "Out of memory");
	succ->instrAddress = instr;
	succ->next = NULL;
	succ->next = succ;
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
	for (int index = 0; index <= labelCnt; ++index)
	{
		if (labelTable[index].address == UNDEFINED)
			errmsg("undefined label", labelTable[index].labelName);
	}
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

class Assemble
{
private:
	int instrCnt;
	char lineBuffer[80];
	int bufIndex;
	Label labelProcess;
	char label[LABELSIZE];
	void getLabel();
	int getOpcode();
	int getOperand();
	void instrWrite();
public:
	void assemble();
	int startAddr;
	Assemble() { instrCnt = 0; }
};

void Assemble::getLabel()
{
	int i;
	while (isspace(lineBuffer[bufIndex])) bufIndex++;
	for (i = 0; i <= LABELSIZE && !isspace(label[i] = lineBuffer[bufIndex]); ++bufIndex, ++i);
	label[i] = '\0';
}

int Assemble::getOpcode()
{
	char mnemonic[5];
	int index = 0;
	bufIndex = 11;
	while (index < 5 && !isspace(lineBuffer[bufIndex]))
	{
		mnemonic[index++] = lineBuffer[bufIndex++];
	}
	mnemonic[index] = '\0';

	for (index = notop; index < none; ++index)
	{
		if (!strcmp(mnemonic, opcodeName[index])) break;
	}
	
	if (index == none) errmsg("Illegal opcode", mnemonic);
	return index;
}

int Assemble::getOperand()
{
	int result = 0;
	while (isspace(lineBuffer[bufIndex])) bufIndex++;
	while (isdigit(lineBuffer[bufIndex]) && lineBuffer[bufIndex] != '\n')
		result = 10 * result + (lineBuffer[bufIndex++] - '0');
	return result;
}

void Assemble::instrWrite()
{
	int i, j; char ch;

	inputFile.seekg(0, ios::beg);
	outputFile << "\n\n" << " line		object		ucode source program" << "\n\n";
	for (i = 1; i <= instrCnt; ++i)
	{
		outputFile.width(4);
		outputFile << i << "	(";
		outputFile.width(2);
		outputFile << instrBuf[i].opcode;
		j = instrBuf[i].opcode;
		if (j == chkl || j == chkh || j == ldc || j == bgn || j == ujp || j == call || j == fjp || j == tjp)
		{
			outputFile.width(5);
			outputFile << instrBuf[i].value1;
			outputFile.width(5);
			outputFile << instrBuf[i].value2;
		}
		else
			outputFile << "			";
		outputFile << ")	";
		while ((ch = inputFile.get()) != '\n')
			outputFile.put(ch);
		outputFile.put('\n');
	}
	outputFile << "\n\n	  ****   Result   ****\n\n";
}

void Assemble::assemble()
{
	int done = FALSE;
	int end = FALSE;
	int n;
	cout << " == Assembling ... ==" << '\n';
	while (!inputFile.eof() && !inputFile.fail() && !end)
	{
		instrCnt++; bufIndex = 0;
		inputFile.getline(lineBuffer, sizeof(lineBuffer));
		if (!isspace(lineBuffer[0]))
		{
			getLabel();
			labelProcess.insertLabel(label, instrCnt);
		}
		instrBuf[instrCnt].opcode = n = getOpcode();
		staticCnt[n]++;
		switch (n)
		{
		case chkl:
		case chkh:
		case ldc:
			instrBuf[instrCnt].value1 = getOperand();
			break;
		case lod:
		case str:
		case lda:
		case sym:
			instrBuf[instrCnt].value1 = getOperand();
			instrBuf[instrCnt].value2 = getOperand();
			break;
		case proc:
			instrBuf[instrCnt].value1 = getOperand();
			instrBuf[instrCnt].value2 = getOperand();
			instrBuf[instrCnt].value3 = getOperand();
			break;
		case bgn:
			instrBuf[instrCnt].value1 = getOperand();
			startAddr = instrCnt;
			done = TRUE;
			break;
		case ujp:
		case call:
		case fjp:
		case tjp:
			getLabel();
			labelProcess.findLabel(label, instrCnt);
			break;
		case endop:
			if (done) end = TRUE;
		default:
			break;
		}
	}
	labelProcess.checkUndefinedLabel();
	instrWrite();
}

class Interpret
{
private:
	UcodeiStack stack;
	int arBase;
	long int tcycle;
	long int exeCount;
	void predefinedProc(int);
	int findAddr(int);
	void statistic();
public:
	void execute(int);
	Interpret();
	virtual ~Interpret() {}
};

void Interpret::predefinedProc(int)
{
}

int Interpret::findAddr(int)
{
	return 0;
}

void Interpret::statistic()
{
}

void Interpret::execute(int)
{
}

Interpret::Interpret()
	:stack(STACKSIZE)
{

}
