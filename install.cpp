#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <string>
#include <winKeyPressH.hpp>

using namespace std;
using namespace wkp;
namespace fs = std::filesystem;


class App
{

private:
    // pip运行 //
    string installCommand = R"(python -m pip install {} -i {})";
    string updatePipCommand = R"(python -m pip install --upgrade pip -i {})";
    string installUrl;

    // 运行标志 //
    bool runPipFlag = true;

    // 包名安装的列表 //
    vector<string> packageList;

private:
    void runPipFunc();
    // 读取镜像源 //
    bool readUrlFunc();
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
    // 更换镜像源 //
    void changeUrlFunc();

public:
    App()
    {
        runPipFunc();
    }
};

// 读取镜像源 //
bool App::readUrlFunc()
{
    // 读取镜像源地址 //
    wkp::Open ipt("./url.txt", "r");
    if (!ipt.isOpen())
    {
        println(Color("green"), "文件未创建，自动创建镜链接文件!");

        // 存入镜像源 //
        print(Color("green"), "首次使用需要输入镜像源网址:");
        cin>>installUrl;

        // 写入文本 //
        wkp::Open opt("./url.txt", "w");
        if (opt.isOpen())
        {
            opt.write(installUrl);
            return true;
        }
    }
        // 如果文本存在 //
    else
    {
        installUrl = ipt.read();
        if (!installUrl.empty())
        {
            return true;
        }
        fs::remove("./url.txt");
    }
    return false;
}

bool App::readFilePackage(const string &str)
{
    wkp::Open inputFile("requirements.txt", "r");
    if (!inputFile.isOpen())
    {
        std::cerr << "Error opening file!" << std::endl;
        wkp::sleep(5);
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

void App::changeUrlFunc()
{
    print(Color("red"), "请输入更换镜像源的网址:");
    installUrl = "";
    cin>>installUrl;

    if (!installUrl.empty())
    {
        wkp::Open opt("./url.txt", "r");
        opt.write(installUrl);
        wkp::println("");
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
        auto copyListCount = copyList.size();


        for (auto &line: copyList)
        {
            println(Color("green"), "前需要备份的库有 {} 个\n", copyListCount);
            system(wkp::format(installCommand, line, installUrl).c_str());
            copyListCount--;
        }
        println("");
    }
    copyInstall.close();  // 关闭文件
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
            wkp::println(Color("green"), "包名 {}",  line);
        }
        wkp::println(Color("#00FFFF"), "共有 %d 个包安装并备份\n");
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

// 运行文件 //
void App::runPipFunc()
{
    // 先读镜像源 //
    bool haveUrl = readUrlFunc();
    if (!haveUrl)
    {
        println(Color("red"), "当前 haveUrl值为 {} 请输入镜像源后再次运行!");
        return;
    }

    while (runPipFlag)
    {
        // 安装库的名称 //
        string packageName;

        // 安装包的步骤 //
        println(Color("green"), "输入 说明 弹出使用方法");
        print(Color("green"), "请输入你要下载的包名字:");

        // 获取安装包名称 //
        cin>>packageName;

        println("");

        // 输入 q 退出 //
        if (packageName == "q")
        {
            return;
        }

        // 更新pip //
        else if (packageName == "更新pip")
        {
            string updatePip = wkp::format(updatePipCommand, installUrl);
            system(updatePip.c_str());
            println("");
        }

        // 删除已经安装的库 //
        else if (packageName == "del")
        {
            while (true)
            {
                string del_package;
                print(Color("blue"), "请输入想要删除的库:");
                cin>>del_package;  // 输入字符窜
                if (del_package == "q")
                {
                    println("");
                    break;
                }
                delPackage(del_package);  // 这个函数删除指定的库
            }
        }

        // 更换镜像源 //
        else if(packageName == "更换镜像源")
        {
            changeUrlFunc();
        }

        // 使用说明 //
        else if(packageName == "说明")
        {
            string printStr = R"(
1.本脚本只适用于镜像源安装库，只需要输入包名字就自动取下载如需卸载输入
2.该脚本会在文件夹生成你已经安装过的包名称文档，输入: 备份 下载备份文档
3.如需卸载输入 uninstall_this
4.输入: del 可以删除安装的库
5.输入: 更换镜像源 可以更换镜像源网址
6.输入: 更新pip 即可更新pip
7.输入: 查看已下载的包 查看已经下载的包
8.输入: 删除备份 查看已经下载的包
9.输入q退出或者点击x退出
)";
            println( Color("blue"), printStr);
            printf("\n");
        }

        // 查看已经备份的所有库 //
        else if (packageName == "查看已下载的包")
        {
            findPackageInstall();  // 查看安装的包
        }

        // 备份已经安装的库 //
        else if (packageName == "备份")
        {
            copyPackageInstall();
        }

        // 删除备份 //
        else if (packageName == "删除备份")
        {
            wkp::Open ipt;
            ipt.open("requirements.txt", "r");
            if (!ipt.isOpen())
            {
                std::cerr<<"无法打开文件\n";
            }
            auto packageLst = ipt.readLine();
            for (auto &line: packageLst)
            {
                delPackage(line);
            }
        }

        // 进入到安装库的功能 //
        else
        {
            bool findPackageGain = readFilePackage(packageName);  // 查找是否重复包
            if (!findPackageGain)  // 返回为假说明没有重复
            {
                int result = system(wkp::format(installCommand, packageName, installUrl).c_str());
                if (result)
                {
                    cerr<<"\n在安装包的过程中出现错误，可能的原因有:包的名称不存在 或者 其他问题\n";
                }
                else
                {
                    writePackage(packageName);
                    wkp::println(Color("green"), installCommand, packageName, installUrl);
                    wkp::println(Color("green"), "包名: {} 已经下载并备份\n", packageName);
                }
            }
            else
            {
                wkp::println(Color("green"), "包名 {} 已经下载\n", packageName);
            }
        }
    }
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
