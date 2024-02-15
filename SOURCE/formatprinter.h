#pragma once
#include"tools.h"
#include<iostream>
#include<windows.h>

namespace fprinter
{
	class formatprinter;
}

class fprinter::formatprinter
{
private:
	int width;				//存储控制台宽度
	int height;				//存储控制台高度
	void getConsoleSize();	//获取控制台长宽
public:
	formatprinter();		//构造函数
	void printDash();		//打印'-'
	void printStar();		//打印'*'
	void printFormat(const std::string& text, const char token);	//打印提示语
	template<typename T>
	void scanFormat(T& input);	//打印输入提示框
};

template<typename T>
void fprinter::formatprinter::scanFormat(T& input)
{
	int x, y, ix, iy;

	printDash();
	std::cout << "***";
	getCursorPosition(ix, iy);
	std::cout << std::endl;
	printDash();
	getCursorPosition(x, y);
	setCursorPosition(ix, iy);
	std::cin >> input;
	setCursorPosition(x, y);
	std::cin.get();
}