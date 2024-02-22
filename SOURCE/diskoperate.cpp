#include"diskoperate.h"
#include"formatprinter.h"
#include"tools.h"
#include<algorithm>
#include<cstring>
#include<ctime>
#include<functional>
#include<queue>
#include<sstream>
#include<stack>
#include<string>
#include<vector>

using namespace diskop;
using namespace fprinter;
using std::vector;
using std::stack;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::shared_ptr;
using std::make_shared;
using std::fstream;
using std::istringstream;
using std::getline;
using std::cerr;
using std::size_t;
using std::ios;

/*�ṹ��Ĭ�Ϲ��캯��*/
disk::fileNode::fileNode() :subDir(nullptr), parentDir(nullptr)
{
    fileSize = 0;
    fileName[0] = 0;
    creationUTCTime = { 0 };
    fileType = _DIR;
    dirt = F;
    creationUTCTime = 0;
}

/*�ṹ�帴�ƹ��캯��*/
disk::fileNode::fileNode(const fileNode& afileNode) :subDir(afileNode.subDir), parentDir(afileNode.parentDir)
{
    fileType = afileNode.fileType;
    fileSize = afileNode.fileSize;
    _tcscpy_s(fileName, MAX_LENGTH, afileNode.fileName);
    dirt = afileNode.dirt;
    creationUTCTime = afileNode.creationUTCTime;
}

/*�ṹ�帳ֵ���������*/
disk::fileNode& disk::fileNode::operator=(const fileNode& afileNode)
{
    fileType = afileNode.fileType;
    fileSize = afileNode.fileSize;
    _tcscpy_s(fileName, MAX_LENGTH, afileNode.fileName);
    subDir = afileNode.subDir;
    parentDir = afileNode.parentDir;
    creationUTCTime = afileNode.creationUTCTime;
    dirt = afileNode.dirt;

    return *this;
}

/*������Ĭ�Ϲ��캯��*/
disk::disk() :dirCount(0), fileCount(0), dirDepth(0), treeDepth(0), root(make_shared<fileTree>())
{
    longestPath[0] = 0;
    log.open("DISKLOG.log", ios::out | ios::app);
}

/*��ѯָ���ļ���λ��*/
disk::nodePtr disk::findFileNode(const TCHAR* path, const TYPE mode) const
{
    vector<string> strs;                    //�洢ÿһ��·��
    string strPath, pathStr;                //�洢ת�����Ŀ¼·��
    TCHAR pathArr[MAX_LENGTH];
    shared_ptr<fileNode> tempNode;          //�ݴ�Ŀ¼���ڵ�
    int i, j;

    /*����Ѱ�Ҷ�ӦĿ¼*/
    i = j = 0;
    strPath = tc2s(path);
    strSplit(strPath, strs, '\\');
    strPath = "";
    tempNode = root;
    do      //����Ѱ�Ҹ�·��
    {
        strPath += strs[i++] + "\\";
    } while (_stricmp(strPath.c_str(), tc2s(root->fileName).c_str()) && i < strs.size());
    j = i;
    if (_stricmp(strPath.c_str(), tc2s(root->fileName).c_str())) //��·�����󣬷��ؿ�ָ��
        return nullptr;
    else if (strs.size() == 2)  //���ҵľ��Ǹ�·�������ظ��ڵ�
        return root;
    else                        //�����׸���Ŀ¼·��
    {
        strPath = tc2s(root->fileName);
        pathStr = strPath;
        if (i == strs.size() - 1)
        {
            if (mode == _DIR)
                strPath += strs[i] + "\\";
            else
                strPath += strs[i];
        }
        else
            strPath += strs[i++] + "\\";
    }

    for (; j < strs.size(); j++)
    {
        if (j == strs.size() - 1)
        {
            if (mode == _DIR)
                pathStr += strs[j] + "\\";
            else
                pathStr += strs[j];
        }
        else
            pathStr += strs[j] + "\\";
    }
    s2tc(pathStr, pathArr);

    for (auto iter = tempNode->subDir->find(strPath); iter != tempNode->subDir->end(); iter = tempNode->subDir->find(strPath))  //������������ļ�
    {
        tempNode = iter->second;

        if (_tcscmp(tempNode->fileName, pathArr) == 0)
            break;
        if (i == strs.size() - 1)
        {
            if (mode == _DIR)
                strPath += strs[i] + "\\";
            else
                strPath += strs[i];
        }
        else
            strPath += strs[i++] + "\\";
    }

    if (_tcscmp(tempNode->fileName, pathArr) != 0)
        return nullptr;
    else
        return tempNode;
}

