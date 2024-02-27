#include"tools.h"
#include<sstream>
#include<vector>

using std::string;
using std::vector;
using std::istringstream;

/*TCHAR转string函数*/
string tc2s(const TCHAR* str)
{
    _tsetlocale(LC_ALL, _T("chs"));     //添加设置，否则无法输出中文

    string astr;

    int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
    char* chRtn = new char[iLen * sizeof(char)];
    WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);
    astr = chRtn;

    return astr;
}

/*string转TCHAR函数*/
void s2tc(std::string str, TCHAR tstr[])
{
    _tsetlocale(LC_ALL, _T("chs"));     //添加设置，否则无法输出中文

    _stprintf_s(tstr, MAXLEN, _T("%s"), _tcsdup(std::wstring(str.begin(), str.end()).c_str()));
}

/*FILETIME转UTC时间*/
time_t ft2t(FILETIME  ft)
{
    ULONGLONG  ull;

    ULARGE_INTEGER ui = { 0 };
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;

    ull = (static_cast<ULONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;

    return (static_cast<time_t>(ui.QuadPart - 116444736000000000) / 10000000);
}

/*字符串分割*/
void strSplit(string& str, vector<string>& strs, char token) 
{
    istringstream iss(str);

    string temp;
    while (getline(iss, temp, token))
        strs.push_back(temp);
}

/*获取当前光标位置*/
void getCursorPosition(int& x, int& y)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    x = csbi.dwCursorPosition.X;
    y = csbi.dwCursorPosition.Y;

    return;
}

/*前往指定光标位置*/
void setCursorPosition(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/*清空控制台*/
void clearConsole()
{
    COORD topLeft = { 0, 0 };
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(console, &csbi);
    FillConsoleOutputCharacter(console, ' ', csbi.dwSize.X * csbi.dwSize.Y, topLeft, &written);
    FillConsoleOutputAttribute(console, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, topLeft, &written);
    SetConsoleCursorPosition(console, topLeft);
}