#include"diskoperate.h"
#include"formatprinter.h"
#include"tools.h"
#include<fstream>
#include<iostream>
#include<string>
#include<tchar.h>

using namespace diskop;
using namespace fprinter;
using std::fstream;
using std::string;
using std::getline;
using std::cout;
using std::cin;
using std::endl;

int _tmain(int argc, TCHAR* argv[])
{
	_tsetlocale(LC_ALL, _T("chs"));	//设置字符环境，否则无法输入中文

	disk myDisk;				//磁盘类对象
	formatprinter printer;		//辅助输出的对象
	TCHAR rootPath[MAX_LENGTH];	//扫描的根路径
	TCHAR tDirPath[MAX_LENGTH]; //存储TCHAR格式目录路径名
	TCHAR tFilePath[MAX_LENGTH];//存储TCHAR格式文件路径名
	string strRoot;				//存储根路径字符串
	string dirPath;				//存储需要查询的目录路径名
	string filePath;			//存储需要查询的文件路径名
	string fileCommand;			//存储文件操作命令
	string dirCommand;			//存储目录操作命令
	string statCommand;			//存储目录查询命令
	string sqlFileName;			//存储SQL文件名
	string statFileName;		//存储目录查询文件名
	string fileopFileName;		//存储文件操作命令文件名
	string diropFileName;		//存储目录操作命令文件名
	fstream sqlFout;			//作为输出的SQL文件
	fstream statFin;			//作为输入的目录文件
	fstream fileFin;			//作为输入的文件操作文件
	fstream dirFin;				//作为输入的目录操作文件
	int choice = 0;				//存储用户功能选择
	int mode = 0;				//存储子功能模式
	int x, y, ix, iy;			//存储控制台光标位置，辅助输出

	rootPath[0] = 0;
	printer.printFormat("DISK SCANNER", '-');
	printer.printFormat("使用前请务必阅读安装文件夹中的README文件", '*');
	while (1)
	{
		if (choice == 0)
		{
			printer.printDash();
			cout << "***请选择功能" << endl;
			cout << "***1.扫描目录" << endl;
			cout << "***2.查询指定文件的基本信息     3.查询指定目录的文件信息" << endl;
			cout << "***4.操作文件                  5.操作目录" << endl;
			cout << "***6.退出" << endl;
		}
		else
		{
			printer.printDash();
			cout << "***请选择功能（当前目录根路径：";
			_tprintf(_T("%s"), rootPath);
			cout << "）" << endl;
			cout << "***1.扫描目录" << endl;
			cout << "***2.查询指定文件的基本信息     3.查询指定目录的文件信息" << endl;
			cout << "***4.操作文件                  5.操作目录" << endl;
			cout << "***6.退出" << endl;
		}
		printer.scanFormat(choice);
		clearConsole();
		switch (choice)
		{
		case 1:
			printer.printDash();
			cout << "***请输入需要扫描的目录的路径名（如：希望扫描C:\\Windows\\,请输入C:\\Windows\\*）" << endl;
			printer.printDash();
			cout << "***";
			getCursorPosition(ix, iy);
			cout << endl;
			printer.printDash();
			getCursorPosition(x, y);
			setCursorPosition(ix, iy);
			_tscanf_s(_T("%s"), rootPath, MAX_LENGTH);
			cin.get();
			setCursorPosition(x, y);
			printer.printDash();
			cout << "***请输入输出的SQL文件的文件路径（输入回车按默认路径输出至安装文件夹，自定义路径时无需输入文件后缀）" << endl;
			printer.printDash();
			cout << "***";
			getCursorPosition(ix, iy);
			cout << endl;
			printer.printDash();
			getCursorPosition(x, y);
			setCursorPosition(ix, iy);
			getline(cin, sqlFileName);
			setCursorPosition(x, y);
			clearConsole();
			myDisk.scanAndBuild(rootPath, sqlFileName);
			cout << "---扫描完成！" << endl;
			myDisk.showDiskInfo();
			break;
		case 2:
			if (_tcslen(rootPath) == 0)
			{
				printer.printDash();
				cout << "***当前还未扫描目录，请先扫描目录！" << endl;
				printer.printDash();
				break;
			}
			printer.printDash();
			cout << "***请输入文件路径" << endl;
			printer.scanFormat(filePath);
			clearConsole();
			s2tc(filePath, tFilePath);
			myDisk.showFileInfo(tFilePath);
			break;
		case 3:
			if (_tcslen(rootPath) == 0)
			{
				printer.printDash();
				cout << "***当前还未扫描目录，请先扫描目录！" << endl;
				printer.printDash();
				break;
			}
			printer.printDash();
			cout << "***请选择操作方式" << endl;
			cout << "***1.查询单个目录         2.读取目录查询命令文件" << endl;
			printer.scanFormat(mode);
			printer.printDash();
			switch (mode)
			{
			case 1:
				cout << "***输入目录路径（末尾无需添加'*'）" << endl;
				printer.scanFormat(dirPath);
				printer.printDash();
				s2tc(dirPath, tDirPath);
				clearConsole();
				myDisk.showDirInfo(tDirPath);
				break;
			case 2:
				cout << "***输入命令文件地址" << endl;
				printer.scanFormat(statFileName);
				statFin.open(statFileName, std::ios::in);
				if (!statFin.is_open())
				{
					printer.printDash();
					cout << "***文件打开失败，请检查输入的文件路径是否正确！" << endl;
					printer.printDash();
					break;
				}
				getline(statFin, statCommand);
				if (statCommand != "stat dirs")
				{
					printer.printDash();
					cout << "***文件命令格式错误，请检查输入的文件路径是否正确！" << endl;
					printer.printDash();
					break;
				}
				clearConsole();
				getline(statFin, statCommand);
				while (statCommand != "end of dirs")
				{
					s2tc(statCommand, tDirPath);
					myDisk.showDirInfo(tDirPath);
					getline(statFin, statCommand);
				}
				statFin.close();
				break;
			default:
				printer.printDash();
				cout << "***无效的输入!" << endl;
				printer.printDash();
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				break;
			}
			break;
		case 4:
			if (_tcslen(rootPath) == 0)
			{
				printer.printDash();
				cout << "***当前还未扫描目录，请先扫描目录！" << endl;
				printer.printDash();
				break;
			}
			printer.printDash();
			cout << "***请选择操作方式" << endl;
			cout << "***1.操作单个文件         2.读取文件操作命令文件" << endl;
			printer.scanFormat(mode);
			printer.printDash();
			switch (mode)
			{
			case 1:
				cout << "***输入文件操作命令" << endl;
				printer.scanFormat(fileCommand);
				clearConsole();
				myDisk.manageFileInfo(fileCommand);
				break;
			case 2:
				cout << "***输入命令文件地址" << endl;
				printer.scanFormat(fileopFileName);
				fileFin.open(fileopFileName, std::ios::in);
				if (!fileFin.is_open())
				{
					printer.printDash();
					cout << "***文件打开失败，请检查输入的文件路径是否正确！" << endl;
					printer.printDash();
					break;
				}
				getline(fileFin, fileCommand);
				if (fileCommand != "selected files")
				{
					printer.printDash();
					cout << "***文件命令格式错误，请检查输入的文件路径是否正确！" << endl;
					printer.printDash();
					break;
				}
				clearConsole();
				getline(fileFin, fileCommand);
				while (fileCommand != "end of files")
				{
					myDisk.manageFileInfo(fileCommand);
					getline(fileFin, fileCommand);
				}
				fileFin.close();
				break;
			default:
				printer.printDash();
				cout << "***无效的输入!" << endl;
				printer.printDash();
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				break;
			}
			break;
		case 5:
			if (_tcslen(rootPath) == 0)
			{
				printer.printDash();
				cout << "***当前还未扫描目录，请先扫描目录！" << endl;
				printer.printDash();
				break;
			}
			printer.printDash();
			cout << "***请选择操作方式" << endl;
			cout << "***1.操作单个目录         2.读取目录操作命令文件" << endl;
			printer.scanFormat(mode);
			printer.printDash();
			switch (mode)
			{
			case 1:
				cout << "***输入目录操作命令" << endl;
				printer.scanFormat(dirCommand);
				clearConsole();
				myDisk.manageDirInfo(dirCommand);
				break;
			case 2:
				cout << "***输入命令文件地址" << endl;
				printer.scanFormat(diropFileName);
				dirFin.open(diropFileName, std::ios::in);
				if (!dirFin.is_open())
				{
					printer.printDash();
					cout << "***文件打开失败，请检查输入的文件路径是否正确！" << endl;
					printer.printDash();
					break;
				}
				getline(dirFin, dirCommand);
				if (dirCommand != "selected dirs")
				{
					printer.printDash();
					cout << "***文件命令格式错误，请检查输入的文件路径是否正确！" << endl;
					printer.printDash();
					break;
				}
				clearConsole();
				getline(dirFin, dirCommand);
				while (dirCommand != "end of dirs")
				{
					myDisk.manageDirInfo(dirCommand);
					getline(dirFin, dirCommand);
				}
				dirFin.close();
				break;
			default:
				printer.printDash();
				cout << "***无效的输入!" << endl;
				printer.printDash();
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				break;
			}
			break;
		case 6:
			printer.printDash();
			cout << "***再见！" << endl;
			printer.printDash();
			return 0;
		default:
			printer.printDash();
			cout << "***无效的输入!" << endl;
			printer.printDash();
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			break;
		}
		printer.printDash();
		cout << "***按下回车键继续..." << endl;
		printer.printDash();
		cin.get();
		clearConsole();
	}

	return 0;
}