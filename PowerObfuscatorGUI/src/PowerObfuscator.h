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

struct MainInfo
{
    uint32_t start;
    uint32_t end;
    uint32_t startWithElfHeader;
    uint32_t endWithElfHeader;
};

// from #include <sys/prx.h>
typedef struct sys_prx_libent32_t {
    unsigned char structsize;	/* 28 */
    unsigned char reserved1[1];
    unsigned short version;
    unsigned short attribute;
    unsigned short nfunc;
    unsigned short nvar;
    unsigned short ntls;
    unsigned char hashinfo;		/* funchashinfo + (varhashinfo << 4) */
    unsigned char hashinfo2;	/* tls */
    unsigned char reserved2[1];
    unsigned char nidaltsets;	/* number of alternate nid set */
    uint32_t libname;
    uint32_t nidtable;
    uint32_t addtable;
} sys_prx_libent32_t;

#define nid_module_prologue     0x0D10FD3F
#define nid_module_epilogue     0x330F7005
#define nid_module_exit         0x3AB9A95E
#define nid_module_start        0xBC9A0086
#define nid_module_stop         0xAB779874
#define nid_module_info         0xD7F43016

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
    MainInfo findMain(uint8_t* byteArray, uint32_t elfHeaderSize, uint32_t textSegmentSize);
    void saveObfuscatedFile(const QString& filePrefix, uint8_t* byteArray);
    void encryptPassphrase(const std::string& passphrase, const std::string& key, std::vector<uint8_t>& encrypted);
    void PrintPrxKey(const std::vector<uint8_t>& keyBytes);
    std::vector<uint8_t> hexStringToBytes(const std::string& hexString);
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    std::vector<uint8_t> parseHexDump(const std::string& hexDump);
    /***
    * @brief convert a 32-bit integer from big-endian to little-endian
    */
    uint32_t bigToLittleEndian(uint32_t value);
    /***
    * @brief convert a 32-bit integer from little-endian to big-endian
    */
    uint32_t littleToBigEndian(uint32_t value);

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
    SegmentInfo m_libEntSegmentInfo;
    QFile m_qFile;
    QFileInfo m_qFileInfo;
    QDataStream m_qDataStream;
    QByteArray m_qBytesArray;
};
