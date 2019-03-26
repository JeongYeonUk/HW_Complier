#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cctype>

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