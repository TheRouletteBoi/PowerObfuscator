#include "PowerObfuscator.h"

PowerObfuscator::PowerObfuscator(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

PowerObfuscator::~PowerObfuscator()
{}

void PowerObfuscator::on_loadPrxButton_clicked()
{
    const QString fileName = QFileDialog::getOpenFileName(this, "Open PRX File", m_currentDir.absolutePath(), "PRX File (*.prx)");

    if (fileName.isEmpty())
        return;

    openFile(fileName);
}

void PowerObfuscator::openFile(const QString& fileName)
{
    QFile file(fileName);

    m_doesfileExist = file.exists();
    if (!m_doesfileExist)
        return;

    QFileInfo fileInfo(file);
    m_currentDir = fileInfo.dir();
    m_fileSize = fileInfo.size();
    ui.filePathComboBox->addItem(fileName);

    std::string fileNameStdString = fileName.toStdString();
    getElfInfo(fileNameStdString);
    getSizeStatistics(fileNameStdString);
    getSectionInfo(fileNameStdString);
    getSymbolInfo(fileNameStdString);
}

void PowerObfuscator::on_showInfoButton_clicked()
{
    QString text = "No file loaded";

    if (m_doesfileExist)
    {
        std::string format = std::format("ELF Type: {0}\n"
            "Entry Point : {1}\nFlags : {2}\n"
            "Segment Information\nText Size : {3}\n"
            "Data Size : {4}\n"
            "RO - Data Size : {5}\n"
            "BSS Size : {6}\n", 
            m_elfInfo.type, m_elfInfo.entryPoint, m_elfInfo.flags, 
            m_sizeStats.textSize, m_sizeStats.dataSize, m_sizeStats.roDataSize, m_sizeStats.bssSize);
        text = QString::fromStdString(format);
    }

    QMessageBox::information(this, windowTitle(), text);
}

void PowerObfuscator::on_obfuscateButton_clicked()
{
    if (!m_doesfileExist)
        return;

    ui.outputTextEdit->append("----- Segment Obfucation [.text]-----");

    ui.outputTextEdit->append("Searching for segment address and size");
    uint32_t address{};
    uint32_t size{};

    for (const auto& section : m_sections)
    {
        if (section.name == ".text")
        {
            address = section.address;
            size = section.size;
        }
    }

    ui.outputTextEdit->append("Encrypting Text segment");
    for (uint32_t i = address; i < size; i++)
    {

    }
}

void PowerObfuscator::on_deobfuscateButton_clicked()
{
    if (!m_doesfileExist)
        return;
}

std::string PowerObfuscator::systemResult(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);

    if (!pipe)
        throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();

    return result;
}

std::string PowerObfuscator::trim(std::string_view str) 
{
    auto isSpace = [](char ch) { return std::isspace(ch); };

    // wtf?? magic
    auto trimmedRange = str |
        std::views::drop_while(isSpace) |
        std::views::reverse |
        std::views::drop_while(isSpace) |
        std::views::reverse;

    return std::ranges::to<std::string>(trimmedRange);
}

