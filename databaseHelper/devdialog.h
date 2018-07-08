#ifndef DEVDIALOG_H
#define DEVDIALOG_H

#include <QDialog>
#include <QFile>

class QToolButton;
class QTextEdit;
class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;
class QFile;
class QKeyEvent;
class QLineEdit;
class QLabel;



// 外设配置窗体
class DevDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DevDialog(QDialog *parent = nullptr);


    // 初始化对话框
    void InitUI();

    // 创建按钮
    void InitButton();

    // 创建textedit
    void InitTextEdit();

    // 创建显示文件名LineEdit
    void InitLineEdit();

    // 初始化窗体其他
    void InitOther();

    // 返回textedit中的内容是否变化，false - 没有变化， true - 变化了
    bool GetTextEditContentIsChanged();


    // 退出前，检查文件是否关闭
    void UnInit();

    // 检查文件是否打开
    bool GetFileIsOpen();


    // 关闭文件
    bool CloseFile();

    // 保存文件
    void SaveFile();

    // 检查文件是否关闭，若没有，则关闭并保存文件
    void CheckFileClose();

signals:

public slots:
    // 打开按钮槽函数
    void btnOpenFileSlot();

    // 关闭按钮槽函数
    void btnCloseFileSlot();

private:
    // 只显示打开文件内容
    void GetFileContext(QString openNewFileName);


protected:
    // 按键
    void keyPressEvent(QKeyEvent *e);


private:
    // 打开和关闭按钮
    QToolButton     *btnOpenFile;
    QToolButton     *btnCloseFile;

    // 显示文件内容
    QTextEdit      *textEditShowFile;

    // 栅格布局
    QGridLayout     *gridLayout;

    // 保存打开的文件，关闭时使用
    QString         openFileName;

    // 按钮水平布局
    QHBoxLayout     *hBtnLayout;
    //整体垂直布局
    QVBoxLayout     *vLayout;

    // 临时保存文件内容
    QString         textEditContent;

    // 文件读写指针
    QFile           *fileReadWrite;

    // 显示当前打开的文件
    QLineEdit       *lineEditFileName;

    // 文件名
    QLabel          *labelFileName;

};


#endif // DEVDIALOG_H
