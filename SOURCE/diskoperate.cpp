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

/*结构体默认构造函数*/
disk::fileNode::fileNode() :subDir(nullptr), parentDir(nullptr)
{
    fileSize = 0;
    fileName[0] = 0;
    creationUTCTime = { 0 };
    fileType = _DIR;
    dirt = F;
    creationUTCTime = 0;
}

/*结构体复制构造函数*/
disk::fileNode::fileNode(const fileNode& afileNode) :subDir(afileNode.subDir), parentDir(afileNode.parentDir)
{
    fileType = afileNode.fileType;
    fileSize = afileNode.fileSize;
    _tcscpy_s(fileName, MAX_LENGTH, afileNode.fileName);
    dirt = afileNode.dirt;
    creationUTCTime = afileNode.creationUTCTime;
}

/*结构体赋值运算符函数*/
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

/*磁盘类默认构造函数*/
disk::disk() :dirCount(0), fileCount(0), dirDepth(0), treeDepth(0), root(make_shared<fileTree>())
{
    longestPath[0] = 0;
    log.open("DISKLOG.log", ios::out | ios::app);
}

/*查询指定文件的位置*/
disk::nodePtr disk::findFileNode(const TCHAR* path, const TYPE mode) const
{
    vector<string> strs;                    //存储每一段路径
    string strPath, pathStr;                //存储转化后的目录路径
    TCHAR pathArr[MAX_LENGTH];
    shared_ptr<fileNode> tempNode;          //暂存目录树节点
    int i, j;

    /*按键寻找对应目录*/
    i = j = 0;
    strPath = tc2s(path);
    strSplit(strPath, strs, '\\');
    strPath = "";
    tempNode = root;
    do      //首先寻找根路径
    {
        strPath += strs[i++] + "\\";
    } while (_stricmp(strPath.c_str(), tc2s(root->fileName).c_str()) && i < strs.size());
    j = i;
    if (_stricmp(strPath.c_str(), tc2s(root->fileName).c_str())) //根路径错误，返回空指针
        return nullptr;
    else if (strs.size() == 2)  //查找的就是根路径，返回根节点
        return root;
    else                        //构造首个子目录路径
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

    for (auto iter = tempNode->subDir->find(strPath); iter != tempNode->subDir->end(); iter = tempNode->subDir->find(strPath))  //遍历深入查找文件
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

/*计算目录树深度*/
int disk::depthOfTree() const
{
    /*变量声明*/
    nodePtr tempNode;           //暂存文件节点
    fileChild::iterator iter;   //fileChilds迭代器
    stack<nodePtr> nodeStack;   //节点栈，用于模拟递归求深度
    int treeDepth = -1;         //存储树深度

    tempNode = root;

    do
    {
        switch (tempNode->fileType)
        {
            /*当前文件是目录*/
        case _DIR:
            if (tempNode->dirt == tempNode->T)  //目录已经被遍历过，出栈
            {
                nodeStack.pop();
                if (nodeStack.empty())
                    return treeDepth;
                else
                    tempNode = nodeStack.top();
            }
            else                                //目录未被遍历，将目录的所有子目录入栈
            {
                nodeStack.push(tempNode);
                tempNode->dirt = tempNode->T;
                for (iter = tempNode->subDir->begin(); iter != tempNode->subDir->end(); iter++)
                    nodeStack.push(iter->second);
                treeDepth = max(treeDepth, static_cast<int>(nodeStack.size()));
                tempNode = nodeStack.top();
            }
            break;
            /*当前文件是文件*/
        case _FILE:
            nodeStack.pop();        //不存在子目录，直接出栈
            if (nodeStack.empty())
                return treeDepth;
            else
                tempNode = nodeStack.top();
            break;
        }
    } while (!nodeStack.empty());

    return treeDepth;
}

/*扫盘函数，采用深度优先遍历，利用栈进行，若扫描到目录，则深入目录搜索，若扫描的是文件，则输出文件信息并扫描下一文件，扫盘的同时构建目录树、生成SQL文件*/
void disk::scanAndBuild(const TCHAR* rootPath, string& sqlFileName)
{
    _tsetlocale(LC_ALL, _T("chs"));     //添加设置，否则无法输出中文

    /*变量声明*/
    enum SIGNAL { T, F };               //指示此时是否可以继续操作
    TYPE type = _DIR;                   //文件类型标识量
    SIGNAL sign = T;                    //栈操作标识量
    WIN32_FIND_DATA findFileData;       //扫描得到的文件信息
    TCHAR path[MAX_LENGTH];             //存储路径，作为辅助数组帮助路径名存入路径名栈
    stack<TCHAR*> pathStack;            //路径名栈，存储不同层次的路径名，辅助模拟递归
    stack<HANDLE> progStack;            //程序栈，存储文件句柄，模拟递归
    nodePtr newNode;                    //用于临时存储新节点
    nodePtr tempNode;                   //用于指向当前节点
    int sqlCount = 1;                   //记录SQL语句的条目数量，数量过大时打开新的文件
    int outputCount = 1;                //记录输出文件个数
    char timeStr[MAX_LENGTH];           //存储时间字符串
    fstream fout;                       //输出SQL文件的文件流
    string outputFileRoot;              //输出文件名，当输出文件多于一个时需要用来作为辅助变量构建文件名
    string outputFileName;              //第一个输出文件的文件名
    int ox, oy, x, y;
    int count = 10000;
    auto judgeFile = [&sqlCount, &outputCount, &fout, &outputFileName, &outputFileRoot]()             //判断是否应该打开新文件的lambda表达式
        {
            if (sqlCount % MAX_COUNT)   //文件中SQL语句小于MAX_COUNT条，继续向当前文件写入
                sqlCount++;
            else                        //当一个文件中的SQL语句数量大于10000条后
            {
                string num;
                fout.close();   //关闭文件
                num = to_string(outputCount++ + 1) + ".sql";   //生成新的文件名
                outputFileName = outputFileRoot + num;
                fout.open(outputFileName, std::ios::out);      //打开新的文件
                fout << "use SYSTEM_DISK;" << endl;
                sqlCount++;
            }
        };

    /*函数必要变量初始化*/
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
    pathStack.push(new TCHAR[_tcslen(path) + 1]);                   //初始路径入路径栈
    _tcscpy_s(pathStack.top(), _tcslen(path) + 1, path);
    progStack.push(FindFirstFile(pathStack.top(), &findFileData));  //初始句柄入程序栈
    root->fileType = _DIR;                                          //初始化根路径节点
    path[_tcslen(path) - 1] = 0;
    _tcscpy_s(root->fileName, MAX_LENGTH, path);
    path[_tcslen(path)] = '*';
    root->parentDir = nullptr;
    root->subDir = make_shared<fileChild>();
    root->dirt = root->F;
    tempNode = root;
    fout.open(outputFileName, ios::out);                            //打开输出文件
    fout << "use SYSTEM_DISK;" << endl;                             //输入使用数据库语句
    time_t now = time(nullptr);
    ctime_s(timeStr, MAX_LENGTH, &now);
    dirCount = fileCount = dirDepth = treeDepth = 0;
    printer.printStar();
    cout << "---扫描目录：";
    _tprintf(_T("%s\n"), rootPath);
    log << "TIME:" << timeStr;
    log << "扫描目录：" << tc2s(rootPath) << endl;

    /*处理特殊情况，即扫描的盘不可访问*/
    if (progStack.top() == INVALID_HANDLE_VALUE)
    {
        cerr << "---目录不可访问！" << endl;
        printer.printStar();
        log << "目录不可访问！" << endl;
        log << "-----------------" << endl;
        return;
    }

    /*扫盘函数主体*/
    cout << "---当前已扫描文件数：";
    getCursorPosition(ox, oy);
    cout << endl;
    printer.printStar();
    getCursorPosition(x, y);
    while (progStack.size() > 0)
    {
        /*处理异常情况，执行出栈操作*/
        while (sign == F)
        {
            sign = T;
            if (dirDepth < progStack.size())       //判断是否为最大深度
                dirDepth = static_cast<int>(progStack.size());
            /*写入SQL文件*/
            judgeFile();
            TCHAR* path = pathStack.top();
            path[_tcslen(path) - 1] = 0;
            fout << "update DIRECTORIES set Ddepth=" << to_string(static_cast<int>(progStack.size())) << ",Dfcount=";  //向SQL文件写入SQL语句
            fout << to_string(static_cast<int>(tempNode->subDir->size()));
            fout << " where Dpath=\'" << tc2s(path) << "\';" << endl;
            /*执行出栈操作*/
            delete[] pathStack.top();           //路径搜索完毕，出栈前先释放内存
            pathStack.pop();
            FindClose(progStack.top());         //句柄已经搜索完成，出栈前需要关闭句柄
            progStack.pop();
            tempNode = tempNode->parentDir;     //由于当前指向的节点已经出栈，此时应该指向亲人节点
            if (progStack.size() <= 0)          //若栈已空，直接返回
            {
                fout.close();
                treeDepth = depthOfTree();
                ctime_s(timeStr, MAX_LENGTH, &now);
                log << "目录数：" << dirCount << endl;
                log << "文件数：" << dirCount + fileCount << endl;
                log << "目录层数：" << dirDepth << endl;
                log << "目录树深度：" << treeDepth << endl;
                log << "最长文件路径：" << tc2s(longestPath) << endl;
                log << "最长文件路径长度：" << _tcslen(longestPath) << endl;
                log << "-----------------" << endl;
                setCursorPosition(ox, oy);
                cout << dirCount + fileCount;
                setCursorPosition(x, y);

                return;
            }
            if (FindNextFile(progStack.top(), &findFileData) == 0)      //若栈顶目录也无下一文件可扫描，设置SIGNAL为F
                sign = F;
        }
        /*排除两种特殊情况*/
        while (_tcscmp(findFileData.cFileName, _T(".")) == 0 || _tcscmp(findFileData.cFileName, _T("..")) == 0)
        {
            if (FindNextFile(progStack.top(), &findFileData) == 0)      //扫描当前目录的下一个文件，若没有下一个文件，则标记SIGNAL枚举量为F，表示此时不应进行任何操作
            {
                sign = F;
                break;
            }
        }
        /*检查当前文件是目录还是文件*/
        if (sign != F && findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            type = _DIR;        //是目录
        else
            type = _FILE;       //是文件
        /*针对不同文件类型执行不同的遍历操作*/
        if (sign != F)
        {
            switch (type)
            {
                /*当前扫描的是目录*/
            case _DIR:
                dirCount++;     //目录计数器递增
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
                _stprintf_s(subDir, MAX_LENGTH, _T("%s%s\\*"), path, findFileData.cFileName);   //存储子目录路径
                /*写入SQL文件*/
                judgeFile();
                subDir[_tcslen(subDir) - 1] = 0;
                fout << "insert into DIRECTORIES(Dname,Dpath) values(" << "\'" << tc2s(findFileData.cFileName) << "\',";   //向SQL文件写入SQL语句
                fout << "\'" << tc2s(subDir) << "\');" << endl;
                /*由于需要深入遍历，相当于此时需要执行递归，故进行栈操作*/
                subDir[_tcslen(subDir)] = '*';
                pathStack.push(new TCHAR[_tcslen(subDir) + 1]);         //目录路径入栈
                _tcscpy_s(pathStack.top(), _tcslen(subDir) + 1, subDir);
                progStack.push(FindFirstFile(subDir, &findFileData));   //目录句柄入栈
                /*构建子目录节点*/
                subDir[_tcslen(subDir) - 1] = 0;
                newNode = make_shared<fileNode>();
                newNode->fileType = _DIR;                        //文件类型设为目录
                _tcscpy_s(newNode->fileName, MAX_LENGTH, subDir);  //存储文件名
                newNode->fileSize = ((static_cast<ULONGLONG>(findFileData.nFileSizeHigh) << 32)) | findFileData.nFileSizeLow;
                newNode->creationUTCTime = ft2t(findFileData.ftCreationTime);
                newNode->parentDir = tempNode;                   //设置文件亲代节点
                newNode->subDir = make_shared<fileChild>();     //为子目录存储的文件节点分配空间
                tempNode->subDir->insert(make_pair(tc2s(subDir), newNode));            //将该节点存入亲代的子文件容器中
                tempNode = newNode;                              //深度优先，下一个遍历的节点是该目录下的节点，tempNode指向该目录节点
                if (progStack.top() == INVALID_HANDLE_VALUE)     //当前路径下的文件不可读，则直接标识SIGNAL，等待进入下一循环进行处理
                {
                    sign = F;
                    break;
                }
                break;

                /*当前扫描的是文件*/
            case _FILE:
                fileCount++;      //文件计数器递增
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
                /*构建文件节点*/
                _stprintf_s(filePath, MAX_LENGTH, _T("%s%s"), path, findFileData.cFileName);
                newNode = make_shared<fileNode>();
                newNode->fileType = _FILE;                                       //文件类型设为文件
                _tcscpy_s(newNode->fileName, MAX_LENGTH, filePath);              //存储路径名
                newNode->parentDir = tempNode;                                   //存储亲代节点
                newNode->subDir = nullptr;                                       //由于是文件，没有子目录，设为空指针
                newNode->creationUTCTime = ft2t(findFileData.ftCreationTime);    //存储文件创建时间
                newNode->fileSize = ((static_cast<ULONGLONG>(findFileData.nFileSizeHigh) << 32)) | findFileData.nFileSizeLow;    //存储文件大小
                tempNode->subDir->insert(make_pair(tc2s(filePath), newNode));                            //将该节点存入亲代的子文件容器
                /*写入SQL文件*/
                judgeFile();
                fout << "insert into FILES values(" << "\'" << tc2s(findFileData.cFileName) << "\',";
                fout << "\'" << tc2s(filePath) << "\'," << "\'" << tc2s(path) << "\'," << to_string(newNode->fileSize) << ",";
                fout << to_string(ft2t(findFileData.ftCreationTime));
                fout << ");" << endl;
                if (_tcslen(longestPath) < _tcslen(filePath))           //比较路径名长度，求最长全路径名
                    _tcscpy_s(longestPath, MAX_LENGTH, filePath);
                if (FindNextFile(progStack.top(), &findFileData) == 0)  //当前已经到达最深，继续扫描下一文件，若无下一文件，则标识SIGNAL
                    sign = F;
                break;
            }
        }
    }

    fout.close();
    treeDepth = depthOfTree();
    ctime_s(timeStr, MAX_LENGTH, &now);
    log << "目录数：" << dirCount << endl;
    log << "文件数：" << dirCount + fileCount << endl;
    log << "目录层数：" << dirDepth << endl;
    log << "目录树深度：" << treeDepth << endl;
    log << "最长文件路径：" << tc2s(longestPath) << endl;
    log << "最长文件路径长度：" << _tcslen(longestPath) << endl;
    log << "-----------------" << endl;
    setCursorPosition(ox, oy);
    cout << dirCount + fileCount;
    setCursorPosition(x, y);

    return;
}

/*管理文件信息*/
void disk::manageFileInfo(string& command)
{
    vector<string> commandPart;     //存储将指令拆分为多部分的容器
    shared_ptr<fileNode> tempNode;  //暂存文件节点
    time_t now;                     //存储当前时间
    TCHAR* tempPath;
    char timeStr[MAX_LENGTH];

    strSplit(command, commandPart, ',');
    tempNode = findFileNode(s2tc(commandPart[0]), _FILE);
    now = time(nullptr);
    ctime_s(timeStr, MAX_LENGTH, &now);

    printer.printStar();
    cout << "---目标文件：" << commandPart[0] << endl;
    log << "TIME:" << timeStr;
    log << "目标文件：" << commandPart[0] << endl;

    /*命令执行主体*/
    switch (commandPart[1][0])
    {
        /*删除操作*/
    case 'D':
        if (tempNode == nullptr)
        {
            cerr << "---目标文件不存在！" << endl;
            printer.printStar();
            log << "目标文件不存在！" << endl;
            log << "-----------------" << endl;
            return;
        }
        else
        {
            ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
            cout << "---原文件大小：" << tempNode->fileSize << endl;
            cout << "---原文件创建时间：" << timeStr;
            cout << "---执行删除操作" << endl;
            log << "原文件大小：" << tempNode->fileSize << endl;
            log << "原文件创建时间：" << timeStr;
            log << "执行删除操作" << endl;
            /*删除指定节点*/
            tempNode = tempNode->parentDir;
            tempNode->parentDir->subDir->erase(commandPart[0]);
            cout << "---文件已删除！" << endl;
            printer.printStar();
            log << "文件已删除！" << endl;
            log << "-----------------" << endl;
        }
        break;
        /*修改操作*/
    case 'M':
        if (tempNode == nullptr)
        {
            cerr << "目标文件不存在！" << endl;
            cout << "---执行删除操作" << endl;
            log << "目标文件不存在！" << endl;
            log << "-----------------" << endl;
            return;
        }
        else
        {
            ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
            cout << "---原文件大小：" << tempNode->fileSize << endl;
            cout << "---原文件创建时间：" << timeStr;
            cout << "---执行修改操作" << endl;
            log << "原文件大小：" << tempNode->fileSize << endl;
            log << "原文件创建时间：" << timeStr;
            log << "执行修改操作" << endl;
            /*修改指定节点*/
            tempNode->creationUTCTime = stoll(commandPart[2]);
            tempNode->fileSize = stoull(commandPart[3]);
            ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
            cout << "---现文件大小：" << tempNode->fileSize << endl;
            cout << "---现文件创建时间：" << timeStr;
            printer.printStar();
            log << "现文件大小：" << tempNode->fileSize << endl;
            log << "现文件创建时间：" << timeStr;
            log << "-----------------" << endl;
        }
        break;
        /*增加操作*/
    case 'A':
        cout << "---原文件不存在" << endl;
        cout << "---执行增加操作" << endl;
        vector<string> strs;
        strSplit(commandPart[0], strs, '\\');
        string path = strs[0] + "\\";
        for (int i = 1; i < strs.size() - 1; i++)
            path += strs[i] + "\\";
        tempPath = s2tc(path);
        tempNode = findFileNode(tempPath, _DIR); //查找该文件的上级目录
        delete[] tempPath;
        if (tempNode == nullptr)
        {
            cerr << "---目标上级目录不存在！" << endl;
            printer.printStar();
            log << "目标上级目录不存在！" << endl;
            log << "-----------------" << endl;
            return;
        }
        /*创建新文件节点*/
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
        cout << "---新文件大小：" << tempNode->fileSize << endl;
        cout << "---新文件创建时间：" << timeStr;
        printer.printStar();
        log << "新文件大小：" << tempNode->fileSize << endl;
        log << "新文件创建时间：" << timeStr;
        log << "-----------------" << endl;
        break;
    }

    return;
}

/*管理目录信息*/
void disk::manageDirInfo(string& command)
{
    vector<string> commandPart;     //存储将指令拆分为多部分的容器
    shared_ptr<fileNode> tempNode;  //暂存文件节点
    time_t now;                     //存储当前时间
    string dirName;                 //目录名
    TCHAR tDirName[MAX_LENGTH];     //TCHAR格式目录名
    TCHAR* tempPath;
    char timeStr[MAX_LENGTH];

    strSplit(command, commandPart, ',');
    tempPath = s2tc(commandPart[0]);
    tempNode = findFileNode(tempPath, _DIR);    //查询目录节点位置
    delete[] tempPath;
    now = time(nullptr);
    ctime_s(timeStr, MAX_LENGTH, &now);

    printer.printStar();
    cout << "---目标目录：" << commandPart[0] << endl;
    log << "TIME:" << timeStr;
    log << "目标目录：" << commandPart[0] << endl;

    if (tempNode == nullptr)
    {
        cerr << "---目标目录不存在！" << endl;
        printer.printStar();
        log << "目标目录不存在！" << endl;
        log << "-----------------" << endl;
        return;
    }

    _tcscpy_s(tDirName, MAX_LENGTH, tempNode->fileName);
    dirName = tc2s(tDirName);
    tempNode = tempNode->parentDir;
    tempNode->subDir->erase(dirName);       //从上级目录删除子目录
    cout << "---目录已删除！" << endl;
    printer.printStar();
    log << "目录已删除！" << endl;
    log << "-----------------" << endl;

    return;
}

/*打印扫盘得到的信息*/
void disk::showDiskInfo()
{
    printer.printStar();
    cout << "---扫描目录：";
    _tprintf(_T("%s\n"), root->fileName);
    cout << "---目录数：" << dirCount << endl;
    cout << "---文件数：" << dirCount + fileCount << endl;
    cout << "---目录层数：" << dirDepth << endl;
    cout << "---目录树深度：" << treeDepth << endl;
    cout << "---最长文件路径：" << tc2s(longestPath) << endl;
    cout << "---最长文件路径长度：" << _tcslen(longestPath) << endl;
    printer.printStar();

    return;
}

/*查找并打印指定目录的文件信息*/
void disk::showDirInfo(const TCHAR* dirPath)
{
    char timeStr[MAX_LENGTH];               //存储时间
    int dirFileCount;                       //存储文件总数
    ULONGLONG dirFileSize;                  //存储文件总大小
    nodePtr tempNode;                       //存储目录节点
    nodePtr eFile, lFile;                   //存储最早/最晚创建的文件节点

    /*查询目录节点位置*/
    time_t now = time(nullptr);
    ctime_s(timeStr, MAX_LENGTH, &now);
    log << "TIME:" << timeStr;
    tempNode = findFileNode(dirPath, _DIR);
    printer.printStar();
    cout << "---目标目录：" << tc2s(dirPath) << endl;
    log << "目标目录：" << tc2s(dirPath) << endl;

    /*处理特殊情况，即不存在该目录*/
    if (tempNode == nullptr)
    {
        cout << "---目标目录不存在！" << endl;
        printer.printStar();
        log << "目标目录不存在！" << endl;
        log << "-----------------" << endl;
        return;
    }

    /*获取文件信息*/
    dirFileCount = 0;
    dirFileSize = 0;
    eFile = tempNode->subDir->begin()->second;
    lFile = tempNode->subDir->begin()->second;
    for (fileChild::iterator iter = tempNode->subDir->begin(); iter != tempNode->subDir->end(); iter++)    //遍历寻找最早/最晚时间文件
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
    cout << "---文件总数：" << dirFileCount << endl;
    cout << "---文件总大小：" << dirFileSize << endl;
    cout << "---最早创建文件：" << tc2s(eFile->fileName) << endl;
    ctime_s(timeStr, MAX_LENGTH, &(eFile->creationUTCTime));
    cout << "\t创建时间：" << static_cast<string>(timeStr);
    cout << "\t文件大小：" << eFile->fileSize << endl;
    cout << "---最晚创建文件：" << tc2s(lFile->fileName) << endl;
    ctime_s(timeStr, MAX_LENGTH, &(lFile->creationUTCTime));
    cout << "\t创建时间：" << static_cast<string>(timeStr);
    cout << "\t文件大小：" << lFile->fileSize << endl;
    printer.printStar();
    log << "文件总数：" << dirFileCount << endl;
    log << "文件总大小：" << dirFileSize << endl;
    log << "最早创建文件：" << tc2s(eFile->fileName) << endl;
    ctime_s(timeStr, MAX_LENGTH, &(eFile->creationUTCTime));
    log << "\t创建时间：" << static_cast<string>(timeStr);
    log << "\t文件大小：" << eFile->fileSize << endl;
    log << "最晚创建文件：" << tc2s(lFile->fileName) << endl;
    ctime_s(timeStr, MAX_LENGTH, &(lFile->creationUTCTime));
    log << "\t创建时间：" << static_cast<string>(timeStr);
    log << "\t文件大小：" << lFile->fileSize << endl;
    log << "-----------------" << endl;

    return;
}

/*查找并打印指定文件的基本信息*/
void disk::showFileInfo(const TCHAR* filePath)
{
    shared_ptr<fileNode> tempNode;      //暂存文件节点
    char timeStr[MAX_LENGTH];           //存储各种时间字符串
    time_t now = time(nullptr);         //存储当前时间

    ctime_s(timeStr, MAX_LENGTH, &now);
    log << "TIME:" << timeStr;
    tempNode = findFileNode(filePath, _FILE);   //查找目标文件节点
    printer.printStar();
    cout << "---目标文件：" << tc2s(filePath) << endl;
    log << "目标文件：" << tc2s(filePath) << endl;

    if (tempNode == nullptr)
    {
        cerr << "---目标文件不存在！" << endl;
        printer.printStar();
        log << "目标文件不存在！" << endl;
        log << "-----------------" << endl;
        return;
    }
    ctime_s(timeStr, MAX_LENGTH, &(tempNode->creationUTCTime));
    cout << "---文件大小：" << tempNode->fileSize << endl;
    cout << "---文件创建时间：" << timeStr;
    printer.printStar();
    log << "文件大小：" << tempNode->fileSize << endl;
    log << "文件创建时间：" << timeStr;
    log << "-----------------" << endl;

    return;
}