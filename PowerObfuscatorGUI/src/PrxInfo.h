#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <concepts>
#include <memory>
#include <string>
#include <array>
#include <cstdio>
#include <sstream>

struct ElfInfo
{
    std::string fileClass;
    std::string dataEncoding;
    std::string type;
    std::string machine;
    std::string entryPoint;
    std::string programHeaderOffset;
    std::string sectionHeaderOffset;
    std::string flags;
    std::string numProgramHeaders;
    std::string numSectionHeaders;
    std::string sectionHeaderStringIndex;
};

struct SectionInfo
{
    int index;
    std::string name;
    std::string size;
    std::string type;
    std::string address;
};

struct SizeStatistics
{
    int64_t textSize;
    int64_t dataSize;
    int64_t roDataSize;
    int64_t bssSize;
    int64_t total;
    std::string fileName;
};

struct SymbolInfo 
{
    std::string value;
    std::string binding;
    std::string type;
    std::string section;
    std::string name;
};

std::string system_result(const char* cmd);

void getElfInfo(const std::string& fileName);
void getSectionInfo(const std::string& fileName);
void getSizeStatistics(const std::string& fileName);
void getSymbolInfo(const std::string& fileName);

