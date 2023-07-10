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
    const QString fileName = QFileDialog::getOpenFileName(this, "Open PRX File", m_currentDir.absolutePath(), "PRX File (*.prx *.bin)");

    if (fileName.isEmpty())
        return;

    openFile(fileName);
}

void PowerObfuscator::openFile(const QString& fileName)
{
    m_qFile.setFileName(fileName);

    m_doesfileExist = m_qFile.exists();
    if (!m_doesfileExist)
        return;

    m_qFileInfo.setFile(m_qFile);
    m_currentDir = m_qFileInfo.dir();
    m_fileSize = m_qFileInfo.size();
    ui.filePathComboBox->addItem(fileName);

    std::string fileNameStdString = fileName.toStdString();
    getElfInfo(fileNameStdString);
    getSizeStatistics(fileNameStdString);
    getSectionInfo(fileNameStdString);
    getSymbolInfo(fileNameStdString);

    if (!m_qFile.open(QIODevice::ReadWrite))
    {
        qDebug() << "Could not open file for reading";
        return;
    }

    // Read file to data stream
    m_qDataStream.setDevice(&m_qFile);
}

void PowerObfuscator::on_obfuscateButton_clicked()
{
    if (!m_doesfileExist)
        return;

    QString passPhraseString = ui.obfuscatePassphraseTextEdit->toPlainText();
    if (passPhraseString.isEmpty())
    {
        QMessageBox::critical(this, windowTitle(), "Pass-phrase can not be empty.");
        return;
    }

    if (passPhraseString.size() > 64)
    {
        QMessageBox::critical(this, windowTitle(), "Pass-phrase is too long.");
        return;
    }
    
    std::vector<uint8_t> keyBytes;
    encryptPassphrase(passPhraseString.toStdString(), "PowerObfuscator", keyBytes);

    ui.outputTextEdit->append("----- Starting prx obfuscation -----");

    if (m_sections.size() <= 1)
    {
        ui.outputTextEdit->append("ERROR: Unable to encrypt prx, No segments found.");
        m_qFile.close();
        return;
    }

    ui.outputTextEdit->append("Reading file buffer");

    // Read file buffer
    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(m_fileSize);
    uint8_t* byteArray = buffer.get();
    int readBytes = m_qDataStream.readRawData(reinterpret_cast<char*>(byteArray), m_fileSize);

    qDebug() << "Read from buffer bytes";
    qDebug() << readBytes;

    qDebug() << "File size";
    qDebug() << m_fileSize;

    // Obfuscate [.text] segment
    obfuscateSegment(".text", byteArray, keyBytes);

    // Obfuscate [.sceStub.text] segment
    obfuscateSegment(".sceStub.text", byteArray, keyBytes);

    // Obfuscate [.rodata] segment
    obfuscateSegment(".rodata", byteArray, keyBytes);

    // Obfuscate [.data] segment
    obfuscateSegment(".data", byteArray, keyBytes);

    // Save obfuscated prx file
    saveObfuscatedFile("obf_", byteArray);
}

void PowerObfuscator::on_deobfuscateButton_clicked()
{
    if (!m_doesfileExist)
        return;

    QString keyString = ui.deobfuscateKeyTextEdit->toPlainText();
    if (keyString.isEmpty())
    {
        QMessageBox::critical(this, windowTitle(), "Key can not be empty.");
        return;
    }

    if (keyString.size() != 64)
    {
        QMessageBox::critical(this, windowTitle(), "Key must be 64 bytes long.");
        return;
    }

    std::vector<uint8_t> keyBytes = hexStringToBytes(keyString.toStdString());
    if (keyBytes.empty())
        return;

    ui.outputTextEdit->append("----- Starting prx deobfuscation -----");

    if (m_sections.size() <= 1)
    {
        ui.outputTextEdit->append("ERROR: Unable to encrypt prx, No segments found.");
        m_qFile.close();
        return;
    }

    ui.outputTextEdit->append("Reading file buffer");

    // Read file buffer
    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(m_fileSize);
    uint8_t* byteArray = buffer.get();
    int readBytes = m_qDataStream.readRawData(reinterpret_cast<char*>(byteArray), m_fileSize);

    // Deobfuscate [.text] segment
    obfuscateSegment(".text", byteArray, keyBytes);

    // Deobfuscate [.sceStub.text] segment
    obfuscateSegment(".sceStub.text", byteArray, keyBytes);

    // Deobfuscate [.rodata] segment
    obfuscateSegment(".rodata", byteArray, keyBytes);

    // Deobfuscate [.data] segment
    obfuscateSegment(".data", byteArray, keyBytes);

    // Save deobfuscated prx file
    saveObfuscatedFile("deobf_", byteArray);
}

