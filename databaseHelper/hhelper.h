#ifndef HHELPER_H
#define HHELPER_H


#include <QObject>


extern "C"
{
    #include "include/HFileInterface.h"
}



class HHelper : public QObject
{
public:
    HHelper();

    // 获取临时目录
    static QString HHGetTmpPath();
};

#endif // HHELPER_H
