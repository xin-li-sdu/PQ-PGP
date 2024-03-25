#ifndef FILEREAD_H
#define FILEREAD_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

void MPKfilewrite(const std::string& str);
bool fileread(const std::string& str);
void searchFiles(const std::string& directory, std::string& result);

#endif // FILEREAD_H
