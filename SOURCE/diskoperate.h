#pragma once
#include"formatprinter.h"
#include<fstream>
#include<iostream>
#include<memory>
#include<tchar.h>
#include<unordered_map>
#include<windows.h>

/*磁盘类名称空间，防止与std空间冲突*/
namespace diskop
{
    const int MAX_LENGTH = 300;  //最长路径名长度
    const int MAX_COUNT = 60000; //单个文件最大SQL语句条数
    class disk;                  //磁盘类
}

class diskop::disk
{
private:
    enum TYPE { _DIR, _FILE };          //用于标识此时读取的文件是目录还是文件
    struct fileNode;                    //声明文件信息节点
    struct statData;                    //声明统计信息节点
    using fileTree = fileNode;
    using fileChild = std::unordered_map<std::string, std::shared_ptr<fileNode>>;   //存储目录节点的孩子节点
    using statInfo = std::unordered_map<std::string, statData>;                     //存储统计信息
    using childPtr = std::shared_ptr<fileChild>;
    using nodePtr = std::shared_ptr<fileNode>;
    using treePtr = nodePtr;
    struct fileNode                     //定义文件信息节点
    {
        enum DIRTBIT { T, F };
        TYPE fileType;                  //文件类型
        DIRTBIT dirt;                   //遍历标识，辅助计算目录树深度
        TCHAR fileName[MAX_LENGTH];     //文件名
        ULONGLONG fileSize;             //文件大小
        time_t creationUTCTime;         //文件创建的时间（UTC格式）
        childPtr subDir;                //指向子目录对应的兄弟姐妹链表（如果当前文件类型是目录）
        nodePtr parentDir;              //指向上级目录
        fileNode();                              //结构体默认构造函数
        fileNode(const fileNode& afileNode);     //结构体复制构造函数
        fileNode& operator=(const fileNode& afileNode);     //结构体重载赋值运算符
    };
    struct statData                         //定义统计信息结构
    {
        int totalFileCount;                 //文件总数
        ULONGLONG totalFileSize;            //文件总大小
        TCHAR earlistFileName[MAX_LENGTH];  //最早创建文件名
        time_t earlistFileCreationTime;     //最早创建时间
        ULONGLONG earlistFileSize;          //最早文件大小
        TCHAR latestFileName[MAX_LENGTH];   //最晚创建文件名
        time_t latestFileCreationTime;      //最晚创建时间
        ULONGLONG latestFileSize;           //最晚文件大小
        statData();                         //结构体默认构造函数
        statData(const statData& astatData);//结构体复制构造函数
        statData& operator=(const statData& astatData); //结构体重载赋值运算符
        bool compareDir(const statData& astatData) const;   //比较目录信息
        bool compareEarlistFile(const statData& astatData) const;   //比较最早文件信息
        bool compareLatestFile(const statData& astatData) const;    //比较最晚文件信息
        bool operator==(const statData& astatData) const;           //结构体重载相等运算符
    };
    int dirCount, fileCount, errorCount, dirDepth, treeDepth;       //存储子目录数，文件数，无权限数，层数，深度
    TCHAR longestPath[MAX_LENGTH];      //存储最长路径名
    std::fstream log;                   //日志文件的流对象
    fprinter::formatprinter printer;    //辅助打印的对象
    treePtr root;                       //根路径节点,目录树的根
    statInfo statTable;                 //存储统计信息
    nodePtr findFileNode(const TCHAR* path, const TYPE mode) const;    //查找指定文件的位置，返回指向文件节点的指针
    int depthOfTree() const;            //计算目录树深度
    /*
    目录树以多叉树的形式实现，每一个文件节点中包含一个指向上级文件的指针和一个下级文件表的指针，
    下级文件节点存储在无序表（unordered_map，本质为哈希表）中，以文件路径作为键，文件节点结构体作为值，
    使用findFileNode()成员函数可以通过几次哈希表查询快速查询到目标文件节点位置
    */
public:
    disk(); //构造函数
    ~disk();//析构函数
    void scanAndBuild(const TCHAR* rootPath, std::string& sqlFileName); //扫盘函数,同时构建目录树
    void manageFileInfo(std::string& command);      //管理文件信息
    void manageDirInfo(std::string& command);       //管理目录信息
    void showDiskInfo();                            //打印扫盘得到的信息，包括所有子目录数量，文件数，目录层数，目录树深度
    bool showDirInfo(const TCHAR* dirPath);         //打印指定目录的文件信息
    void showFileInfo(const TCHAR* filePath);       //打印指定文件的基本信息
};