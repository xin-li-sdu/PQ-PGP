#include"fileread.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
char B[5465]; // 定义一个字符数组用于存储读取的内容
extern char MPK1[10000];
void MPKfilewrite(const std::string& str)
{
    //std::cout<<"文件输出测试"<<str;
    std::ofstream outputFile("C:/Users/Public/Documents/" + str + ".txt"); // 在指定路径下创建以字符串命名的 txt 文件

    if (outputFile.is_open())
    {
        outputFile << MPK1; // 向文件中写入公钥
        outputFile.close();
    }
    else {
        std::cout << "Unable to open output file." << std::endl;
    }
}
bool fileread(const std::string& str)
{
    std::ifstream inputFile("C:/Users/Public/Documents/" + str + ".txt"); // 打开指定路径下的 txt 文件

    if (inputFile.is_open())
    {
        inputFile.get(B, 10000); // 读取文件内容到数组 B[]
        inputFile.close();
        //std::cout << B<< std::endl;
        return 1;
    }
    else {
        //std::cout << "Unable to open input file." << std::endl;
        return 0;
    }
}

void searchFiles(const std::string& directory, std::string& result)
{
    // 遍历指定目录下的所有文件和子目录
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        // 如果是文件
        if (entry.is_regular_file())
        {
            // 获取文件的扩展名
            std::string extension = entry.path().extension().string();
            // 如果扩展名为.txt，则将文件名添加到结果字符串中
            if (extension == ".txt")
            {
                std::string filename = entry.path().filename().string();
                std::string filenameWithoutExtension = filename.substr(0, filename.find_last_of('.'));
                result += filenameWithoutExtension + "\n";
            }
        }
    }
}