void PowerObfuscator::obfuscateSegment(const QString& segmentName, uint8_t* byteArray, const std::vector<uint8_t>& encryptionKey)
{
    ui.outputTextEdit->append("Searching for [" + segmentName + "] segment address and size");

    const uint32_t elfHeaderSize = 0xF0;

    uint32_t segmentAddress{};
    uint32_t segmentSize{};

    for (const auto& section : m_sections)
    {
        if (section.name == segmentName.toStdString())
        {
            segmentAddress = section.address;
            segmentSize = section.size;
        }
    }

    //uint32_t segmentLocation = geBinaryOffsetFromSegment(segmentName);

    ui.outputTextEdit->append("Found [" + segmentName + "] segment address and size");

    qDebug() << "----- " << segmentName << " segment -----";
    qDebug() << "Segment Address: " << Qt::hex << Qt::showbase << segmentAddress;
    qDebug() << "Segment Size: " << Qt::hex << Qt::showbase << segmentSize;

    segmentAddress += elfHeaderSize;
    qDebug() << "Segment Address with ELF header: " << Qt::hex << Qt::showbase << segmentAddress;

    uint32_t segmentSizeInBinary = segmentAddress + segmentSize;
    qDebug() << "Segment Size with ELF header: " << Qt::hex << Qt::showbase << segmentSizeInBinary;

    ui.outputTextEdit->append("Encrypting [" + segmentName + "] segment");
    for (uint32_t i = segmentAddress; i < segmentSizeInBinary; i++)
    {
        /*if (byteArray[i] == 0)
            continue;*/

        //TODO(Roulette): temporarily here until prx obfuscation is finished
        byteArray[i] = (byteArray[i] ^ 0x69);

        //TODO(Roulette): uncomment this code when prx obfuscation is finished
        //byteArray[i] = (byteArray[i] ^ encryptionKey[(i - segmentAddress) % encryptionKey.size()]);
    }
}

uint32_t PowerObfuscator::geBinaryOffsetFromSegment(const QString& segmentName)
{
    uint32_t locationInBinary{};

    for (const auto& section : m_sections)
    {
        locationInBinary += section.size;

        if (section.name == segmentName.toStdString())
            break;
    }

    return locationInBinary;
}

void PowerObfuscator::saveObfuscatedFile(const QString& filePrefix, uint8_t* byteArray)
{
    ui.outputTextEdit->append("Saving obfuscated prx file");
    QString obfuscatedFileName = m_qFileInfo.path() + "/" + filePrefix + m_qFileInfo.fileName();
    ui.outputTextEdit->append(obfuscatedFileName);

    QFile obfuscatedFile(obfuscatedFileName);
    if (obfuscatedFile.open(QIODevice::WriteOnly))
    {
        QDataStream obfuscatedDataStream(&obfuscatedFile);

        // Write the modified data to the new file
        obfuscatedDataStream.writeRawData(reinterpret_cast<const char*>(byteArray), m_fileSize);

        obfuscatedFile.close();
    }
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
    // Lambda function to check if a character is a whitespace character
    auto isSpace = [](char ch) { return std::isspace(ch); };

    // Create a view of the input string and apply range adaptors
    auto trimmedRange = str |
        std::views::drop_while(isSpace) |       // Drop leading whitespace
        std::views::reverse |                   // Reverse the order of elements
        std::views::drop_while(isSpace) |       // Drop trailing whitespace
        std::views::reverse;                    // Reverse back to original order

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

    std::istringstream inStringStream(result);
    std::string line;

    while (std::getline(inStringStream, line))
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

void PowerObfuscator::encryptPassphrase(const std::string& passphrase, const std::string& key, std::vector<uint8_t>& encrypted)
{
    for (size_t i = 0; encrypted.size() < 64; ++i)
        encrypted.push_back(passphrase[i % passphrase.size()] ^ key[i % key.size()]);
}

std::vector<uint8_t> PowerObfuscator::hexStringToBytes(const std::string& hexString)
{
    std::vector<uint8_t> bytes;

    for (size_t i = 0; i < hexString.length(); i += 2)
    {
        std::string byteString = hexString.substr(i, 2);

        // Check if the byteString consists of valid hex characters
        bool isValidHex = std::all_of(byteString.begin(), byteString.end(), ::isxdigit);

        if (!isValidHex)
        {
            QMessageBox::critical(this, windowTitle(), "Key contains invalid hex value.");
            return {};
        }

        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}