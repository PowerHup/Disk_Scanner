#pragma once
#include"formatprinter.h"
#include<fstream>
#include<iostream>
#include<memory>
#include<tchar.h>
#include<unordered_map>
#include<windows.h>

/*���������ƿռ䣬��ֹ��std�ռ��ͻ*/
namespace diskop
{
    const int MAX_LENGTH = 300;  //�·��������
    const int MAX_COUNT = 60000; //�����ļ����SQL�������
    class disk;                  //������
}

class diskop::disk
{
private:
    enum TYPE { _DIR, _FILE };          //���ڱ�ʶ��ʱ��ȡ���ļ���Ŀ¼�����ļ�
    struct fileNode;                    //�����ļ���Ϣ�ڵ�
    using fileTree = fileNode;
    using fileChild = std::unordered_map<std::string, std::shared_ptr<fileNode>>;   //�洢Ŀ¼�ڵ�ĺ��ӽڵ�
    using childPtr = std::shared_ptr<fileChild>;
    using nodePtr = std::shared_ptr<fileNode>;
    using treePtr = nodePtr;
    struct fileNode                     //�����ļ���Ϣ�ڵ�
    {
        enum DIRTBIT { T, F };
        TYPE fileType;                  //�ļ�����
        DIRTBIT dirt;                   //������ʶ����������Ŀ¼�����
        TCHAR fileName[MAX_LENGTH];     //�ļ���
        ULONGLONG fileSize;             //�ļ���С
        time_t creationUTCTime;         //�ļ�������ʱ�䣨UTC��ʽ��
        childPtr subDir;                //ָ����Ŀ¼��Ӧ���ֵܽ������������ǰ�ļ�������Ŀ¼��
        nodePtr parentDir;              //ָ���ϼ�Ŀ¼
        fileNode();                              //�ṹ��Ĭ�Ϲ��캯��
        fileNode(const fileNode& afileNode);     //�ṹ�帴�ƹ��캯��
        fileNode& operator=(const fileNode& afileNode);     //�ṹ�����ظ�ֵ�����
    };
    int dirCount, fileCount, dirDepth, treeDepth;           //�洢��Ŀ¼�����ļ��������������
    TCHAR longestPath[MAX_LENGTH];      //�洢�·����
    std::fstream log;                   //��־�ļ���������
    fprinter::formatprinter printer;    //������ӡ�Ķ���
    treePtr root;                       //��·���ڵ�,Ŀ¼���ĸ�
    nodePtr findFileNode(const TCHAR* path, const TYPE mode) const;    //����ָ���ļ���λ�ã�����ָ���ļ��ڵ��ָ��
    int depthOfTree() const;            //����Ŀ¼�����
    /*
    Ŀ¼���Զ��������ʽʵ�֣�ÿһ���ļ��ڵ��а���һ��ָ���ϼ��ļ���ָ���һ���¼��ļ����ָ�룬
    �¼��ļ��ڵ�洢�������unordered_map������Ϊ��ϣ���У����ļ�·����Ϊ�����ļ��ڵ�ṹ����Ϊֵ��
    ʹ��findFileNode()��Ա��������ͨ�����ι�ϣ���ѯ���ٲ�ѯ��Ŀ���ļ��ڵ�λ��
    */
public:
    disk(); //���캯��
    void scanAndBuild(const TCHAR* rootPath, std::string& sqlFileName);       //ɨ�̺���,ͬʱ����Ŀ¼��
    void manageFileInfo(std::string& command);      //�����ļ���Ϣ
    void manageDirInfo(std::string& command);       //����Ŀ¼��Ϣ
    void showDiskInfo();                            //��ӡɨ�̵õ�����Ϣ������������Ŀ¼�������ļ�����Ŀ¼������Ŀ¼�����
    void showDirInfo(const TCHAR* dirPath);         //��ӡָ��Ŀ¼���ļ���Ϣ
    void showFileInfo(const TCHAR* filePath);       //��ӡָ���ļ��Ļ�����Ϣ
};