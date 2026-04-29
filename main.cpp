#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 强制开启高 DPI 支持（Qt 5.6+ 适用）
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    #endif
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();
}
