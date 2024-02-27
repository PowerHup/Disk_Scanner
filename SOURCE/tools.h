#pragma once
#include<string>
#include<tchar.h>
#include<vector>
#include<windows.h>

const int MAXLEN = 300;

std::string tc2s(const TCHAR* str);			//TCHAR转string
void s2tc(std::string str, TCHAR tstr[]);	//string转TCHAR
time_t ft2t(FILETIME ft);					//FILETIME转UTC时间
void strSplit(std::string& str, std::vector<std::string>& strs, char token);  //字符串分割
void getCursorPosition(int& x, int& y);		//获取当前光标位置
void setCursorPosition(int x, int y);		//前往指定光标位置
void clearConsole();						//清空控制台