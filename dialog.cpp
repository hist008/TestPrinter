#include "dialog.h"
#include "ui_dialog.h"
#include <QTextCodec>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

#pragma comment(lib, "setupapi.lib")

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);


    // 👇 一键美化样式表，复制这段即可
    this->setStyleSheet(R"(
        QDialog {
            background-color: #f5f7fa;
        }
        QComboBox {
            border: 1px solid #d0d7de;
            border-radius: 6px;
            padding: 8px 12px;
            background-color: white;
            font-size: 14px;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QPushButton {
            border: none;
            border-radius: 6px;
            padding: 10px;
            font-size: 14px;
            font-weight: 500;
        }

        QMessageBox {
            background-color:#ffffff;
            border-radius:12px;
            border:1px solid #e0e0e0;
        }
        QMessageBox QLabel {
            font-size:14px;
            color:#333;
            padding:10px 5px;
        }
        QMessageBox QPushButton {
            min-width:80px;
            padding:8px 16px;
            background-color:#409eff;
            color:white;
            border-radius:6px;
        }
        QMessageBox QPushButton:hover {
            background-color:#338ecc;
        }
        #btnScan {
            background-color: #36a2eb;
            color: white;
        }
        #btnScan:hover {
            background-color: #2b8fd9;
        }
        #btnPrint {
            background-color: #28a745;
            color: white;
        }
        #btnPrint:hover {
            background-color: #218838;
        }
    )");

    connect(ui->btnScan, &QPushButton::clicked, this, &Dialog::scanUsbPrinters);
    connect(ui->btnPrint, &QPushButton::clicked, this, &Dialog::printSelected);
}

Dialog::~Dialog()
{
    delete ui;
}

// 搜索 USB 打印机
void Dialog::scanUsbPrinters()
{
    ui->comboPrinters->clear();
    QStringList list = findUsbPrinters();

    if (list.isEmpty()) {
        QMessageBox::warning(this,"提示","未检测到可用USB打印机");
        return;
    }

    ui->comboPrinters->addItems(list);
    QMessageBox::information(this,"扫描完成",QString("共找到%1台打印机").arg(list.count()));
}

// 枚举 USB PRINT 设备
QStringList Dialog::findUsbPrinters()
{
    QStringList paths;

    HDEVINFO hDevInfo = SetupDiGetClassDevsA(&GUID_USBPRINT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE)
        return paths;

    SP_DEVICE_INTERFACE_DATA iface = { 0 };
    iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, 0, &GUID_USBPRINT, i, &iface); i++)
    {
        DWORD size = 0;
        SetupDiGetDeviceInterfaceDetailA(hDevInfo, &iface, 0, 0, &size, 0);
        if (size <= 0) continue;

        PSP_DEVICE_INTERFACE_DETAIL_DATA_A pd = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)malloc(size);
        pd->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

        if (SetupDiGetDeviceInterfaceDetailA(hDevInfo, &iface, pd, size, 0, 0))
        {
            paths << pd->DevicePath;
        }
        free(pd);
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return paths;
}

// 打开 USB 设备
HANDLE Dialog::openUsbDevice(const QString &path)
{
    return CreateFileA(
        path.toLocal8Bit().constData(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
}

// 发送打印数据
bool Dialog::sendPrintData(HANDLE h, const QByteArray &data)
{
    DWORD write = 0;
    bool ok = WriteFile(h, data.constData(), data.size(), &write, NULL);
    CloseHandle(h);
    return ok;
}


void Dialog::printSelected()
{
    QString path = ui->comboPrinters->currentText();
    if (path.isEmpty()) {
        QMessageBox::warning(this,"错误","请选择打印机");
        return;
    }

    HANDLE h = openUsbDevice(path);
    if (h == INVALID_HANDLE_VALUE) {
        QMessageBox::critical(this, "错误", "打开设备失败");
        return;
    }

    // ==============================
     // 🔥 关键：UTF8 → GB2312 编码
     // ==============================
    QString content = QString(R"(
    ===== USB Print =====
    测试打印成功
    中文正常
    日期：%1
    时间：%2
    ====================
    )").arg(__DATE__).arg(__TIME__);

    QTextCodec *codec = QTextCodec::codecForName("GB2312");
    QByteArray printData = codec->fromUnicode(content);

    // 打印指令
    QByteArray cmd;
    cmd += "\x1B\x40";        // 初始化
    cmd += "\x1B\x61\x01";    // 居中
    cmd += printData;         // GB2312 内容
    cmd += "\n\n\n";
    cmd += "\x1D\x56\x42\x00";// 切纸
    cmd += "\x1B\x40";// 切纸
    bool ok = sendPrintData(h, cmd);
    if (ok)
        QMessageBox::information(this, "成功", "打印指令已发送！");
    else
        QMessageBox::critical(this, "失败", "打印失败");
}

