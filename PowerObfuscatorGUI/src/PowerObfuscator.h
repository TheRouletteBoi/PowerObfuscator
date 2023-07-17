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

#include <Windows.h>

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
    uint64_t entryPoint;
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
    uint32_t size; // FIXME(Roulette): This should be std::string because some sizes have parentheses. Specifically .bss segment so if you encounter a bug when reading .bss it's because of this code
    std::string type;
    uint32_t address;
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
    uint64_t value;         // symbol offset in section
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

struct pobfHeader
{
    char magic[4];
    uint32_t signature1;
    uint32_t signature2;
    uint32_t signature3;
    uint32_t textSegmentStart;
    uint32_t textSegmentSize;
    uint32_t _padding1;
    uint32_t dataSegmentStart;
    uint32_t dataSegmentSize;
    uint32_t _padding2;
    uint32_t rodataStart;
    uint32_t rodataSize;
    uint32_t _padding3;
    uint32_t placeHolder1;
    uint32_t placeHolder2;
    uint32_t _padding4;
    char placeHolder5[60];
};

#define POBF_MAGIC 'P', 'O', 'B', 'F'
#define POBF_SIGNATURE littleToBigEndian(0xAABBCCDD), littleToBigEndian(0x12345678), littleToBigEndian(0xEEFFEEFF)
// these values will be replaced by fixHeader()
#define POBF_TEXT_SEGMENT_DUMMY_VALUES littleToBigEndian(0xDEADBEEF), littleToBigEndian(0x0BADCAFE), littleToBigEndian(0x00DDBA11) 
#define POBF_DATA_SEGMENT_DUMMY_VALUES littleToBigEndian(0x5CA1AB1E), littleToBigEndian(0x0DEC0DED), littleToBigEndian(0x00EFFEC7) 
#define POBF_PLACEHOLDER_DUMMY_VALUES1 littleToBigEndian(0x5E1EC7ED), littleToBigEndian(0x05EEDBED), littleToBigEndian(0x05CABB1E) 
#define POBF_PLACEHOLDER_DUMMY_VALUES2 littleToBigEndian(0x00FF5E75), littleToBigEndian(0x0001ABE1), littleToBigEndian(0x000F100D) 



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
    void getElfInfo(const std::string& fileName);
    void getSectionHeaders(const std::string& fileName);
    void getSizeStatistics(const std::string& fileName);
    void getSymbolInfo(const std::string& fileName);
    void getSegmentInfo(const std::string& fileName, const std::string& segmentName, SegmentInfo& segmentInfo);
    void stripSymbolsPrx(const std::string& fileName);
    void signPrx(const std::string& inFileName, const std::string& outFileName);
    void obfuscateSegment(const QString& segmentName, uint8_t* byteArray, const std::vector<uint8_t>& encryptionKey);
    void fixHeader(uint8_t* byteArray);

    /***
    * @brief skip specific PowerPC instructions to allow .data/.rodata segment encryption
    */
    bool SkipOpCode();

    uint32_t geBinaryOffsetFromSegment(const QString& segmentName);
    MainInfo findMain(uint8_t* byteArray, uint32_t elfHeaderSize, uint32_t textSegmentSize);
    SymbolInfo findHeaderBySymbolName(const QString& segmentName, const QString& symbolName, bool* outFound);
    void saveFileWithPrefix(const QString& filePrefix, uint8_t* byteArray, bool isEncrypted);

    /***
    * @brief calls a terminal command then returns the result
    */
    std::string systemResult(const char* cmd);

    /***
    * @brief calls a terminal command using WIN32 API
    *
    * HACK: for some reason when calling SecureTool.exe using system() or _popen() seems to fail so this is a temporary fix
    */
    void systemWin32(const char* cmd);

    /***
    * @brief removes white space from text
    */
    std::string trim(std::string_view str);

    void encryptPassphrase(const std::string& passphrase, const std::string& key, std::vector<uint8_t>& encrypted);
    void PrintPrxKey(const std::vector<uint8_t>& keyBytes);

    /***
    * @brief convert 5A5A5A5A5A5A5A5A5A5A5A5A into a uint8_t array
    */
    std::vector<uint8_t> hexStringToBytes(const std::string& hexString);

    std::vector<std::string> splitString(const std::string& str, char delimiter);

    /***
    * @brief convert hex dump into a uint8_t array
    * 5A 5A 5A 5A 5A 5A 5A 5A 5A 5A
    * 5A 5A 5A 5A 5A 5A 5A 5A 5A 5A
    * 5A 5A 5A 5A 5A 5A 5A 5A 5A 5A
    */
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
