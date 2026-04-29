#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>

// USB 打印设备 GUID
DEFINE_GUID(GUID_USBPRINT, 0x28d78fad, 0x5a12, 0x11d1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void scanUsbPrinters();    // 搜索
    void printSelected();      // 打印

private:
    Ui::Dialog *ui;

    // 工具函数
    QStringList findUsbPrinters();
    HANDLE openUsbDevice(const QString &path);
    bool sendPrintData(HANDLE h, const QByteArray &data);
};
#endif // DIALOG_H
