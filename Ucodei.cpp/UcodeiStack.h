#pragma once
class UcodeiStack
{
private:
	int size;
	int sp;
	int* stackArray;
public:
	void push(int);
	int pop();
	int top(){ return sp; }
	void spSet(int n) { sp = n; }
	void dump();
	int& operator[](int);
	UcodeiStack(int);
	~UcodeiStack() { delete[] stackArray; }
		
};

