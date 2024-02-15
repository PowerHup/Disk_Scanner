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
	int width;				//�洢����̨���
	int height;				//�洢����̨�߶�
	void getConsoleSize();	//��ȡ����̨����
public:
	formatprinter();		//���캯��
	void printDash();		//��ӡ'-'
	void printStar();		//��ӡ'*'
	void printFormat(const std::string& text, const char token);	//��ӡ��ʾ��
	template<typename T>
	void scanFormat(T& input);	//��ӡ������ʾ��
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