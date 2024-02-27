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

/*获取控制台长宽*/
void formatprinter::getConsoleSize()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

/*构造函数*/
formatprinter::formatprinter()
{
	width = 0;
	height = 0;
}

/*打印'-'*/
void formatprinter::printDash()
{
	getConsoleSize();
	fill_n(ostream_iterator<char>(cout), width, '-');
	cout << endl;
}

/*打印'*'*/
void formatprinter::printStar()
{
	getConsoleSize();
	fill_n(ostream_iterator<char>(cout), width, '*');
	cout << endl;
}

/*传入提示语和装饰符，打印提示语*/
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