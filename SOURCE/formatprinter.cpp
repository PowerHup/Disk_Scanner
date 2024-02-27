#include"formatprinter.h"
#include<algorithm>
#include<iterator>
#include<windows.h>

using namespace fprinter;
using std::string;
using std::cout;
using std::endl;
using std::fill_n;
using std::ostream_iterator;

/*��ȡ����̨����*/
void formatprinter::getConsoleSize()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

/*���캯��*/
formatprinter::formatprinter()
{
	width = 0;
	height = 0;
}

/*��ӡ'-'*/
void formatprinter::printDash()
{
	getConsoleSize();
	fill_n(ostream_iterator<char>(cout), width, '-');
	cout << endl;
}

/*��ӡ'*'*/
void formatprinter::printStar()
{
	getConsoleSize();
	fill_n(ostream_iterator<char>(cout), width, '*');
	cout << endl;
}

/*������ʾ���װ�η�����ӡ��ʾ��*/
void formatprinter::printFormat(const string& text, const char token)
{
	int count;

	getConsoleSize();
	count = (width - static_cast<int>(text.length())) / 2;
	fill_n(ostream_iterator<char>(cout), count, token);
	cout << text;
	fill_n(ostream_iterator<char>(cout), width - count - text.length(), token);
	cout << endl;
}