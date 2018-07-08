#include "devdialog.h"
#include <QTextEdit>
#include <QToolButton>
#include <QGridLayout>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>
#include <QLabel>
#include <QTextBlock>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QIcon>


DevDialog::DevDialog(QDialog *parent) : QDialog(parent)
  ,fileReadWrite(nullptr)
{
   InitUI();
}

//
void DevDialog::InitUI()
{
    textEditContent = QString("");

    // 创建文件读写
    fileReadWrite   = new QFile;


    // 创建按钮
    InitButton();

    // 创建textedit
    InitTextEdit();

    // 创建显示文件名的控件
    InitLineEdit();

    // 初始化其他
    InitOther();
}

// 创建按钮
void DevDialog::InitButton()
{
    // 打开文件
    btnOpenFile     = new QToolButton;
    btnOpenFile->setText(tr("open a file"));
    btnOpenFile->setToolTip(tr("open a file"));
    connect(btnOpenFile, SIGNAL(clicked(bool)), this, SLOT(btnOpenFileSlot()));
    btnOpenFile->setIcon(QIcon(":/res/ico/dev/open"));
    btnOpenFile->setFixedSize(80, 48);
    btnOpenFile->setIconSize(QSize(80, 48));



    // 关闭文件
    btnCloseFile    = new QToolButton;
    btnCloseFile->setText(tr("close file"));
    btnCloseFile->setToolTip(tr("close file"));
    connect(btnCloseFile, SIGNAL(clicked(bool)), this, SLOT(btnCloseFileSlot()));
    btnCloseFile->setIcon(QIcon(":/res/ico/dev/save"));
    btnCloseFile->setFixedSize(80, 48);
    btnCloseFile->setIconSize(QSize(80, 48));
}

// 创建textedit
void DevDialog::InitTextEdit()
{
    textEditShowFile    = new QTextEdit;

    // 初始化时，设置为只读
    textEditShowFile->setReadOnly(true);
}

// 创建显示文件名LineEdit
void DevDialog::InitLineEdit()
{
    lineEditFileName    = new QLineEdit;
    lineEditFileName->setReadOnly(true);
    lineEditFileName->setToolTip(tr("show a file name"));
    lineEditFileName->setPlaceholderText(tr("show the name of opening file"));
    lineEditFileName->setFixedSize(400, 30);


}

// 初始化窗体其他
void DevDialog::InitOther()
{
    // 1、创建布局
//    gridLayout          = new QGridLayout;

    hBtnLayout          = new QHBoxLayout;
    vLayout             = new QVBoxLayout;

    // 2、添加布局
    hBtnLayout->addWidget(btnOpenFile);
    hBtnLayout->addWidget(btnCloseFile);
    hBtnLayout->addWidget(lineEditFileName);
    hBtnLayout->setSpacing(10);

    vLayout->addLayout(hBtnLayout);
    vLayout->addWidget(textEditShowFile);
    vLayout->setSpacing(10);
    vLayout->setContentsMargins(10, 10, 10, 10);



    // 3、显示布局
    setLayout(vLayout);

    // 4、重置文件路径
    openFileName    = QString("");

    setAcceptDrops(true);
}

//
// 初始化结束
//
///----------------------------------------------------------------------------------------




// 返回textedit中的内容是否变化，false - 没有变化， true - 变化了
bool DevDialog::GetTextEditContentIsChanged()
{
    QString curStr  = textEditShowFile->toPlainText();
    if (curStr      != textEditContent)
    {
        return true;
    }

    return false;
}

// 退出前，检查文件是否关闭
void DevDialog::UnInit()
{
    // 1、检查文件是否关闭
    CheckFileClose();

    delete fileReadWrite;
    fileReadWrite       = nullptr;
}



// 检查文件是否打开
bool DevDialog::GetFileIsOpen()
{
    bool openFlag   = fileReadWrite->isOpen();

    return openFlag;
}

// 关闭文件
bool DevDialog::CloseFile()
{
    bool fileIsOpen    = fileReadWrite->isOpen();
    if (false           == fileIsOpen)
    {
        return false;
    }

    fileReadWrite->close();
    return true;
}


