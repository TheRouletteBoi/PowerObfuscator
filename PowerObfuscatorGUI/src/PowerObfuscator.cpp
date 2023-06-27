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