/*����Ŀ¼�����*/
int disk::depthOfTree() const
{
    /*��������*/
    nodePtr tempNode;           //�ݴ��ļ��ڵ�
    fileChild::iterator iter;   //fileChilds������
    stack<nodePtr> nodeStack;   //�ڵ�ջ������ģ��ݹ������
    int treeDepth = -1;         //�洢�����

    tempNode = root;

    do
    {
        switch (tempNode->fileType)
        {
            /*��ǰ�ļ���Ŀ¼*/
        case _DIR:
            if (tempNode->dirt == tempNode->T)  //Ŀ¼�Ѿ�������������ջ
            {
                nodeStack.pop();
                if (nodeStack.empty())
                    return treeDepth;
                else
                    tempNode = nodeStack.top();
            }
            else                                //Ŀ¼δ����������Ŀ¼��������Ŀ¼��ջ
            {
                nodeStack.push(tempNode);
                tempNode->dirt = tempNode->T;
                for (iter = tempNode->subDir->begin(); iter != tempNode->subDir->end(); iter++)
                    nodeStack.push(iter->second);
                treeDepth = max(treeDepth, static_cast<int>(nodeStack.size()));
                tempNode = nodeStack.top();
            }
            break;
            /*��ǰ�ļ����ļ�*/
        case _FILE:
            nodeStack.pop();        //��������Ŀ¼��ֱ�ӳ�ջ
            if (nodeStack.empty())
                return treeDepth;
            else
                tempNode = nodeStack.top();
            break;
        }
    } while (!nodeStack.empty());

    return treeDepth;
}

