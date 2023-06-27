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

#include <QtWidgets/QMainWindow>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_PowerObfuscator.h"

#include "PrxInfo.h"

class PowerObfuscator : public QMainWindow
{
    Q_OBJECT

public:
    PowerObfuscator(QWidget *parent = nullptr);
    ~PowerObfuscator();

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
