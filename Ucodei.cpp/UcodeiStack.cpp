#include "UcodeiStack.h"
#include "BASE.h"
using namespace std;


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