/*ɨ�̺���������������ȱ���������ջ���У���ɨ�赽Ŀ¼��������Ŀ¼��������ɨ������ļ���������ļ���Ϣ��ɨ����һ�ļ���ɨ�̵�ͬʱ����Ŀ¼��������SQL�ļ�*/
void disk::scanAndBuild(const TCHAR* rootPath, string& sqlFileName)
{
    _tsetlocale(LC_ALL, _T("chs"));     //������ã������޷��������

    /*��������*/
    enum SIGNAL { T, F };               //ָʾ��ʱ�Ƿ���Լ�������
    TYPE type = _DIR;                   //�ļ����ͱ�ʶ��
    SIGNAL sign = T;                    //ջ������ʶ��
    WIN32_FIND_DATA findFileData;       //ɨ��õ����ļ���Ϣ
    TCHAR path[MAX_LENGTH];             //�洢·������Ϊ�����������·��������·����ջ
    stack<TCHAR*> pathStack;            //·����ջ���洢��ͬ��ε�·����������ģ��ݹ�
    stack<HANDLE> progStack;            //����ջ���洢�ļ������ģ��ݹ�
    nodePtr newNode;                    //������ʱ�洢�½ڵ�
    nodePtr tempNode;                   //����ָ��ǰ�ڵ�
    int sqlCount = 1;                   //��¼SQL������Ŀ��������������ʱ���µ��ļ�
    int outputCount = 1;                //��¼����ļ�����
    char timeStr[MAX_LENGTH];           //�洢ʱ���ַ���
    fstream fout;                       //���SQL�ļ����ļ���
    string outputFileRoot;              //����ļ�����������ļ�����һ��ʱ��Ҫ������Ϊ�������������ļ���
    string outputFileName;              //��һ������ļ����ļ���
    int ox, oy, x, y;
    int count = 10000;
    auto judgeFile = [&sqlCount, &outputCount, &fout, &outputFileName, &outputFileRoot]()             //�ж��Ƿ�Ӧ�ô����ļ���lambda���ʽ
        {
            if (sqlCount % MAX_COUNT)   //�ļ���SQL���С��MAX_COUNT����������ǰ�ļ�д��
                sqlCount++;
            else                        //��һ���ļ��е�SQL�����������10000����
            {
                string num;
                fout.close();   //�ر��ļ�
                num = to_string(outputCount++ + 1) + ".sql";   //�����µ��ļ���
                outputFileName = outputFileRoot + num;
                fout.open(outputFileName, std::ios::out);      //���µ��ļ�
                fout << "use SYSTEM_DISK;" << endl;
                sqlCount++;
            }
        };

    /*������Ҫ������ʼ��*/
    if (sqlFileName.empty())
    {
        outputFileRoot = "SQLstatements-";
        outputFileName = outputFileRoot + "1.sql";
    }
    else
    {
        outputFileRoot = sqlFileName + "-";
        outputFileName = outputFileRoot + "1.sql";
    }
    _tcscpy_s(path, MAX_LENGTH, rootPath);
    pathStack.push(new TCHAR[_tcslen(path) + 1]);                   //��ʼ·����·��ջ
    _tcscpy_s(pathStack.top(), _tcslen(path) + 1, path);
    progStack.push(FindFirstFile(pathStack.top(), &findFileData));  //��ʼ��������ջ
    root->fileType = _DIR;                                          //��ʼ����·���ڵ�
    path[_tcslen(path) - 1] = 0;
    _tcscpy_s(root->fileName, MAX_LENGTH, path);
    path[_tcslen(path)] = '*';
    root->parentDir = nullptr;
    root->subDir = make_shared<fileChild>();
    root->dirt = root->F;
    tempNode = root;
    fout.open(outputFileName, ios::out);                            //������ļ�
    fout << "use SYSTEM_DISK;" << endl;                             //����ʹ�����ݿ����
    time_t now = time(nullptr);
    ctime_s(timeStr, MAX_LENGTH, &now);
    dirCount = fileCount = dirDepth = treeDepth = 0;
    printer.printStar();
    cout << "---ɨ��Ŀ¼��";
    _tprintf(_T("%s\n"), rootPath);
    log << "TIME:" << timeStr;
    log << "ɨ��Ŀ¼��" << tc2s(rootPath) << endl;

    /*���������������ɨ����̲��ɷ���*/
    if (progStack.top() == INVALID_HANDLE_VALUE)
    {
        cerr << "---Ŀ¼���ɷ��ʣ�" << endl;
        printer.printStar();
        log << "Ŀ¼���ɷ��ʣ�" << endl;
        log << "-----------------" << endl;
        return;
    }

    /*ɨ�̺�������*/
    cout << "---��ǰ��ɨ���ļ�����";
    getCursorPosition(ox, oy);
    cout << endl;
    printer.printStar();
    getCursorPosition(x, y);
    while (progStack.size() > 0)
    {
        /*�����쳣�����ִ�г�ջ����*/
        while (sign == F)
        {
            sign = T;
            if (dirDepth < progStack.size())       //�ж��Ƿ�Ϊ������
                dirDepth = static_cast<int>(progStack.size());
            /*д��SQL�ļ�*/
            judgeFile();
            TCHAR* path = pathStack.top();
            path[_tcslen(path) - 1] = 0;
            fout << "update DIRECTORIES set Ddepth=" << to_string(static_cast<int>(progStack.size())) << ",Dfcount=";  //��SQL�ļ�д��SQL���
            fout << to_string(static_cast<int>(tempNode->subDir->size()));
            fout << " where Dpath=\'" << tc2s(path) << "\';" << endl;
            /*ִ�г�ջ����*/
            delete[] pathStack.top();           //·��������ϣ���ջǰ���ͷ��ڴ�
            pathStack.pop();
            FindClose(progStack.top());         //����Ѿ�������ɣ���ջǰ��Ҫ�رվ��
            progStack.pop();
            tempNode = tempNode->parentDir;     //���ڵ�ǰָ��Ľڵ��Ѿ���ջ����ʱӦ��ָ�����˽ڵ�
            if (progStack.size() <= 0)          //��ջ�ѿգ�ֱ�ӷ���
            {
                fout.close();
                treeDepth = depthOfTree();
                ctime_s(timeStr, MAX_LENGTH, &now);
                log << "Ŀ¼����" << dirCount << endl;
                log << "�ļ�����" << dirCount + fileCount << endl;
                log << "Ŀ¼������" << dirDepth << endl;
                log << "Ŀ¼����ȣ�" << treeDepth << endl;
                log << "��ļ�·����" << tc2s(longestPath) << endl;
                log << "��ļ�·�����ȣ�" << _tcslen(longestPath) << endl;
                log << "-----------------" << endl;
                setCursorPosition(ox, oy);
                cout << dirCount + fileCount;
                setCursorPosition(x, y);

                return;
            }
            if (FindNextFile(progStack.top(), &findFileData) == 0)      //��ջ��Ŀ¼Ҳ����һ�ļ���ɨ�裬����SIGNALΪF
                sign = F;
        }
        /*�ų������������*/
        while (_tcscmp(findFileData.cFileName, _T(".")) == 0 || _tcscmp(findFileData.cFileName, _T("..")) == 0)
        {
            if (FindNextFile(progStack.top(), &findFileData) == 0)      //ɨ�赱ǰĿ¼����һ���ļ�����û����һ���ļ�������SIGNALö����ΪF����ʾ��ʱ��Ӧ�����κβ���
            {
                sign = F;
                break;
            }
        }
        /*��鵱ǰ�ļ���Ŀ¼�����ļ�*/
        if (sign != F && findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            type = _DIR;        //��Ŀ¼
        else
            type = _FILE;       //���ļ�
        /*��Բ�ͬ�ļ�����ִ�в�ͬ�ı�������*/
        if (sign != F)
        {
            switch (type)
            {
                /*��ǰɨ�����Ŀ¼*/
            case _DIR:
                dirCount++;     //Ŀ¼����������
                if (dirCount + fileCount >= count)
                {
                    setCursorPosition(ox, oy);
                    cout << dirCount + fileCount;
                    if (count < 200000)
                        count += 10000;
                    else
                        count += 5000;
                }
                TCHAR subDir[MAX_LENGTH];
                _tcscpy_s(path, MAX_LENGTH, pathStack.top());
                path[_tcslen(path) - 1] = 0;
                _stprintf_s(subDir, MAX_LENGTH, _T("%s%s\\*"), path, findFileData.cFileName);   //�洢��Ŀ¼·��
                /*д��SQL�ļ�*/
                judgeFile();
                subDir[_tcslen(subDir) - 1] = 0;
                fout << "insert into DIRECTORIES(Dname,Dpath) values(" << "\'" << tc2s(findFileData.cFileName) << "\',";   //��SQL�ļ�д��SQL���
                fout << "\'" << tc2s(subDir) << "\');" << endl;
                /*������Ҫ����������൱�ڴ�ʱ��Ҫִ�еݹ飬�ʽ���ջ����*/
                subDir[_tcslen(subDir)] = '*';
                pathStack.push(new TCHAR[_tcslen(subDir) + 1]);         //Ŀ¼·����ջ
                _tcscpy_s(pathStack.top(), _tcslen(subDir) + 1, subDir);
                progStack.push(FindFirstFile(subDir, &findFileData));   //Ŀ¼�����ջ
                /*������Ŀ¼�ڵ�*/
                subDir[_tcslen(subDir) - 1] = 0;
                newNode = make_shared<fileNode>();
                newNode->fileType = _DIR;                        //�ļ�������ΪĿ¼
                _tcscpy_s(newNode->fileName, MAX_LENGTH, subDir);  //�洢�ļ���
                newNode->fileSize = ((static_cast<ULONGLONG>(findFileData.nFileSizeHigh) << 32)) | findFileData.nFileSizeLow;
                newNode->creationUTCTime = ft2t(findFileData.ftCreationTime);
                newNode->parentDir = tempNode;                   //�����ļ��״��ڵ�
                newNode->subDir = make_shared<fileChild>();     //Ϊ��Ŀ¼�洢���ļ��ڵ����ռ�
                tempNode->subDir->insert(make_pair(tc2s(subDir), newNode));            //���ýڵ�����״������ļ�������
                tempNode = newNode;                              //������ȣ���һ�������Ľڵ��Ǹ�Ŀ¼�µĽڵ㣬tempNodeָ���Ŀ¼�ڵ�
                if (progStack.top() == INVALID_HANDLE_VALUE)     //��ǰ·���µ��ļ����ɶ�����ֱ�ӱ�ʶSIGNAL���ȴ�������һѭ�����д���
                {
                    sign = F;
                    break;
                }
                break;

                /*��ǰɨ������ļ�*/
            case _FILE:
                fileCount++;      //�ļ�����������
                if (dirCount + fileCount >= count)
                {
                    setCursorPosition(ox, oy);
                    cout << dirCount + fileCount;
                    if (count < 200000)
                        count += 10000;
                    else
                        count += 5000;
                }
                _tcscpy_s(path, MAX_LENGTH, pathStack.top());
                path[_tcslen(path) - 1] = 0;
                TCHAR filePath[MAX_LENGTH] = { 0 };
                /*�����ļ��ڵ�*/
                _stprintf_s(filePath, MAX_LENGTH, _T("%s%s"), path, findFileData.cFileName);
                newNode = make_shared<fileNode>();
                newNode->fileType = _FILE;                                       //�ļ�������Ϊ�ļ�
                _tcscpy_s(newNode->fileName, MAX_LENGTH, filePath);              //�洢·����
                newNode->parentDir = tempNode;                                   //�洢�״��ڵ�
                newNode->subDir = nullptr;                                       //�������ļ���û����Ŀ¼����Ϊ��ָ��
                newNode->creationUTCTime = ft2t(findFileData.ftCreationTime);    //�洢�ļ�����ʱ��
                newNode->fileSize = ((static_cast<ULONGLONG>(findFileData.nFileSizeHigh) << 32)) | findFileData.nFileSizeLow;    //�洢�ļ���С
                tempNode->subDir->insert(make_pair(tc2s(filePath), newNode));                            //���ýڵ�����״������ļ�����
                /*д��SQL�ļ�*/
                judgeFile();
                fout << "insert into FILES values(" << "\'" << tc2s(findFileData.cFileName) << "\',";
                fout << "\'" << tc2s(filePath) << "\'," << "\'" << tc2s(path) << "\'," << to_string(newNode->fileSize) << ",";
                fout << to_string(ft2t(findFileData.ftCreationTime));
                fout << ");" << endl;
                if (_tcslen(longestPath) < _tcslen(filePath))           //�Ƚ�·�������ȣ����ȫ·����
                    _tcscpy_s(longestPath, MAX_LENGTH, filePath);
                if (FindNextFile(progStack.top(), &findFileData) == 0)  //��ǰ�Ѿ������������ɨ����һ�ļ���������һ�ļ������ʶSIGNAL
                    sign = F;
                break;
            }
        }
    }

    fout.close();
    treeDepth = depthOfTree();
    ctime_s(timeStr, MAX_LENGTH, &now);
    log << "Ŀ¼����" << dirCount << endl;
    log << "�ļ�����" << dirCount + fileCount << endl;
    log << "Ŀ¼������" << dirDepth << endl;
    log << "Ŀ¼����ȣ�" << treeDepth << endl;
    log << "��ļ�·����" << tc2s(longestPath) << endl;
    log << "��ļ�·�����ȣ�" << _tcslen(longestPath) << endl;
    log << "-----------------" << endl;
    setCursorPosition(ox, oy);
    cout << dirCount + fileCount;
    setCursorPosition(x, y);

    return;
}

/*�����ļ���Ϣ*/
void disk::manageFileInfo(string& command)
{
    vector<string> commandPart;     //�洢��ָ����Ϊ�ಿ�ֵ�����
    shared_ptr<fileNode> tempNode;  //�ݴ��ļ��ڵ�
    time_t now;                     //�洢��ǰʱ��
    TCHAR* tempPath;
    char timeStr[MAX_LENGTH];

    strSplit(command, commandPart, ',');
    tempNode = findFileNode(s2tc(commandPart[0]), _FILE);
    now = time(nullptr);
    ctime_s(timeStr, MAX_LENGTH, &now);

    printer.printStar();
    cout << "---Ŀ���ļ���" << commandPart[0] << endl;
    log << "TIME:" << timeStr;
    log << "Ŀ���ļ���" << commandPart[0] << endl;

    /*����ִ������*/
    switch (commandPart[1][0])
    {
        /*ɾ������*/
    case 'D':
        if (tempNode == nullptr)
        {
            cerr << "---Ŀ���ļ������ڣ�" << endl;
            printer.printStar();
            log << "Ŀ���ļ������ڣ�" << endl;
            log << "-----------------" << endl;
            return;
        }
        else
        {
            ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
            cout << "---ԭ�ļ���С��" << tempNode->fileSize << endl;
            cout << "---ԭ�ļ�����ʱ�䣺" << timeStr;
            cout << "---ִ��ɾ������" << endl;
            log << "ԭ�ļ���С��" << tempNode->fileSize << endl;
            log << "ԭ�ļ�����ʱ�䣺" << timeStr;
            log << "ִ��ɾ������" << endl;
            /*ɾ��ָ���ڵ�*/
            tempNode = tempNode->parentDir;
            tempNode->parentDir->subDir->erase(commandPart[0]);
            cout << "---�ļ���ɾ����" << endl;
            printer.printStar();
            log << "�ļ���ɾ����" << endl;
            log << "-----------------" << endl;
        }
        break;
        /*�޸Ĳ���*/
    case 'M':
        if (tempNode == nullptr)
        {
            cerr << "Ŀ���ļ������ڣ�" << endl;
            cout << "---ִ��ɾ������" << endl;
            log << "Ŀ���ļ������ڣ�" << endl;
            log << "-----------------" << endl;
            return;
        }
        else
        {
            ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
            cout << "---ԭ�ļ���С��" << tempNode->fileSize << endl;
            cout << "---ԭ�ļ�����ʱ�䣺" << timeStr;
            cout << "---ִ���޸Ĳ���" << endl;
            log << "ԭ�ļ���С��" << tempNode->fileSize << endl;
            log << "ԭ�ļ�����ʱ�䣺" << timeStr;
            log << "ִ���޸Ĳ���" << endl;
            /*�޸�ָ���ڵ�*/
            tempNode->creationUTCTime = stoll(commandPart[2]);
            tempNode->fileSize = stoull(commandPart[3]);
            ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
            cout << "---���ļ���С��" << tempNode->fileSize << endl;
            cout << "---���ļ�����ʱ�䣺" << timeStr;
            printer.printStar();
            log << "���ļ���С��" << tempNode->fileSize << endl;
            log << "���ļ�����ʱ�䣺" << timeStr;
            log << "-----------------" << endl;
        }
        break;
        /*���Ӳ���*/
    case 'A':
        cout << "---ԭ�ļ�������" << endl;
        cout << "---ִ�����Ӳ���" << endl;
        vector<string> strs;
        strSplit(commandPart[0], strs, '\\');
        string path = strs[0] + "\\";
        for (int i = 1; i < strs.size() - 1; i++)
            path += strs[i] + "\\";
        tempPath = s2tc(path);
        tempNode = findFileNode(tempPath, _DIR); //���Ҹ��ļ����ϼ�Ŀ¼
        delete[] tempPath;
        if (tempNode == nullptr)
        {
            cerr << "---Ŀ���ϼ�Ŀ¼�����ڣ�" << endl;
            printer.printStar();
            log << "Ŀ���ϼ�Ŀ¼�����ڣ�" << endl;
            log << "-----------------" << endl;
            return;
        }
        /*�������ļ��ڵ�*/
        shared_ptr<fileNode> newNode = make_shared<fileNode>();
        tempPath = s2tc(commandPart[0]);
        _tcscpy_s(newNode->fileName, MAX_LENGTH, tempPath);
        delete[] tempPath;
        newNode->creationUTCTime = stoll(commandPart[2]);
        newNode->fileSize = stoull(commandPart[3]);
        newNode->fileType = _FILE;
        newNode->parentDir = tempNode;
        newNode->subDir = nullptr;
        tempNode->subDir->insert(make_pair(commandPart[0], newNode));
        tempNode = newNode;
        ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
        cout << "---���ļ���С��" << tempNode->fileSize << endl;
        cout << "---���ļ�����ʱ�䣺" << timeStr;
        printer.printStar();
        log << "���ļ���С��" << tempNode->fileSize << endl;
        log << "���ļ�����ʱ�䣺" << timeStr;
        log << "-----------------" << endl;
        break;
    }

    return;
}

/*����Ŀ¼��Ϣ*/
void disk::manageDirInfo(string& command)
{
    vector<string> commandPart;     //�洢��ָ����Ϊ�ಿ�ֵ�����
    shared_ptr<fileNode> tempNode;  //�ݴ��ļ��ڵ�
    time_t now;                     //�洢��ǰʱ��
    string dirName;                 //Ŀ¼��
    TCHAR tDirName[MAX_LENGTH];     //TCHAR��ʽĿ¼��
    TCHAR* tempPath;
    char timeStr[MAX_LENGTH];

    strSplit(command, commandPart, ',');
    tempPath = s2tc(commandPart[0]);
    tempNode = findFileNode(tempPath, _DIR);    //��ѯĿ¼�ڵ�λ��
    delete[] tempPath;
    now = time(nullptr);
    ctime_s(timeStr, MAX_LENGTH, &now);

    printer.printStar();
    cout << "---Ŀ��Ŀ¼��" << commandPart[0] << endl;
    log << "TIME:" << timeStr;
    log << "Ŀ��Ŀ¼��" << commandPart[0] << endl;

    if (tempNode == nullptr)
    {
        cerr << "---Ŀ��Ŀ¼�����ڣ�" << endl;
        printer.printStar();
        log << "Ŀ��Ŀ¼�����ڣ�" << endl;
        log << "-----------------" << endl;
        return;
    }

    _tcscpy_s(tDirName, MAX_LENGTH, tempNode->fileName);
    dirName = tc2s(tDirName);
    tempNode = tempNode->parentDir;
    tempNode->subDir->erase(dirName);       //���ϼ�Ŀ¼ɾ����Ŀ¼
    cout << "---Ŀ¼��ɾ����" << endl;
    printer.printStar();
    log << "Ŀ¼��ɾ����" << endl;
    log << "-----------------" << endl;

    return;
}

/*��ӡɨ�̵õ�����Ϣ*/
void disk::showDiskInfo()
{
    printer.printStar();
    cout << "---ɨ��Ŀ¼��";
    _tprintf(_T("%s\n"), root->fileName);
    cout << "---Ŀ¼����" << dirCount << endl;
    cout << "---�ļ�����" << dirCount + fileCount << endl;
    cout << "---Ŀ¼������" << dirDepth << endl;
    cout << "---Ŀ¼����ȣ�" << treeDepth << endl;
    cout << "---��ļ�·����" << tc2s(longestPath) << endl;
    cout << "---��ļ�·�����ȣ�" << _tcslen(longestPath) << endl;
    printer.printStar();

    return;
}

/*���Ҳ���ӡָ��Ŀ¼���ļ���Ϣ*/
void disk::showDirInfo(const TCHAR* dirPath)
{
    char timeStr[MAX_LENGTH];               //�洢ʱ��
    int dirFileCount;                       //�洢�ļ�����
    ULONGLONG dirFileSize;                  //�洢�ļ��ܴ�С
    nodePtr tempNode;                       //�洢Ŀ¼�ڵ�
    nodePtr eFile, lFile;                   //�洢����/���������ļ��ڵ�

    /*��ѯĿ¼�ڵ�λ��*/
    time_t now = time(nullptr);
    ctime_s(timeStr, MAX_LENGTH, &now);
    log << "TIME:" << timeStr;
    tempNode = findFileNode(dirPath, _DIR);
    printer.printStar();
    cout << "---Ŀ��Ŀ¼��" << tc2s(dirPath) << endl;
    log << "Ŀ��Ŀ¼��" << tc2s(dirPath) << endl;

    /*��������������������ڸ�Ŀ¼*/
    if (tempNode == nullptr)
    {
        cout << "---Ŀ��Ŀ¼�����ڣ�" << endl;
        printer.printStar();
        log << "Ŀ��Ŀ¼�����ڣ�" << endl;
        log << "-----------------" << endl;
        return;
    }

    /*��ȡ�ļ���Ϣ*/
    dirFileCount = 0;
    dirFileSize = 0;
    eFile = tempNode->subDir->begin()->second;
    lFile = tempNode->subDir->begin()->second;
    for (fileChild::iterator iter = tempNode->subDir->begin(); iter != tempNode->subDir->end(); iter++)    //����Ѱ������/����ʱ���ļ�
    {
        if (iter->second->fileType == _FILE)
        {
            if (iter->second->creationUTCTime < eFile->creationUTCTime)
                eFile = iter->second;
            if (iter->second->creationUTCTime > lFile->creationUTCTime)
                lFile = iter->second;
            dirFileCount++;
            dirFileSize += iter->second->fileSize;
        }
    }
    cout << "---�ļ�������" << dirFileCount << endl;
    cout << "---�ļ��ܴ�С��" << dirFileSize << endl;
    cout << "---���紴���ļ���" << tc2s(eFile->fileName) << endl;
    ctime_s(timeStr, MAX_LENGTH, &(eFile->creationUTCTime));
    cout << "\t����ʱ�䣺" << static_cast<string>(timeStr);
    cout << "\t�ļ���С��" << eFile->fileSize << endl;
    cout << "---�������ļ���" << tc2s(lFile->fileName) << endl;
    ctime_s(timeStr, MAX_LENGTH, &(lFile->creationUTCTime));
    cout << "\t����ʱ�䣺" << static_cast<string>(timeStr);
    cout << "\t�ļ���С��" << lFile->fileSize << endl;
    printer.printStar();
    log << "�ļ�������" << dirFileCount << endl;
    log << "�ļ��ܴ�С��" << dirFileSize << endl;
    log << "���紴���ļ���" << tc2s(eFile->fileName) << endl;
    ctime_s(timeStr, MAX_LENGTH, &(eFile->creationUTCTime));
    log << "\t����ʱ�䣺" << static_cast<string>(timeStr);
    log << "\t�ļ���С��" << eFile->fileSize << endl;
    log << "�������ļ���" << tc2s(lFile->fileName) << endl;
    ctime_s(timeStr, MAX_LENGTH, &(lFile->creationUTCTime));
    log << "\t����ʱ�䣺" << static_cast<string>(timeStr);
    log << "\t�ļ���С��" << lFile->fileSize << endl;
    log << "-----------------" << endl;

    return;
}

/*���Ҳ���ӡָ���ļ��Ļ�����Ϣ*/
void disk::showFileInfo(const TCHAR* filePath)
{
    shared_ptr<fileNode> tempNode;      //�ݴ��ļ��ڵ�
    char timeStr[MAX_LENGTH];           //�洢����ʱ���ַ���
    time_t now = time(nullptr);         //�洢��ǰʱ��

    ctime_s(timeStr, MAX_LENGTH, &now);
    log << "TIME:" << timeStr;
    tempNode = findFileNode(filePath, _FILE);   //����Ŀ���ļ��ڵ�
    printer.printStar();
    cout << "---Ŀ���ļ���" << tc2s(filePath) << endl;
    log << "Ŀ���ļ���" << tc2s(filePath) << endl;

    if (tempNode == nullptr)
    {
        cerr << "---Ŀ���ļ������ڣ�" << endl;
        printer.printStar();
        log << "Ŀ���ļ������ڣ�" << endl;
        log << "-----------------" << endl;
        return;
    }
    ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
    cout << "---�ļ���С��" << tempNode->fileSize << endl;
    cout << "---�ļ�����ʱ�䣺" << timeStr;
    printer.printStar();
    log << "�ļ���С��" << tempNode->fileSize << endl;
    log << "�ļ�����ʱ�䣺" << timeStr;
    log << "-----------------" << endl;

    return;
}