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
}

void PowerObfuscator::on_showInfoButton_clicked()
{
    QString text = "No file loaded";

    if (m_doesfileExist)
    {
        std::string elfFlags = "Yes";
        std::string isObfuscated = "Yes";
        std::string format = std::format("ELF Flags: {0}\nObfuscated: {1}\nFile Size: {2}\n", elfFlags, isObfuscated, m_fileSize);
        text = QString::fromStdString(format);
    }

    QMessageBox::information(this, windowTitle(), text);
}

void PowerObfuscator::on_obfuscateButton_clicked()
{

}

void PowerObfuscator::on_deobfuscateButton_clicked()
{

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

void PowerObfuscator::getElfInfo(const std::string& fileName)
{
    std::string command = "ps3bin.exe --dump-elf-header " + fileName;
    std::string result = systemResult(command.c_str());

    if (result.contains("ERROR: "))
        return;

    std::istringstream ss(result);
    std::string line;
    ElfInfo elfInfo;

    while (std::getline(ss, line))
    {
        if (line.find("File Class:") != std::string::npos)
        {
            elfInfo.fileClass = line.substr(line.find(":") + 2);
        }
        else if (line.find("Data Encoding:") != std::string::npos)
        {
            elfInfo.dataEncoding = line.substr(line.find(":") + 2);
        }
        else if (line.find("Type:") != std::string::npos)
        {
            elfInfo.type = line.substr(line.find(":") + 2);
        }
        else if (line.find("Machine:") != std::string::npos)
        {
            elfInfo.machine = line.substr(line.find(":") + 2);
        }
        else if (line.find("Entry point:") != std::string::npos)
        {
            elfInfo.entryPoint = line.substr(line.find(":") + 2);
        }
        else if (line.find("Program Header Offset:") != std::string::npos)
        {
            elfInfo.programHeaderOffset = line.substr(line.find(":") + 2);
        }
        else if (line.find("Section Header Offset:") != std::string::npos)
        {
            elfInfo.sectionHeaderOffset = line.substr(line.find(":") + 2);
        }
        else if (line.find("Flags:") != std::string::npos)
        {
            elfInfo.flags = line.substr(line.find(":") + 2);
        }
        else if (line.find("Number of Program Headers:") != std::string::npos)
        {
            elfInfo.numProgramHeaders = line.substr(line.find(":") + 2);
        }
        else if (line.find("Number of Section Headers:") != std::string::npos)
        {
            elfInfo.numSectionHeaders = line.substr(line.find(":") + 2);
        }
        else if (line.find("Section Header String Index:") != std::string::npos)
        {
            elfInfo.sectionHeaderStringIndex = line.substr(line.find(":") + 2);
        }
    }
}

void PowerObfuscator::getSectionInfo(const std::string& fileName)
{
    std::string command = "ps3bin.exe --dump-section-headers " + fileName;
    std::string result = systemResult(command.c_str());

    if (result.contains("ERROR: "))
        return;

    std::istringstream inStringStream(result);
    std::string line;
    std::vector<SectionInfo> sections;

    // Skip the first line containing column headers
    std::getline(inStringStream, line);

    while (std::getline(inStringStream, line))
    {
        std::istringstream lineStream(line);
        SectionInfo section;
        lineStream >> section.index >> section.name >> section.size >> section.type >> section.address;
        sections.push_back(section);
    }
}

void PowerObfuscator::getSizeStatistics(const std::string& fileName)
{
    std::string command = "ps3bin.exe --dump-sizes " + fileName;
    std::string result = systemResult(command.c_str());

    if (result.contains("ERROR: "))
        return;

    std::istringstream inStringStream(result);
    std::string line;
    SizeStatistics sizeStats;

    // Skip the first line containing column headers
    std::getline(inStringStream, line);

    while (std::getline(inStringStream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> sizeStats.textSize >> sizeStats.dataSize >> sizeStats.roDataSize >> sizeStats.bssSize >> sizeStats.total >> sizeStats.fileName;
    }
}

void PowerObfuscator::getSymbolInfo(const std::string& fileName)
{
    std::string command = "ps3bin.exe --dump-symbols " + fileName;
    std::string result = systemResult(command.c_str());

    if (result.contains("ERROR: ") || result.contains("WARNING: "))
        return;

    std::istringstream inStringStream(result);
    std::string line;
    std::vector<SymbolInfo> symbolsInfo;

    // Skip the first line containing column headers
    std::getline(inStringStream, line);

    while (std::getline(inStringStream, line))
    {
        std::istringstream lineStream(line);
        SymbolInfo symbolInfo;
        lineStream >> symbolInfo.value >> symbolInfo.binding >> symbolInfo.type >> symbolInfo.section >> symbolInfo.name;
        symbolsInfo.push_back(symbolInfo);
    }
}