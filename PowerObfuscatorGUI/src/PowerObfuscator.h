#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PowerObfuscator.h"

class PowerObfuscator : public QMainWindow
{
    Q_OBJECT

public:
    PowerObfuscator(QWidget *parent = nullptr);
    ~PowerObfuscator();

private:
    Ui::PowerObfuscatorClass ui;
};
