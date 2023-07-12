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
    getSectionHeaders(fileNameStdString);
    getSegmentInfo(fileNameStdString, ".lib.ent", m_libEntSegmentInfo); // .rodata.sceResident OR .lib.ent
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

#if 0
    // Obfuscate [.sceStub.text] segment
    obfuscateSegment(".sceStub.text", byteArray, keyBytes);

    // Obfuscate [.rodata] segment
    obfuscateSegment(".rodata", byteArray, keyBytes);

    // Obfuscate [.data] segment
    obfuscateSegment(".data", byteArray, keyBytes);
#endif

    // Save obfuscated prx file
    saveObfuscatedFile("obf_", byteArray);

    PrintPrxKey(keyBytes);
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

#if 0 
    // Deobfuscate [.sceStub.text] segment
    obfuscateSegment(".sceStub.text", byteArray, keyBytes);

    // Deobfuscate [.rodata] segment
    obfuscateSegment(".rodata", byteArray, keyBytes);

    // Deobfuscate [.data] segment
    obfuscateSegment(".data", byteArray, keyBytes);
#endif

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

    MainInfo mainInfo = findMain(byteArray, elfHeaderSize, segmentSizeInBinary);

    ui.outputTextEdit->append("Encrypting [" + segmentName + "] segment");
    for (uint32_t i = segmentAddress; i < segmentSizeInBinary; i++)
    {
        // Skip the main() function
        if (i >= mainInfo.startWithElfHeader && i <= mainInfo.endWithElfHeader)
            continue;

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

MainInfo PowerObfuscator::findMain(uint8_t* byteArray, uint32_t elfHeaderSize, uint32_t textSegmentSize)
{
    MainInfo mainInfo{};
    qDebug() << "-----  Searching for module_start in [.lib.ent] Segment -----";

    // HACK(Roulette): fucking hack to get module entry point via nid hash
    sys_prx_libent32_t* libEnt = reinterpret_cast<sys_prx_libent32_t*>(m_libEntSegmentInfo.byteData.data());

    uint32_t firstSubroutine = littleToBigEndian(libEnt->libname);
    uint32_t nidTable = littleToBigEndian(libEnt->nidtable);
    uint32_t funcTable = littleToBigEndian(libEnt->addtable);
    int hashIndex = 0;

    qDebug() << "First Subroutine Address: " << Qt::hex << Qt::showbase << firstSubroutine;
    qDebug() << "Nid Table Address: " << Qt::hex << Qt::showbase << nidTable;
    qDebug() << "Function Table Address: " << Qt::hex << Qt::showbase << funcTable;

    uint32_t moduleStartNidHash = 0;
    memcpy(&moduleStartNidHash, byteArray + elfHeaderSize + (nidTable + (hashIndex * 4)), sizeof(uint32_t));
    moduleStartNidHash = littleToBigEndian(moduleStartNidHash);

    uint32_t moduleStartOpd = 0;
    memcpy(&moduleStartOpd, byteArray + elfHeaderSize + (funcTable + (hashIndex * 4)), sizeof(uint32_t));
    moduleStartOpd = littleToBigEndian(moduleStartOpd);

    qDebug() << "module_start nid Hash: " << Qt::hex << Qt::showbase << moduleStartNidHash;
    qDebug() << "module_start opd Address: " << Qt::hex << Qt::showbase << moduleStartOpd;

    if (moduleStartNidHash == nid_module_start)
    {
        uint32_t moduleStartAddress = 0;
        memcpy(&moduleStartAddress, byteArray + elfHeaderSize + moduleStartOpd, sizeof(uint32_t));

        moduleStartAddress = littleToBigEndian(moduleStartAddress);
        mainInfo.start = moduleStartAddress;
        qDebug() << "module_start Address: " << Qt::hex << Qt::showbase << moduleStartAddress;

        moduleStartAddress += elfHeaderSize;
        mainInfo.startWithElfHeader = moduleStartAddress;
        qDebug() << "module_start Address with ELF header: " << Qt::hex << Qt::showbase << moduleStartAddress;

        qDebug() << "Searching for end of module_start";
        for (uint32_t i = moduleStartAddress; i < textSegmentSize; i++)
        {
            // find end of function by searching for 'blr' instruction 4E 80 00 20
            if (byteArray[i] == 0x4E && byteArray[i + 1] == 0x80 && byteArray[i + 2] == 0x00 && byteArray[i + 3] == 0x20)
            {
                mainInfo.endWithElfHeader = i + 4;
                mainInfo.end = (i + 4) - elfHeaderSize;

                // Bytes found
                qDebug() << "End of function found at offset: " << Qt::hex << Qt::showbase << i;
                break;
            }
        }
    }

    return mainInfo;
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

void PowerObfuscator::PrintPrxKey(const std::vector<uint8_t>& keyBytes)
{
    ui.outputTextEdit->append("\n----- Use this key in your sprx code -----");
    std::stringstream ss;
    ss << "uint8_t key[" << keyBytes.size() << "] = {";

    auto byteView = keyBytes | std::views::transform([](uint8_t byte) {
        return std::format(" 0x{:02X}", byte);
    });

    bool firstByte = true;
    for (const auto& byte : byteView) 
    {
        if (!firstByte) 
        {
            ss << ", ";
        }
        else 
        {
            firstByte = false;
        }
        ss << byte;
    }

    ss << " };\n";

    ui.outputTextEdit->append(QString::fromStdString(ss.str()));
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
    /*
        Target: PS3 PPU
        ELF header :

        File Class : ELFCLASS64
        Data Encoding : ELFDATA2MSB
        Type : ET_SCE_PPURELEXEC
        Machine : PowerPC64
        Entry point : 0x0000000000000000
        Program Header Offset : 0x0000000000000040
        Section Header Offset : 0x0000000000006468
        Flags : 0x01000000
        Number of Program Headers : 3
        Number of Section Headers : 40
        Section Header String Index : 26
    */
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

void PowerObfuscator::getSectionHeaders(const std::string& fileName)
{
    /*
        Index Name                      Size           Type                  Address
        0     SHN_UNDEF                 (0x00000000)   SHT_NULL              0x00000000
        1     .text                     0x00003014     SHT_PROGBITS          0x00000000
        2     .rela.text                0x000015D8     SHT_SCE_PPURELA       0x00000000
        3     .sceStub.text             0x00000080     SHT_PROGBITS          0x00003014
        4     .rela.sceStub.text        0x000000C0     SHT_SCE_PPURELA       0x00000000
        5     .lib.ent.top              0x00000004     SHT_PROGBITS          0x000030A0
        6     .lib.ent                  0x0000001C     SHT_PROGBITS          0x000030A4
        7     .rela.lib.ent             0x00000030     SHT_SCE_PPURELA       0x00000000
        8     .lib.ent.btm              0x00000004     SHT_PROGBITS          0x000030C0
        9     .lib.stub.top             0x00000004     SHT_PROGBITS          0x000030C4
        10    .lib.stub                 0x0000002C     SHT_PROGBITS          0x000030C8
        11    .rela.lib.stub            0x00000048     SHT_SCE_PPURELA       0x00000000
        12    .lib.stub.btm             0x00000004     SHT_PROGBITS          0x000030F4
        13    .rodata.sceModuleInfo     0x00000034     SHT_PROGBITS          0x000030F8
        14    .rela.rodata.sceModul...  0x00000078     SHT_SCE_PPURELA       0x00000000
        15    .rodata.sceResident       0x0000002C     SHT_PROGBITS          0x0000312C
        16    .rela.rodata.sceResident  0x00000048     SHT_SCE_PPURELA       0x00000000
        17    .rodata.sceFNID           0x00000010     SHT_PROGBITS          0x00003158
        18    .rodata                   0x000003F0     SHT_PROGBITS          0x00003168
        19    .data.sceFStub            0x00000010     SHT_PROGBITS          0x00003560
        20    .rela.data.sceFStub       0x00000060     SHT_SCE_PPURELA       0x00000000
        21    .data                     0x00000040     SHT_PROGBITS          0x00003570
        22    .opd                      0x000002C8     SHT_PROGBITS          0x000035B0
        23    .rela.opd                 0x000010B0     SHT_SCE_PPURELA       0x00000000
        24    .toc                      0x00000000     SHT_PROGBITS          0x00003880
        25    .bss                      (0x00000008)   SHT_NOBITS            0x00003880
        26    .shstrtab                 0x00000212     SHT_STRTAB            0x00000000
        27    .symtab                   0x00001C38     SHT_SYMTAB            0x00000000
        28    .strtab                   0x00004274     SHT_STRTAB            0x00000000
        29    .debug_aranges            0x000001F0     SHT_PROGBITS          0x00000000
        30    .debug_pubnames           0x00000A9A     SHT_PROGBITS          0x00000000
        31    .debug_info               0x00007247     SHT_PROGBITS          0x00000000
        32    .debug_abbrev             0x0000029B     SHT_PROGBITS          0x00000000
        33    .debug_line               0x00000776     SHT_PROGBITS          0x00000000
        34    .debug_str                0x00002AE8     SHT_PROGBITS          0x00000000
        35    .rela.debug_aranges       0x000005A0     SHT_RELA              0x00000000
        36    .rela.debug_pubnames      0x00000018     SHT_RELA              0x00000000
        37    .rela.debug_info          0x00000EA0     SHT_RELA              0x00000000
        38    .rela.debug_line          0x00000870     SHT_RELA              0x00000000
        39    .sceversion               0x00000077     SHT_PROGBITS          0x00000000
    */
    std::string command = "ps3bin.exe --dump-section-headers " + fileName;
    std::string result = systemResult(command.c_str());

    ui.outputTextEdit->append("----- Section Headers -----");

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
    /*
        Text Size   Data Size   RO-Data Size  BSS Size    Total       Filename
        12436       792         1208          8           14444       C:\Users\agent\Documents\GitHub\PowerObfuscator\bin\Debug-PS3\PowerObfuscatorSPRX.prx    
    */
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
    /*
        Value              Binding      Type         Section          Name
        0x0000000000000000 Local        Section      .sceversion      .sceversion
        0x0000000000000000 Local        Object       .rodata          [local to Main_cpp]::pobf::Vx::vxCplEncryptCharKey
        0x0000000000000000 Local        Object       .lib.ent.top     __begin_of_section_lib_ent
        0x0000000000000000 Local        Object       .lib.ent.btm     __end_of_section_lib_ent
        0x0000000000000000 Local        Object       .lib.stub.top    __begin_of_section_lib_stub
        0x0000000000000000 Local        Object       .lib.stub.btm    __end_of_section_lib_stub
        0x0000000000000000 Local        Source File  SHN_ABS          /home/kanee/svnwork/ps3-svn/svn/sys/trunk/dinkumware/dinkum-pu/source/memcpy.S
        0x0000000000000000 Local        Source File  SHN_ABS          /home/kanee/svnwork/ps3-svn/svn/sys/trunk/dinkumware/dinkum-pu/source/sys/lv2/memset.S
        0x0000000000000000 Local        STT_NOTYPE   .data.sceFStub   _sys_printf.stub_entry
        0x0000000000000000 Local        Object       .rodata.sceR...  _sce_package_version_sysPrxForUser
        0x0000000000000000 Global       Function     .text            ThisFuncShouldBeEncrypted001()
        0x0000000000000000 Global       STT_NOTYPE   .opd             ThisFuncShouldBeEncrypted001()
        0x0000000000000000 Global       Object       .rodata.sceM...  __psp_moduleinfo
        0x0000000000000000 Global       Object       .data            gTestEncryptedThreadId
        0x0000000000000000 Global       Function     .sceStub.text    ._sys_printf
        0x0000000000000000 Global       STT_NOTYPE   .text            __start__Ztext
        0x0000000000000000 Global       Object       .bss             pobf::DumpObfuscation::g_EbootStart
        0x0000000000000000 Global       Object       .lib.stub        _sysPrxForUser_0001_stub_head
        0x0000000000000004 Local        Object       .rodata          ..LNst._Z28ThisFuncShouldBeEncrypted001v.0
        0x0000000000000004 Local        STT_NOTYPE   .data.sceFStub   sys_ppu_thread_exit.stub_entry
        0x0000000000000004 Global       Object       .bss             pobf::DumpObfuscation::g_EbootEnd
        0x0000000000000004 Global       Object       .rodata.sceR...  _sysPrxForUser_stub_str
        0x0000000000000008 Local        Object       .data            LS.10.AGGR.CONST._Z28pobf_vxStringEncrypt_Examplev.1
        0x0000000000000008 Local        STT_NOTYPE   .data.sceFStub   _sys_malloc.stub_entry
        0x0000000000000008 Global       STT_NOTYPE   .opd             ThisFuncShouldBeEncrypted002(int, int)
        0x000000000000000C Local        STT_NOTYPE   .data.sceFStub   _sys_free.stub_entry
        0x0000000000000010 Global       STT_NOTYPE   .opd             ThisFuncShouldBeEncrypted003()
        0x0000000000000014 Local        STT_NOTYPE   .rodata.sceR...  _NONAMEnid_table
        0x0000000000000015 Local        Object       .data            LS.13.AGGR.CONST._Z33pobf_AllStringEncryptions_Examplev.1
        0x0000000000000018 Global       STT_NOTYPE   .opd             MainThread(unsigned long long)
        0x0000000000000020 Local        STT_NOTYPE   .rodata.sceR...  _NONAMEentry_table
        0x0000000000000020 Global       STT_NOTYPE   .opd             pobf_CompileTime_RandomInt_Example()
        0x0000000000000020 Global       Function     .sceStub.text    .sys_ppu_thread_exit
        0x0000000000000024 Local        Object       .rodata          ..LNst._Z28ThisFuncShouldBeEncrypted001v.1
        0x0000000000000028 Global       STT_NOTYPE   .opd             pobf_vxRandom1_Example()
        0x0000000000000030 Local        Object       .rodata          ..LNst._Z28ThisFuncShouldBeEncrypted001v.2
        0x0000000000000030 Global       STT_NOTYPE   .opd             pobf_vxRandom2_Example()
        0x0000000000000038 Global       STT_NOTYPE   .opd             pobf_vxHashing_Example()
        0x000000000000003C Local        Object       .rodata          ..LNst._Z10MainThready.0
        0x0000000000000040 Global       STT_NOTYPE   .opd             pobf_vxStringEncrypt_Example()
        0x0000000000000040 Global       Function     .sceStub.text    ._sys_malloc
        0x0000000000000048 Local        Object       .rodata          ..LNst._Z10MainThready.1
        0x0000000000000048 Global       STT_NOTYPE   .opd             pobf_AllStringEncryptions_Example()
        0x0000000000000050 Global       STT_NOTYPE   .opd             pobf_DumpObfuscation_Example()
        0x0000000000000058 Global       STT_NOTYPE   .opd             size_of_dot_text()
        0x000000000000005C Local        Object       .rodata          ..LNst._Z34pobf_CompileTime_RandomInt_Examplev.0
        0x0000000000000060 Global       STT_NOTYPE   .opd             PowerObfuscatorSPRXMain
        0x0000000000000060 Global       Function     .sceStub.text    ._sys_free
        0x0000000000000068 Global       STT_NOTYPE   .opd             PowerObfuscatorSPRXStop
        0x0000000000000070 Weak         STT_NOTYPE   .opd             std::_String_val<char, std::allocator<char>>::_String_val(std::allocator<char>)
        0x0000000000000078 Weak         STT_NOTYPE   .opd             std::_String_val<char, std::allocator<char>>::_String_val(std::allocator<char>)
        0x0000000000000080 Local        Object       .rodata          ..LNst._Z34pobf_CompileTime_RandomInt_Examplev.1
        0x0000000000000080 Weak         STT_NOTYPE   .opd             std::char_traits<char>::copy(char*, char const*, unsigned int)
        0x0000000000000088 Weak         STT_NOTYPE   .opd             std::allocator<char>::deallocate(char*, unsigned int)
        0x0000000000000090 Global       Function     .text            ThisFuncShouldBeEncrypted002(int, int)
        0x0000000000000090 Weak         STT_NOTYPE   .opd             std::basic_string<char, std::char_traits<char>, std::allocator<char>>::_Myptr()
        0x0000000000000098 Weak         STT_NOTYPE   .opd             std::char_traits<char>::assign(char&, char const&)
        0x000000000000009C Local        Object       .rodata          ..LNst._Z34pobf_CompileTime_RandomInt_Examplev.2
        0x00000000000000A0 Weak         STT_NOTYPE   .opd             std::basic_string<char, std::char_traits<char>, std::allocator<char>>::_Eos(unsigned int)
        0x00000000000000A8 Weak         STT_NOTYPE   .opd             std::basic_string<char, std::char_traits<char>, std::allocator<char>>::_Tidy(bool, unsigned int)
        0x00000000000000B0 Weak         STT_NOTYPE   .opd             std::_String_base::_Xlen() const
        0x00000000000000B8 Local        Object       .rodata          ..LNst._Z34pobf_CompileTime_RandomInt_Examplev.3
        0x00000000000000B8 Weak         STT_NOTYPE   .opd             std::allocator<char>::max_size() const
        0x00000000000000C0 Weak         STT_NOTYPE   .opd             std::basic_string<char, std::char_traits<char>, std::allocator<char>>::max_size() const
        0x00000000000000C8 Weak         STT_NOTYPE   .opd             char* std::_Allocate<char>(unsigned int, char*)
        0x00000000000000D0 Weak         STT_NOTYPE   .opd             std::allocator<char>::allocate(unsigned int)
        0x00000000000000D8 Local        Object       .rodata          ..LNst._Z34pobf_CompileTime_RandomInt_Examplev.4
        0x00000000000000D8 Weak         STT_NOTYPE   .opd             std::basic_string<char, std::char_traits<char>, std::allocator<char>>::_Copy(unsigned int, unsigned int)
        0x00000000000000DC Global       Function     .text            ThisFuncShouldBeEncrypted003()
    */
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

void PowerObfuscator::getSegmentInfo(const std::string& fileName, const std::string& segmentName, SegmentInfo& segmentInfo)
{
    /*
        15 - .rodata.sceResident:
          Type:        SHT_PROGBITS (0x00000001)
          Flags:       SHF_ALLOC
          Address:     0x000000000000312C | Offset:      0x000000000000321C
          Size:        0x000000000000002C | Link:        0x00000000
          Info:        0x00000000         | Align:       0x0000000000000004
          Entry Size:  0x0000000000000000

        0x0000312C  00 00 00 00 73 79 73 50 72 78 46 6F 72 55 73 65  ....sysPrxForUse
        0x0000313C  72 00 00 00 BC 9A 00 86 AB 77 98 74 D7 F4 30 16  r........w.t..0.
        0x0000314C  00 00 36 10 00 00 36 18 00 00 30 F8              ..6...6...0.


        6 - .lib.ent:
          Type:        SHT_PROGBITS (0x00000001)
          Flags:       SHF_ALLOC
          Address:     0x00000000000030A4 | Offset:      0x0000000000003194
          Size:        0x000000000000001C | Link:        0x00000000
          Info:        0x00000000         | Align:       0x0000000000000004
          Entry Size:  0x0000000000000000

        0x000030A4  1C 00 00 00 80 00 00 02 00 01 00 00 00 00 00 00  ................
        0x000030B4  00 00 00 00 00 00 31 40 00 00 31 4C              ......1@..1L
    */
    std::string command = "ps3bin.exe --dump-sections=" + segmentName + " --hex " + fileName;
    std::string result = systemResult(command.c_str());

    ui.outputTextEdit->append("----- Segment Info -----");

    if (result.contains("ERROR: ") || result.contains("WARNING: "))
    {
        ui.outputTextEdit->append(QString::fromStdString(result));
        return;
    }

    // Extract section information
    std::vector<std::string> lines = splitString(result, '\n');
    for (const std::string& line : lines) 
    {
        if (line.contains("Type:")) 
        {
            std::vector<std::string> tokens = splitString(line, ':');
            segmentInfo.type = tokens[1];
        }
        else if (line.contains("Flags:")) 
        {
            std::vector<std::string> tokens = splitString(line, ':');
            segmentInfo.flags = tokens[1];
        }
        else if (line.contains("Address:")) 
        {
            std::vector<std::string> tokens = splitString(line, '|');
            segmentInfo.address = tokens[0];
            segmentInfo.offset = tokens[1];
        }
        else if (line.contains("Size:")) 
        {
            std::vector<std::string> tokens = splitString(line, ':');
            segmentInfo.size = tokens[1];
        }
        else if (line.contains("Link:")) 
        {
            std::vector<std::string> tokens = splitString(line, ':');
            segmentInfo.link = tokens[1];
        }
        else if (line.contains("Info:")) 
        {
            std::vector<std::string> tokens = splitString(line, ':');
            segmentInfo.info = tokens[1];
        }
        else if (line.contains("Align:")) 
        {
            std::vector<std::string> tokens = splitString(line, ':');
            segmentInfo.align = tokens[1];
        }
        else if (line.contains("Entry Size:")) 
        {
            std::vector<std::string> tokens = splitString(line, ':');
            segmentInfo.entrySize = tokens[1];
        }
    }

    segmentInfo.byteData = parseHexDump(result);


    ui.outputTextEdit->append(QString::fromStdString(result));
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

std::vector<std::string> PowerObfuscator::splitString(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream inStringStream(str);
    std::string token;
    while (std::getline(inStringStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<uint8_t> PowerObfuscator::parseHexDump(const std::string& hexDump)
{
    std::vector<uint8_t> bytes;
    std::istringstream inStringStream(hexDump);
    std::string line;

    // Parse the hex values and add them to the byte vector
    while (std::getline(inStringStream, line))
    {
        if (line.substr(0, 2) == "0x")
        {
            std::istringstream hexStream(line.substr(10));
            std::string byteString;
            while (hexStream >> byteString)
            {
                try
                {
                    uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
                    bytes.push_back(byte);
                }
                catch (const std::invalid_argument&)
                {
                    // Skip non-hexadecimal characters
                }
            }
        }
    }

    return bytes;
}

uint32_t PowerObfuscator::bigToLittleEndian(uint32_t value) 
{
    return std::endian::native == std::endian::big ? value : std::byteswap(value);
}

uint32_t PowerObfuscator::littleToBigEndian(uint32_t value) 
{
    return std::endian::native == std::endian::big ? value : std::byteswap(value);
}