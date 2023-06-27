#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <bit>
#include <concepts>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <cstdio>
#include <sstream>

#include <QtWidgets/QMainWindow>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_PowerObfuscator.h"

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

class PowerObfuscator : public QMainWindow
{
    Q_OBJECT

public:
    PowerObfuscator(QWidget *parent = nullptr);
    ~PowerObfuscator();

public: 
    std::string systemResult(const char* cmd);
    void getElfInfo(const std::string& fileName);
    void getSectionInfo(const std::string& fileName);
    void getSizeStatistics(const std::string& fileName);
    void getSymbolInfo(const std::string& fileName);

public slots:
    void openFile(const QString& fileName);

private slots:
    void on_loadPrxButton_clicked();
    void on_showInfoButton_clicked();
    void on_obfuscateButton_clicked();
    void on_deobfuscateButton_clicked();

private:
    Ui::PowerObfuscatorClass ui;
    QDir m_currentDir;
    bool m_doesfileExist{};
    quint64 m_fileSize{};
};
