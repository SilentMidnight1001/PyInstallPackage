

#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <string>
#include <winKeyPressH.hpp>

using namespace std;
namespace fs = std::filesystem;


class App
{

private:
    // pip运行 //
    bool runPip = true;
    string install_package;  // 安装包指令
    string pip = "python -m pip install ";  // pip指令，后期连接包名
    string passage_install = " -i https://pypi.tuna.tsinghua.edu.cn/simple";   // 镜像通道

    unsigned int package_install_num = 0;
    vector<string> packageList;  // 包名安装的列表

private:
    void runPipFunc();
    // 删除库 //
    void delPackage(const string &str);
    // 备份下载 //
    void copyPackageInstall();
    // 查看已经安装包的列表 //
    void findPackageInstall();
    // 写入包文件 //
    void writePackage(const string& str);
    // 读取文档查找是否重复,传入包名字对比 //
    bool readFilePackage(const string &str);

public:
    App();
};


// 开始死循环 //
void App::runPipFunc()
{
    while (runPip)
    {
        // 安装库的名称 //
        string pip_name;

        // 安装包的步骤
        system("color 7");
        printf("输入 说明 弹出使用方法\n");
        printf("请输入你要下载的包名字:");

        // 获取安装包名称 //
        cin>>pip_name;

        // 首先判断是否输入q退出 //
        if (pip_name == "q")
        {
            break;
        }

            // 更新pip的功能 //
        else if (pip_name == "更新pip")
        {
            system("python.exe -m pip install -i https://pypi.tuna.tsinghua.edu.cn/simple  --upgrade pip");
        }

        // 删除已经安装的库 //
        else if (pip_name == "del")
        {
            system("color 3");
            printf("请输入想要删除的库:");
            string del_package;
            cin>>del_package;  // 输入字符窜
            delPackage(del_package);  // 这个函数删除指定的库
        }

            // 使用说明 //
        else if(pip_name == "说明")
        {
            printf("1.本脚本只适用于镜像源安装库，只需要输入包名字就自动取下载如需卸载输入\n");
            printf("2.该脚本会在文件夹生成你已经安装过的包名称文档，输入: 备份 下载备份文档\n");
            printf("3.如需卸载输入 uninstall_this\n");
            printf("4.输入: del 可以删除安装的库\n");
            printf("5.输入: 更新pip 即可更新pip\n");
            printf("6.输入: 查看已下载的包 查看已经下载的包\n");
            printf("7.输入: 删除备份 查看已经下载的包\n");
            printf("8.输入q退出或者点击x退出\n");
            printf("\n");
        }

            // 查看已经备份的所有库 //
        else if (pip_name == "查看已下载的包")
        {
            findPackageInstall();  // 查看安装的包
        }

            // 备份已经安装的库 //
        else if (pip_name == "备份")
        {
            copyPackageInstall();
        }

        // 删除备份 //
        else if (pip_name == "删除备份")
        {
            wkp::Open ipt;
            ipt.open("requirements.txt", "r");
            if (!ipt.isOpen())
            {
                std::cerr<<"无法打开文件\n";
            }
            auto packageList = ipt.readLine();
            for (auto &line: packageList)
            {
                delPackage(line);
            }
        }

            // 进入到安装库的功能 //
        else
        {
            bool findPackageGain = readFilePackage(pip_name);  // 查找是否重复包
            if (!findPackageGain)  // 返回为假说明没有重复
            {
                system("color A");
                install_package = pip+pip_name+passage_install;
                int result = system(install_package.c_str());
                if (result)
                {
                    cerr<<"\n在安装包的过程中出现错误，可能的原因有:包的名称不存在 或者 其他问题\n";
                }
                else
                {
                    writePackage(pip_name);
                    cout<<install_package<<endl;
                    cout<<"包名 "<< pip_name<<" 已经下载并备份\n"<<endl;
                }
            }
            else
            {
                system("color 4");
                printf("包名 %s 已经下载\n\n", pip_name.c_str());
            }
        }
    }
}

// 删除库 //
void App::delPackage(const string &str)
{
    packageList.clear(); // 新增：清空vector防止数据残留
    wkp::Open readFile("requirements.txt", "r");
    if (!readFile.isOpen())
    {
        std::cerr<<"文件打开失败"<<endl;
    }
    else
    {
        // 逐行读取文件 //
        auto lineList = readFile.readLine();

        system("color 8");
        string remove_package_name = "python -m pip uninstall "+str;
        system(remove_package_name.c_str());
        printf("\n");

        for (auto &line: lineList)
        {
            // 如果不是要删除的包，放到容器中，再次写入文件 //
            if (line != str)
            {
                packageList.push_back(line);
            }
        }
        readFile.close();

        // 重新写入文件 //
        wkp::Open writeFile("requirements.txt", "w");
        if (!writeFile.isOpen())
        {
            std::cerr<<"文件打开失败"<<endl;
        }
        else
        {
            for (auto &i: packageList)
            {
                writeFile.write(i+"\n");
            }
            writeFile.close();
            packageList.clear(); // 可选：再次清空以备下次使用
        }
    }
}

// 备份下载 //
void App::copyPackageInstall()
{
    wkp::Open copyInstall("requirements.txt", "r");  // 只读
    if (!copyInstall.isOpen())
    {
        std::cerr<<"可能你还没有备份"<<endl;
    }
    else
    {
        auto copyList = copyInstall.readLine();
        package_install_num = copyList.size();

        for (auto &line: copyList)
        {
            printf("当前需要备份的库有 %d 个\n\n", package_install_num);
            system("color A");
            system((pip+line+passage_install).c_str());
            package_install_num--;
        }
    }
    copyInstall.close();  // 关闭文件
}

// 查看已经安装包的列表
void App::findPackageInstall()
{
    wkp::Open findPackage("requirements.txt", "r");
    if (!findPackage.isOpen())
    {
        std::cerr<<"可能你还没有备份"<<endl;
    }
    else
    {
        unsigned int num = 0;
        auto lineList = findPackage.readLine();
        num = lineList.size();
        for (auto &line: lineList)
        {
            fmt::println("包名 {}",  line);
        }
        printf("共有 %d 个包安装并备份\n\n", num);
    }
    findPackage.close();
}

void App::writePackage(const string &str)
{
    wkp::Open writeFile("requirements.txt", "a");
    if (!writeFile.isOpen())
    {
        std::cerr<<"文件打开失败"<<endl;
    }
    else
    {
        writeFile.write(str+"\n");
        writeFile.close();
    }
}

// 检查重复的包 //
bool App::readFilePackage(const string &str)
{
    wkp::Open inputFile("requirements.txt", "r");
    if (!inputFile.isOpen())
    {
        std::cerr << "Error opening file!" << std::endl;
        return false; // 如果文件打开失败，输出错误并退出程序,说明没有文件
    }
    auto pack_lst = inputFile.readLine();
    for (auto &line: pack_lst)
    {
        if (line == str)
        {
            inputFile.close();  // 关闭文件
            return true;
        }
    }
    inputFile.close();
    return false;
}

App::App()
{
    runPipFunc();
}


int main()
{
    // 检查是否为管理员 //
    if (!wkp::isAdmin())
    {
        // 获取管理员 //
        if (wkp::openAdmin())
        {
            return 0; // 结束当前非管理员进程
        }
        else
        {
            std::cerr << "提权失败，程序将继续以普通权限运行" << std::endl;
            return -1;
        }
    }
    wkp::useConsoleUtf8();
    App app;

    return 0;
}
