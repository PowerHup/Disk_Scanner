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
	_tsetlocale(LC_ALL, _T("chs"));	//�����ַ������������޷���������

	disk myDisk;				//���������
	formatprinter printer;		//��������Ķ���
	TCHAR rootPath[MAX_LENGTH];	//ɨ��ĸ�·��
	TCHAR tDirPath[MAX_LENGTH]; //�洢TCHAR��ʽĿ¼·����
	TCHAR tFilePath[MAX_LENGTH];//�洢TCHAR��ʽ�ļ�·����
	string strRoot;				//�洢��·���ַ���
	string dirPath;				//�洢��Ҫ��ѯ��Ŀ¼·����
	string filePath;			//�洢��Ҫ��ѯ���ļ�·����
	string fileCommand;			//�洢�ļ���������
	string dirCommand;			//�洢Ŀ¼��������
	string statCommand;			//�洢Ŀ¼��ѯ����
	string sqlFileName;			//�洢SQL�ļ���
	string statFileName;		//�洢Ŀ¼��ѯ�ļ���
	string fileopFileName;		//�洢�ļ����������ļ���
	string diropFileName;		//�洢Ŀ¼���������ļ���
	fstream sqlFout;			//��Ϊ�����SQL�ļ�
	fstream statFin;			//��Ϊ�����Ŀ¼�ļ�
	fstream fileFin;			//��Ϊ������ļ������ļ�
	fstream dirFin;				//��Ϊ�����Ŀ¼�����ļ�
	int choice = 0;				//�洢�û�����ѡ��
	int mode = 0;				//�洢�ӹ���ģʽ
	int x, y, ix, iy;			//�洢����̨���λ�ã��������

	rootPath[0] = 0;
	printer.printFormat("DISK SCANNER", '-');
	printer.printFormat("ʹ��ǰ������Ķ���װ�ļ����е�README�ļ�", '*');
	while (1)
	{
		if (rootPath[0] == 0)
		{
			printer.printDash();
			cout << "***��ѡ����" << endl;
			cout << "***1.ɨ��Ŀ¼" << endl;
			cout << "***2.��ѯָ���ļ��Ļ�����Ϣ     3.��ѯָ��Ŀ¼���ļ���Ϣ" << endl;
			cout << "***4.�����ļ�                  5.����Ŀ¼" << endl;
			cout << "***6.�˳�" << endl;
		}
		else
		{
			printer.printDash();
			cout << "***��ѡ���ܣ���ǰĿ¼��·����";
			_tprintf(_T("%s"), rootPath);
			cout << "��" << endl;
			cout << "***1.ɨ��Ŀ¼" << endl;
			cout << "***2.��ѯָ���ļ��Ļ�����Ϣ     3.��ѯָ��Ŀ¼���ļ���Ϣ" << endl;
			cout << "***4.�����ļ�                  5.����Ŀ¼" << endl;
			cout << "***6.�˳�" << endl;
		}
		printer.scanFormat(choice);
		clearConsole();
		switch (choice)
		{
			/*ɨ��Ŀ¼*/
		case 1:
			printer.printDash();
			cout << "***��������Ҫɨ���Ŀ¼��·�������磺ϣ��ɨ��C:\\Windows\\,������C:\\Windows\\*��" << endl;
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
			cout << "***�����������SQL�ļ����ļ�·��������س���Ĭ��·���������װ�ļ��У��Զ���·��ʱ���������ļ���׺��" << endl;
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
			cout << "---ɨ����ɣ�" << endl;
			myDisk.showDiskInfo();
			break;

			/*�����ļ���Ϣ*/
		case 2:
			if (_tcslen(rootPath) == 0)
			{
				printer.printDash();
				cout << "***��ǰ��δɨ��Ŀ¼������ɨ��Ŀ¼��" << endl;
				printer.printDash();
				break;
			}
			printer.printDash();
			cout << "***�������ļ�·��" << endl;
			printer.scanFormat(filePath);
			clearConsole();
			s2tc(filePath, tFilePath);
			myDisk.showFileInfo(tFilePath);
			break;

			/*����Ŀ¼��Ϣ*/
		case 3:
			if (_tcslen(rootPath) == 0)
			{
				printer.printDash();
				cout << "***��ǰ��δɨ��Ŀ¼������ɨ��Ŀ¼��" << endl;
				printer.printDash();
				break;
			}
			printer.printDash();
			cout << "***��ѡ�������ʽ" << endl;
			cout << "***1.��ѯ����Ŀ¼         2.��ȡĿ¼��ѯ�����ļ�" << endl;
			printer.scanFormat(mode);
			printer.printDash();
			switch (mode)
			{
				/*���ҵ���Ŀ¼*/
			case 1:
				cout << "***����Ŀ¼·����ĩβ�������'*'��" << endl;
				printer.scanFormat(dirPath);
				printer.printDash();
				s2tc(dirPath, tDirPath);
				clearConsole();
				myDisk.showDirInfo(tDirPath);
				break;

				/*��ȡĿ¼��ѯ����*/
			case 2:
				int diffCount, commandCount;
				diffCount = commandCount = 0;
				cout << "***���������ļ���ַ" << endl;
				printer.scanFormat(statFileName);
				statFin.open(statFileName, std::ios::in);
				if (!statFin.is_open())
				{
					printer.printDash();
					cout << "***�ļ���ʧ�ܣ�����������ļ�·���Ƿ���ȷ��" << endl;
					printer.printDash();
					break;
				}
				getline(statFin, statCommand);
				if (statCommand != "stat dirs")
				{
					printer.printDash();
					cout << "***�ļ������ʽ��������������ļ�·���Ƿ���ȷ��" << endl;
					printer.printDash();
					break;
				}
				clearConsole();
				getline(statFin, statCommand);
				while (statCommand != "end of dirs")
				{
					s2tc(statCommand, tDirPath);
					if (myDisk.showDirInfo(tDirPath) == false)
						diffCount++;
					getline(statFin, statCommand);
					commandCount++;
				}
				printer.printStar();
				cout << "---����ѯ" << commandCount << "��Ŀ¼����" << diffCount << "������" << endl;
				printer.printStar();
				statFin.close();
				break;
			default:
				printer.printDash();
				cout << "***��Ч������!" << endl;
				printer.printDash();
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				break;
			}
			break;

			/*�����ļ�*/
		case 4:
			if (_tcslen(rootPath) == 0)
			{
				printer.printDash();
				cout << "***��ǰ��δɨ��Ŀ¼������ɨ��Ŀ¼��" << endl;
				printer.printDash();
				break;
			}
			printer.printDash();
			cout << "***��ѡ�������ʽ" << endl;
			cout << "***1.���������ļ�         2.��ȡ�ļ����������ļ�" << endl;
			printer.scanFormat(mode);
			printer.printDash();
			switch (mode)
			{
				/*���������ļ�*/
			case 1:
				cout << "***�����ļ���������" << endl;
				printer.scanFormat(fileCommand);
				clearConsole();
				myDisk.manageFileInfo(fileCommand);
				break;

				/*��ȡ�ļ���������*/
			case 2:
				cout << "***���������ļ���ַ" << endl;
				printer.scanFormat(fileopFileName);
				fileFin.open(fileopFileName, std::ios::in);
				if (!fileFin.is_open())
				{
					printer.printDash();
					cout << "***�ļ���ʧ�ܣ�����������ļ�·���Ƿ���ȷ��" << endl;
					printer.printDash();
					break;
				}
				getline(fileFin, fileCommand);
				if (fileCommand != "selected files")
				{
					printer.printDash();
					cout << "***�ļ������ʽ��������������ļ�·���Ƿ���ȷ��" << endl;
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

				/*������Ч����*/
			default:
				printer.printDash();
				cout << "***��Ч������!" << endl;
				printer.printDash();
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				break;
			}
			break;

			/*����Ŀ¼*/
		case 5:
			if (_tcslen(rootPath) == 0)
			{
				printer.printDash();
				cout << "***��ǰ��δɨ��Ŀ¼������ɨ��Ŀ¼��" << endl;
				printer.printDash();
				break;
			}
			printer.printDash();
			cout << "***��ѡ�������ʽ" << endl;
			cout << "***1.��������Ŀ¼         2.��ȡĿ¼���������ļ�" << endl;
			printer.scanFormat(mode);
			printer.printDash();
			switch (mode)
			{
				/*��������Ŀ¼*/
			case 1:
				cout << "***����Ŀ¼��������" << endl;
				printer.scanFormat(dirCommand);
				clearConsole();
				myDisk.manageDirInfo(dirCommand);
				break;

				/*��ȡĿ¼�����ļ�*/
			case 2:
				cout << "***���������ļ���ַ" << endl;
				printer.scanFormat(diropFileName);
				dirFin.open(diropFileName, std::ios::in);
				if (!dirFin.is_open())
				{
					printer.printDash();
					cout << "***�ļ���ʧ�ܣ�����������ļ�·���Ƿ���ȷ��" << endl;
					printer.printDash();
					break;
				}
				getline(dirFin, dirCommand);
				if (dirCommand != "selected dirs")
				{
					printer.printDash();
					cout << "***�ļ������ʽ��������������ļ�·���Ƿ���ȷ��" << endl;
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

				/*������Ч����*/
			default:
				printer.printDash();
				cout << "***��Ч������!" << endl;
				printer.printDash();
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				break;
			}
			break;

			/*�˳�����*/
		case 6:
			printer.printDash();
			cout << "***�ټ���" << endl;
			printer.printDash();
			printer.printDash();
			cout << "***���»س����˳�..." << endl;
			printer.printDash();
			cin.get();
			return 0;

			/*��Ч���봦��*/
		default:
			printer.printDash();
			cout << "***��Ч������!" << endl;
			printer.printDash();
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			break;
		}
		printer.printDash();
		cout << "***���»س�������..." << endl;
		printer.printDash();
		cin.get();
		clearConsole();
	}

	return 0;
}