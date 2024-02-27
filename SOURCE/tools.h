#pragma once
#include<string>
#include<tchar.h>
#include<vector>
#include<windows.h>

const int MAXLEN = 300;

std::string tc2s(const TCHAR* str);			//TCHARתstring
void s2tc(std::string str, TCHAR tstr[]);	//stringתTCHAR
time_t ft2t(FILETIME ft);					//FILETIMEתUTCʱ��
void strSplit(std::string& str, std::vector<std::string>& strs, char token);  //�ַ����ָ�
void getCursorPosition(int& x, int& y);		//��ȡ��ǰ���λ��
void setCursorPosition(int x, int y);		//ǰ��ָ�����λ��
void clearConsole();						//��տ���̨