void PowerObfuscator::getElfInfo(const std::string& fileName)
{
    std::string command = "ps3bin.exe --dump-elf-header " + fileName;
    std::string result = systemResult(command.c_str());

    ui.outputTextEdit->append("----- ELF Information -----");

    if (result.contains("ERROR: "))
    {
        ui.outputTextEdit->append(QString::fromStdString(result));
        return;
    }

    std::istringstream ss(result);
    std::string line;

    while (std::getline(ss, line))
    {
        if (line.contains("File Class:"))
        {
            m_elfInfo.fileClass = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Data Encoding:"))
        {
            m_elfInfo.dataEncoding = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Type:"))
        {
            m_elfInfo.type = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Machine:"))
        {
            m_elfInfo.machine = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Entry point:"))
        {
            m_elfInfo.entryPoint = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Program Header Offset:"))
        {
            m_elfInfo.programHeaderOffset = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Section Header Offset:"))
        {
            m_elfInfo.sectionHeaderOffset = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Flags:"))
        {
            m_elfInfo.flags = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Number of Program Headers:"))
        {
            m_elfInfo.numProgramHeaders = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Number of Section Headers:"))
        {
            m_elfInfo.numSectionHeaders = trim(line.substr(line.find(":") + 2));
        }
        else if (line.contains("Section Header String Index:"))
        {
            m_elfInfo.sectionHeaderStringIndex = trim(line.substr(line.find(":") + 2));
        }
    }

    ui.outputTextEdit->append(QString::fromStdString(result));
}

void PowerObfuscator::getSectionInfo(const std::string& fileName)
{
    std::string command = "ps3bin.exe --dump-section-headers " + fileName;
    std::string result = systemResult(command.c_str());

    ui.outputTextEdit->append("----- Section Information -----");

    if (result.contains("ERROR: "))
    {
        ui.outputTextEdit->append(QString::fromStdString(result));
        return;
    }

    std::istringstream inStringStream(result);
    std::string line;

    // Skip the first line containing column headers
    std::getline(inStringStream, line);

    while (std::getline(inStringStream, line))
    {
        std::istringstream lineStream(line);
        SectionInfo section;

        lineStream >> section.index;
        lineStream >> section.name;
        lineStream >> std::hex >> section.size; 
        lineStream >> section.type;
        lineStream >> std::hex >> section.address;

        // Remove leading/trailing white space from values
        //section.name = trim(section.name);
        //section.size = trim(section.size);
        //section.type = trim(section.type);
        //section.address = trim(section.address);

        m_sections.push_back(section);
    }

    ui.outputTextEdit->append(QString::fromStdString(result));
}

void PowerObfuscator::getSizeStatistics(const std::string& fileName)
{
    std::string command = "ps3bin.exe --dump-sizes " + fileName;
    std::string result = systemResult(command.c_str());

    ui.outputTextEdit->append("----- Segment Sizes -----");

    if (result.contains("ERROR: "))
    {
        ui.outputTextEdit->append(QString::fromStdString(result));
        return;
    }

    std::istringstream inStringStream(result);
    std::string line;

    // Skip the first line containing column headers
    std::getline(inStringStream, line);

    while (std::getline(inStringStream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> m_sizeStats.textSize >> m_sizeStats.dataSize >> m_sizeStats.roDataSize >> m_sizeStats.bssSize >> m_sizeStats.total >> m_sizeStats.fileName;
    }

    ui.outputTextEdit->append(QString::fromStdString(result));
}

void PowerObfuscator::getSymbolInfo(const std::string& fileName)
{
    std::string command = "ps3bin.exe --dump-symbols " + fileName;
    std::string result = systemResult(command.c_str());

    ui.outputTextEdit->append("----- Symbol Information -----");

    if (result.contains("ERROR: ") || result.contains("WARNING: "))
    {
        ui.outputTextEdit->append(QString::fromStdString(result));
        return;
    }

    std::istringstream inStringStream(result);
    std::string line;

    // Skip the first line containing column headers
    std::getline(inStringStream, line);

    while (std::getline(inStringStream, line))
    {
        std::istringstream lineStream(line);
        SymbolInfo symbolInfo;
        lineStream >> symbolInfo.value >> symbolInfo.binding >> symbolInfo.type >> symbolInfo.section >> symbolInfo.name;
        m_symbolsInfo.push_back(symbolInfo);
    }

    // Find entry point function symbol
    auto found = std::ranges::find_if(m_symbolsInfo, [this](const SymbolInfo& symInfo) {
        return (symInfo.section == ".text") && (symInfo.type == "Function") && (symInfo.value == m_elfInfo.entryPoint);
        });

    // Only display entry point symbol
    std::string str = std::format("Entry Point Symbol: {0}\n", (*found).name);

    ui.outputTextEdit->append(QString::fromStdString(str));
}