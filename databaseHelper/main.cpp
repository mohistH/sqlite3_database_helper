#include "dialog.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    Dialog w;

#ifndef _LOAD_QSS_

    // 加载样式表
    const QString qssFile(":/qss/style.qss");
    bool isExist    = QFile::exists(qssFile);
    if (true        == isExist)
    {
        QFile file(qssFile);
        bool isOpen = file.open(QFile::ReadOnly);
        // 打开失败
        if (false   == isOpen)
        {

        }
        else
        {
            // 打开成功
            w.setStyleSheet(file.readAll());

            qDebug() << "qss文件打开成功";

            file.close();
        }
    }
#endif

    w.show();

    return a.exec();
}