//----------------------------------------------------------------------
//
// 打开按钮槽函数
void DevDialog::btnOpenFileSlot()
{
    /*
       getOpenFileName函数说明
       函数原形： QStringList QFileDialog::getOpenFileNames(
       QWidget * parent = 0,
       const QString & caption = QString(),    //  打开文件对话框的标题
       const QString & dir = QString(),            //  查找目录
       const QString & filter = QString(),     //  设置需要过滤的文件格式
       QString * selectedFilter = 0,
       Options options = 0) [static]
       */
       //---获取文件名;
    QString qexeFullPath    = QDir::currentPath();
    QString fileName        = QFileDialog :: getOpenFileName(this, tr("选择一个文件"), qexeFullPath, "*.txt");

    // 1、若没有选择文件
    if (true                == fileName.isEmpty())
    {
        // 什么也不做
        return;
    }

    // 2、选择了文件，打开新选择的文件前，检查先前的文件
    CheckFileClose();

    // 3、 打开文件，显示文件内容
    GetFileContext(fileName);

}

// 关闭按钮槽函数
void DevDialog::btnCloseFileSlot()
{
    // 1、检查文件是否关闭
    CheckFileClose();

}

// 只显示打开文件内容
void DevDialog::GetFileContext(QString openNewFileName)
{
    // 之前已经保证文件关闭了，现在重新打开文件

    // 2、 打开文件
    fileReadWrite->setFileName(openNewFileName);
    bool openFlag           = fileReadWrite->open(QIODevice ::ReadWrite | QIODevice ::Text);
    // 若打开失败
    if (false               == openFlag)
    {
        QMessageBox::critical(this, tr("warning"),  tr("open file err"));
        return;
    }

    // 保存文件名
    openFileName            = openNewFileName;

    // 3.1 删除原有的内容
    textEditShowFile->clear();

    // 3.2 显示文件内容
    QTextStream textStream(fileReadWrite);
    while (!textStream.atEnd())
    {
        //---QtextEdit按行显示文件内容
        textEditShowFile->append(textStream.readLine());
    }

    // 5、解除只读
    textEditShowFile->setReadOnly(false);

    // 6、临时保存当前打开文件内容
    textEditContent = textEditShowFile->toPlainText();

    // 7、显示打开的文件名
    lineEditFileName->setText(openFileName);
}

// 保存文件
void DevDialog::SaveFile()
{

    bool isOpen = fileReadWrite->isOpen();
    // 若文件没有打开
    if (false   == isOpen)
    {
        return;
    }

    // 关闭文件
    fileReadWrite->close();

    fileReadWrite->open(QIODevice ::WriteOnly | QIODevice ::Text | QIODevice::Truncate);
    QString writeStr    = textEditShowFile->toPlainText();
#ifdef QT_DEBUG
    qDebug() << "文件内容 = " << writeStr;
#endif

    // 文件打开了，现在关闭
    QTextStream outFile(fileReadWrite);
    outFile << writeStr << endl;
    outFile.flush();

}

// 检查文件是否关闭，若没有，则关闭并保存文件
void DevDialog::CheckFileClose()
{

    // 1、若已经打开文件，且文件内容发生变化，此时又打开文件，则提示是否保存先前的文件
    bool fileIsOpen         = fileReadWrite->isOpen();
    // 1.1 若打开了， 没有关闭
    if (true                == fileIsOpen)
    {
        // 1.1.1 若文件内容发生变化
        bool isChanged      = GetTextEditContentIsChanged();
        if (true            == isChanged)
        {
            int  okcancel   = QMessageBox::information(this, tr("mention"), tr("dev tab, textEdit's content has changed, do U wanna save ？"), QMessageBox::Ok | QMessageBox::Cancel);

            // 点击了是，则需要保存文件
            if (QMessageBox::Ok == okcancel)
            {
                SaveFile();
            }

            // 点击了否，什么也不做
            else
            {

            }
        }

        // 1.1.2 文件内容没有变化
        else
        {
            // 什么也不做
        }

        // 1.1.3 关闭文件
        fileReadWrite->close();

        // 1.1.4 清空显示内容
        textEditShowFile->clear();

        // 1.1.5 设置为只读
        textEditShowFile->setReadOnly(true);

        // 1.1.6 清空文件内容缓冲区
        textEditContent = QString("");

        // 1.1.6 清除文件名
        lineEditFileName->setText("");
    }
    // 1.2文件没有打开
    else
    {
        // 文件没有打开，什么也不做

    }
}

void DevDialog::keyPressEvent(QKeyEvent *e)
{
    if (!e->modifiers() || (e->modifiers() & Qt::KeypadModifier && e->key() == Qt::Key_Enter))
    {
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        {
            e->ignore();
        }
        break;
        case Qt::Key_Escape:
        {
             e->ignore();
        }
        break;
        default:
            e->ignore();
            return;
        }
    }
    else
    {
        e->ignore();
    }
}










