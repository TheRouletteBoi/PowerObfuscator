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
#include <algorithm>
#include <ranges>

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
    std::uint32_t size; // FIXME(Roulette): This should be std::string because some sizes have parentheses. Specifically .bss segment so if you encounter a bug when reading .bss it's because of this code
    std::string type;
    std::uint32_t address;
};

struct SizeStatistics
{
    uint64_t textSize;
    uint64_t dataSize;
    uint64_t roDataSize;
    uint64_t bssSize;
    uint64_t total;
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

struct SegmentInfo
{
    std::string type;
    std::string flags;
    std::string address;
    std::string offset;
    std::string size;
    std::string link;
    std::string info;
    std::string align;
    std::string entrySize;
    std::vector<uint8_t> byteData;
};

class PowerObfuscator : public QMainWindow
{
    Q_OBJECT

public:
    PowerObfuscator(QWidget *parent = nullptr);
    ~PowerObfuscator();

public: 
    /***
    * @brief calls a terminal command then returns the result
    */
    std::string systemResult(const char* cmd);
    /***
    * @brief removes white space from text
    */
    std::string trim(std::string_view str);
    void getElfInfo(const std::string& fileName);
    void getSectionHeaders(const std::string& fileName);
    void getSizeStatistics(const std::string& fileName);
    void getSymbolInfo(const std::string& fileName);
    void getSegmentInfo(const std::string& fileName, const std::string& segmentName, SegmentInfo& segmentInfo);
    void obfuscateSegment(const QString& segmentName, uint8_t* byteArray, const std::vector<uint8_t>& encryptionKey);
    uint32_t geBinaryOffsetFromSegment(const QString& segmentName);
    void saveObfuscatedFile(const QString& filePrefix, uint8_t* byteArray);
    void encryptPassphrase(const std::string& passphrase, const std::string& key, std::vector<uint8_t>& encrypted);
    std::vector<uint8_t> hexStringToBytes(const std::string& hexString);
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    std::vector<uint8_t> parseHexDump(const std::string& hexDump);

public slots:
    void openFile(const QString& fileName);

private slots:
    void on_loadPrxButton_clicked();
    void on_obfuscateButton_clicked();
    void on_deobfuscateButton_clicked();

private:
    Ui::PowerObfuscatorClass ui;
    QDir m_currentDir;
    bool m_doesfileExist{};
    bool m_isPrx{};
    quint64 m_fileSize{};
    ElfInfo m_elfInfo;
    std::vector<SectionInfo> m_sections;
    SizeStatistics m_sizeStats;
    std::vector<SymbolInfo> m_symbolsInfo;
    SegmentInfo m_segmentInfo;
    QFile m_qFile;
    QFileInfo m_qFileInfo;
    QDataStream m_qDataStream;
    QByteArray m_qBytesArray;
};
