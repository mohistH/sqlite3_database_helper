#include "hhelper.h"



HHelper::HHelper()
{

}

QString HHelper::HHGetTmpPath()
{
    char tmpPath[260] = {0};

    HGetTmpPath(tmpPath);

    //
    QString str = QString(QLatin1String(tmpPath));

    return str;
}
