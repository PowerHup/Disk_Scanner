# Disk_Scanner代码说明

**SOURCE文件夹中存放程序源码**

## diskoperate

1. diskoperate.h和diskoperate.cpp定义了程序使用的核心类disk；
2. disk类中有扫盘函数（采用windows.h的函数），提供了构建目录树、输出sql文件的功能；
3. 在扫盘完成后，可以调用disk类中的其他函数，按照一定的格式来操作目录树，达成模拟文件操作的效果。

## formatprinter

1. formatprinter.h和formatprinter.cpp定义了辅助构建命令行界面的类formatprinter；
2. formatprinter类中给出了打印格式界面、打印输入框并读取输入的函数。

## tools

1. tools.h和tools.cpp定义了程序中的工具函数；
2. 函数功能包括：string和TCHAR字符串的转换、清空控制台、获取/调整光标位置……

## 来自作者

第一次用C++编写较大的项目，代码非常简陋且略显臃肿，本人尽力解决了内存管理的问题，但是可能仍然存在少量内存未释放的问题，敬请谅解！
