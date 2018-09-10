#include "dialog.h"
#include "ui_dialog.h"
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QDebug>
#include <QTabWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QTableView>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QGridLayout>
#include <QSqlQuery>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QDateTime>
#include <QStatusBar>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QEvent>
#include <QIcon>
#include <QScrollBar>



Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    devDialog   = nullptr;

    ui->setupUi(this);

    InitUI();

#ifdef he_use_log
    m_Log.HInit(QString("ABC"));
#endif//he_use_log
}

Dialog::~Dialog()
{

#ifdef he_use_log
    m_Log.HUnInit();
#endif//he_use_log


    if (nullptr != devDialog)
    {
        devDialog->UnInit();

        delete devDialog;
        devDialog  = nullptr;
    }

    delete ui;
}

// 初始化创建对话框
void Dialog::InitUI()
{

    // 重新调整窗体大小
    resize(800, 600);
    setMinimumSize(600, 480);

    curTableViewColumnNameList.clear();

    // 设置数据库连接名
    databaseConnectionName    = QString("helperConnection");

    // 指定数据库驱动
    InitSqliteDatabase();

    // 创建sqltableModel
    InitSqlTableModel();

    // 创建菜单栏
    InitMenuBar();

    // 创建工具栏
    InitToolBar();

    // 创建tableView
    InitTableView();

    // 创建 tab widget
    InitTabWidget();

    // 创建状态栏
    InitStatusBar();

    // 创建外设窗体
    InitDevDialog();

    // 初始化窗体其他属性
    InitOther();
}

// 创建菜单
void Dialog::InitMenuBar()
{
    // 1、开始菜单
    topMenuBar              = new QMenuBar(this);

    // 2、创建开始按钮
    topMenuStart            = new QMenu(tr("Start(&s)"));

    // 创建 打开 、设置、 退出菜单
    topMenuActionStart[0]   = new QAction(tr("Open(&o)"));
    // 添加提示
    topMenuActionStart[0]->setToolTip(tr("open a file"));
    // 设置菜单前的ico图标
    topMenuActionStart[0]->setIcon(QIcon(":/res/ico/menu/open"));

    topMenuActionStart[1]   = new QAction(tr("Setting(&s)"));
    topMenuActionStart[1]->setToolTip(tr("can get most data"));
    topMenuActionStart[1]->setIcon(QIcon(":/res/ico/menu/setting"));

    topMenuActionStart[2]   = new QAction(tr("Exit(&e)"));
    topMenuActionStart[2]->setToolTip(tr("exit the app"));
    topMenuActionStart[2]->setIcon(QIcon(":/res/ico/menu/exit"));

    // 添加菜单
    topMenuStart->addAction(topMenuActionStart[0]);
    topMenuStart->addAction(topMenuActionStart[1]);
    topMenuStart->addAction(topMenuActionStart[2]);


    // 初始化是，隐藏 设置
    topMenuActionStart[1]->setVisible(false);

    // 关联点击槽函数
    connect(topMenuActionStart[0], SIGNAL(triggered()), this, SLOT(TopMenuStartSlotOpen()));
    connect(topMenuActionStart[1], SIGNAL(triggered()), this, SLOT(TopMenuStartSlotSetting()));
    connect(topMenuActionStart[2], SIGNAL(triggered()), this, SLOT(TopMenuStartSlotExit()));




    // 3、创建帮助按钮
    topMenuHelp             = new QMenu(tr("Help(&h)"));
    // 关于菜单
    topMenuActionHelp[0]    = new QAction(tr("About(&a)"));
    topMenuActionHelp[0]->setToolTip(tr("help"));
    topMenuHelp->addAction(topMenuActionHelp[0]);
    connect(topMenuActionHelp[0], SIGNAL(triggered()), this, SLOT(TopMenuHelpSlotAbout()));
    topMenuActionHelp[0]->setIcon(QIcon(":/res/ico/menu/about"));

    // 4、添加菜单
    topMenuBar->addMenu(topMenuStart);
    topMenuBar->addMenu(topMenuHelp);
}

// 创建工具栏
void Dialog::InitToolBar()
{
    topToolWidgetHLayout    = new QHBoxLayout;

    // 1、创建工具栏 操作 tablview的按钮
    ///-----------------------------------------------------------------------------
    QStringList nameList;
    nameList << tr("top") << tr("up") << tr("down")  << tr("last") << tr("delete") << tr("add") << tr("submit");

    // 最前、上一个、下一个、最后、添加、删除、提交
    for (int i = 0; i < he_tool_bar_btn_7; i++)
    {
        topToolBtnArr[i]    = new QToolButton;
        topToolBtnArr[i]->setText(nameList.at(i));
        topToolBtnArr[i]->setToolTip(nameList.at(i));
        topToolBtnArr[i]->setFixedSize(40, 30);
    }


    // 绑定槽函数
    // 最前
    connect(topToolBtnArr[0], SIGNAL(clicked(bool)), this, SLOT(TopToolBtnSlotTop()));
    topToolBtnArr[0]->setIcon(QIcon(":/res/ico/toolbar/forward"));

    // 上一个
    connect(topToolBtnArr[1], SIGNAL(clicked(bool)), this, SLOT(TopToolBtnSlotUp()));
    topToolBtnArr[1]->setIcon(QIcon(":/res/ico/toolbar/previous"));

    // 下一个
    connect(topToolBtnArr[2], SIGNAL(clicked(bool)), this, SLOT(TopToolBtnSlotDown()));
    topToolBtnArr[2]->setIcon(QIcon(":/res/ico/toolbar/next"));

    // 最后
    connect(topToolBtnArr[3], SIGNAL(clicked(bool)), this, SLOT(TopToolBtnSlotLast()));
    topToolBtnArr[3]->setIcon(QIcon(":/res/ico/toolbar/last"));

    // 删除
    connect(topToolBtnArr[4], SIGNAL(clicked(bool)), this, SLOT(TopToolBtnSlotDelete()));
    topToolBtnArr[4]->setIcon(QIcon(":/res/ico/toolbar/del"));

    // 添加
    connect(topToolBtnArr[5], SIGNAL(clicked(bool)), this, SLOT(TopToolBtnSlotAdd()));
    topToolBtnArr[5]->setIcon(QIcon(":/res/ico/toolbar/add"));

    // 提交
    connect(topToolBtnArr[6], SIGNAL(clicked(bool)), this, SLOT(TopToolBtnSlotSubmit()));
    topToolBtnArr[6]->setIcon(QIcon(":/res/ico/toolbar/submit"));


    // 2、创建查找和排序用的控件
    ///-----------------------------------------------------------------------------

    // 根据哪一列查找
    comboQeury          = new QComboBox;
    comboQeury->setFixedSize(100, 30);
    comboQeury->setToolTip(tr("which column query"));
    connect(comboQeury, SIGNAL(currentIndexChanged(QString)), this, SLOT(ComboQueryCurTextChangedSlot(const QString &)));

    // 查找的内容是什么
    lineEditQuery       = new QLineEdit;
    lineEditQuery->setMaxLength(30);
    lineEditQuery->setPlaceholderText(tr("please input sth to query"));
    lineEditQuery->setFixedSize(200, 30);
    connect(lineEditQuery, SIGNAL(textChanged(QString)), this, SLOT(LineEditQueryTextChangedSlot(const QString &)));
    lineEditQuery->setToolTip(tr("please input text to query"));


    // 排序label
    labelSort           = new QLabel;
    labelSort->setText(tr("sort"));
    labelSort->setToolTip(tr("sort"));

    // 选择哪一列排序
    comboSort           = new QComboBox;
    comboSort->setFixedSize(100, 30);
    comboSort->setToolTip(tr("which column sort"));
    connect(comboSort, SIGNAL(currentIndexChanged(QString)), this, SLOT(ComboSortCurTextChangedSlot(const QString &)));
}

// 创建tableview
void Dialog::InitTableView()
{
    // 1、先创建10个，属性设置为隐藏
    for (int i = 0; i < he_tableview_count_10; i++)
    {
        tableViewArr[i]     = new QTableView;
        tableViewArr[i]->setVisible(false);

        // 隐藏列头
        tableViewArr[i]->verticalHeader()->setVisible(false);

        // 设置每次只能选中一行
        tableViewArr[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
    }

    // 设置样式表
    tableViewArr[0]->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
      "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
      "QScrollBar::handle:hover{background:gray;}"
      "QScrollBar::sub-line{background:transparent;}"
      "QScrollBar::add-line{background:transparent;}");

      tableViewArr[0]->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
      "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
      "QScrollBar::handle:hover{background:gray;}"
      "QScrollBar::sub-line{background:transparent;}"
      "QScrollBar::add-line{background:transparent;}");


}


// 指定数据库的驱动
void Dialog::InitSqliteDatabase()
{
    // 解除对数据库的默认连接
    bool isContain      = QSqlDatabase::contains(databaseConnectionName);
    if (true           == isContain)
    {
        sqliteDatabase  = QSqlDatabase::database(databaseConnectionName);
    }
    else
    {
        sqliteDatabase  = QSqlDatabase::addDatabase( "QSQLITE", databaseConnectionName);
    }
}

// 创建sqltableModel
void Dialog::InitSqlTableModel()
{
    //
    for (int i = 0; i < he_tableview_count_10; i++)
    {
        sqlTableModel[i]    = new QSqlTableModel(NULL, sqliteDatabase);
        sqlTableModel[i]->setEditStrategy(QSqlTableModel::OnManualSubmit);
    }
}



// 初始化对话框属性
void Dialog::InitOther()
{
    // 初始化，设置没有添加过外设窗体
    SetAddDevTabFlag(false);
    // 2018年6月24日 23:22:02 改为栅格布局
    gridLayout      = new QGridLayout;

    // 菜单栏， 0行0列 占 1行 11列
    gridLayout->addWidget(topMenuBar, 0, 0, 1, 10);

    // 工具栏按钮部分
    for (int i = 0 ; i < he_tool_bar_btn_7; i++)
    {
        gridLayout->addWidget(topToolBtnArr[i], 1, i, 1, 1);
    }

    // 从哪一列查找 ， 位置，1行 7列， 占1行1列
    gridLayout->addWidget(comboQeury, 1, 7, 1, 1);
    // 输入框，位置：1行8列， 占 1行 1 列
    gridLayout->addWidget(lineEditQuery, 1, 8, 1, 1);

    // 排序
    gridLayout->addWidget(labelSort, 1, 9, 1, 1);
    // 按照哪一列排序
    gridLayout->addWidget(comboSort, 1, 9, 1, 1);

    // tabwidget
    gridLayout->addWidget(tabWidget, 2, 0, 1, 11);

    // 添加状态栏
    gridLayout->addWidget(statusBar, 3, 0, 1, 11);

    // 设置控件之间的间距
    gridLayout->setSpacing(10);

    // 设置栅格布局控件与窗体之间的间距
    gridLayout->setContentsMargins(0, 0, 0, 0);

    // 现实布局
    setLayout(gridLayout);

    // 软件首次启动，设置工具栏控件为禁用 只读
    SetToolBarPosCtrlIsEnable(false);


    // 设置对话框icon
    setWindowIcon(QIcon(":/res/ico/helper.ico"));

    // 设置窗体标题
    setWindowTitle(tr("Config Tool V1.0 By PandaMo"));

    // 设置窗体最大化和最小化
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlag);

    //
    setAcceptDrops(true);


    // 创建定时器
    timer       = new QTimer;
    timer->setInterval(he_timer_interval_1000ms);
    // 定时器槽函数
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeOutUpdateSlot()));
    // 启动定时器
    timer->start();
}

// 创建床体上 的tabwidget
void Dialog::InitTabWidget()
{
    tabWidget                   = new QTabWidget();

//    tabWidget->resize(this->width(), this->height());

    // 关联tab页点击槽函数
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(TabWidgetWhichTabIsDown(int )));
}

// 读取数据库内容
void Dialog::ReadDbData()
{
    // 1、设置数据库名
    sqliteDatabase.setDatabaseName(configFilePath);

    // 2、尝试打开数据库
    bool openFlag           = sqliteDatabase.open();

    // 2.1 若打开失败
    if (false               == openFlag)
    {
        QMessageBox::critical(this, tr("warning"),  tr("open database file err"));
        return;
    }

    // 3、读取数据库中的所有表名， 排除重复的表名 和 sqlite_sequence
    QStringList tableNameList   = sqliteDatabase.tables();

#ifdef QT_DEBUG
    qDebug() << "查询的表名的个数= " << tableNameList.count();
#endif // QT_DEBUG


    ///------------------------------------------------------------------
    // 读取数据库中的表名
    realTableName.clear();
    int realTableNameCount  = 0;
    bool isFind             = false;
    foreach (QString name, tableNameList)
    {
        isFind  = false;
        // sqlite_sequence 排除
        if ( QString("sqlite_sequence") == name)
        {
            continue;
        }

        realTableNameCount  = realTableName.count();
        // 若已经存在了，跳过
        for (int i = 0; i < realTableNameCount; i++)
        {
            // 已经存在
            if (name    == realTableName.at(i))
            {
                isFind  = true;
                break;
            }
        }

        // 若没有找到，则添加到列表中
        if (false == isFind)
        {
            realTableName << name;
        }
    }

    // 4、读取数据库表中的数据，添加到tableView中显示
    int sqlTableModelIndex  = 0;
    int index    = 0;
    foreach (QString name, realTableName)
    {
        QSqlTableModel *pModel      = sqlTableModel[sqlTableModelIndex++];

        // 设置连接的数据库名
        pModel->setTable(name);

        pModel->setSort(0,  Qt::AscendingOrder);

        // 设置手动提交
        pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        // 查询数据
        pModel->select();
        // 添加到tableview
        tableViewArr[index]->setModel(pModel);

        // 添加到tabwidget
        tabWidget->addTab(tableViewArr[index], name);

        // 解除对256行的限制
        while(pModel->canFetchMore())
        {
            pModel->fetchMore();
        }

        // 计数器 +1
        index   += 1;

        // 只显示第一张表的内容
        if (1 == index)
        {
            break;
        }
    }

    // 5、设置tabwidget显示
    tabWidget->setVisible(true);

    // 7、显示设置菜单
    topMenuActionStart[1]->setVisible(true);

    //8、设置当前活动的tab
    tabWidget->setCurrentIndex(0);
}

// 添加最后一个外设tab
void Dialog::InsertDevTab()
{
//    InitDevDialog();
    AddDevDialog();
}

// 添加下拉项
void Dialog::ComboInsertItem(int whichTabIsSelected)
{
    // 1、获取当前活动的tab
    // 1.1 若没有tab
    if(-1               == whichTabIsSelected)
    {
        return;
    }

    // 2、清空现有下拉框中的内容
    comboQeury->clear();
    comboSort->clear();

    // 3、获取有哪些列名
    // 3.1 得到表名
    QString tableName   = sqlTableModel[whichTabIsSelected]->tableName();

    // 3.2 查询表对应的字段列名
    QString str = "PRAGMA table_info(" + tableName + ")";
    QSqlQuery query(str, sqliteDatabase);

    bool queryFlag  = query.exec();
    if (false       == queryFlag)
    {
        QMessageBox::critical(this, tr("warning"),  tr("query table data err"));
        return;
    }

    // 3.3 保存字段名
    QStringList columnName;
    QString tmpName("");
    while (true     == query.next())
    {
        tmpName     = query.value(1).toString();
#ifdef QT_DEBUG
        qDebug() << "查到的列名 = "  << tmpName;
#endif // QT_DEBUG
        columnName << tmpName;
    }

#ifdef QT_DEBUG
    qDebug() << "列名的个数 = " << columnName.count();
#endif//QT_DEBUG

    // 4、添加到下拉框中
    // 根据哪一列查找
    comboQeury->addItems(columnName);

    // 5、根据哪一列排序
    foreach (QString name, columnName)
    {
        // 列名升序
        tmpName =  name + " ASC";
        comboSort->addItem(tmpName);
        // 降序
        tmpName =  name + " DESC";
        comboSort->addItem(tmpName);
    }

    // 设置默认选择第一个
    comboQeury->setCurrentIndex(0);
    comboSort->setCurrentIndex(0);

    // 保存当前表的列名
    curTableViewColumnNameList.clear();
    curTableViewColumnNameList  = columnName;
}

// 添加外设窗体
void Dialog::InitDevDialog()
{
    devDialog   = new DevDialog;
}

// 创建状态栏
void Dialog::InitStatusBar()
{
    statusBar       = new QStatusBar;
    // 显示时间的label
    labelShowTime   = new QLabel;
    statusBar->addWidget(labelShowTime, 1);
}

// 设置工具栏位置上的控件的状态
void Dialog::SetToolBarPosCtrlIsEnable(bool isEnable)
{
    // 7个按钮
    for (int i = 0; i < he_tool_bar_btn_7; i++)
    {
        topToolBtnArr[i]->setEnabled(isEnable);
    }


    // 2、禁用
    bool lineEditReadOnly   = isEnable ? false : true;

    // 设置 只读和 禁用
    comboQeury->setEnabled(isEnable);
    lineEditQuery->setReadOnly(lineEditReadOnly);
    comboSort->setEnabled(isEnable);
}



//---------------------------------------------------------------
// // 菜单槽函数

// open slot
void Dialog::TopMenuStartSlotOpen()
{

#ifdef he_use_log
    m_Log.HLogTime(QString("void Dialog::TopMenuStartSlotOpen() start"));
#endif // he_use_log





    // 0、检查外设标签是否添加
    bool isAddDev               = GetAddDevTabFlag();

    // 0.1 添加了
    if (true                    == isAddDev)
    {
        // 外设窗体中文件打开没有关闭
        bool devFileIsOepn      = devDialog->GetFileIsOpen();
        if (true                == devFileIsOepn)
        {
            int okcancel        = QMessageBox::warning(this, tr("tip"), tr("dev file has opended, do U wanna go?"), QMessageBox::Ok | QMessageBox::Cancel);

            // 若选择否
            if  (QMessageBox::Cancel    == okcancel)
            {
                // 选择否，不继续打开新的文件
                return;
            }

            // 选择是，则继续

        }

        // 检查文件是否关闭
        devDialog->CheckFileClose();
    }




    // 1、打开文件
    QString fileName            = QFileDialog::getOpenFileName(this,tr("Open a config file"),QString(),"DB (*.db)");
    if (true                    == fileName.isEmpty())
    {
        // 1.1 没有选择文件，什么也不做
#ifdef QT_DEBUG
        qDebug() << "没有选择文件";
#endif//QT_DEBUG
        return;
    }

#ifdef he_use_log
    m_Log.HLogTime(QString("void Dialog::TopMenuStartSlotOpen() choose file: %0").arg(fileName));
#endif // he_use_log


    // 2、选择了文件，读取数据库数据
    PreOpenFile();


    // 3、尝试打开并获取文件内容
    OpenFile(fileName);
}





// set slot
void Dialog::TopMenuStartSlotSetting()
{
    // setting， 数据库已经打开过了，这里只需要重新读取数据库内容即可

    //---------------------------------------------------------------
    // 1、清理tab
    ClearTab();


    // 2、读取全部表的内容
//    int tableCount  = realTableName.count();
    int index    = 0;
    foreach (QString name, realTableName)
    {

        QSqlTableModel *pModel  = sqlTableModel[index];

        // 设置连接的数据库名
        pModel->setTable(name);

        pModel->setSort(0,  Qt::AscendingOrder);

        // 设置手动提交
        pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        // 查询数据
        pModel->select();
        // 添加到tableview
        tableViewArr[index]->setModel(pModel);

        // 添加到tabwidget
        tabWidget->addTab(tableViewArr[index], name);

        tabWidget->setTabIcon(index, QIcon(":/res/ico/tab/tab"));

        // 解除对256行的限制
        while(pModel->canFetchMore())
        {
            pModel->fetchMore();
        }

        // 计数器 +1
        index   += 1;
    }

    // 3、最后一个tab添加外设窗体
    InsertDevTab();

    // 4、设置当前标题
    QString titleStr    = GetatabaseFilePath() + QString(" - DataHelper");
    setWindowTitle(titleStr);
}



// exit slot
void Dialog::TopMenuStartSlotExit()
{
    // 退出前，需要检查项
    PreDestroyDialog();

    QApplication::exit(0);
#ifdef QT_DEBUG
    qDebug() << " exit success";
#endif //QT_DEBUG
}

// help slot
void Dialog::TopMenuHelpSlotAbout()
{
    QMessageBox::information(this, tr("Helper"), tr("Copyright © 2018 - 2018 PandaMo.  All rights reserved"));
}
//---------------------------------------------------------------





//---------------------------------------------------------------------------
//
// 工具栏槽函数

// 最前
void Dialog::TopToolBtnSlotTop()
{

    // 1、获取当前选择的是哪一个tab
    int curTabIndex         = tabWidget->currentIndex();
    // 若没有tab
    if (-1                  == curTabIndex)
    {
        QMessageBox::warning(this, tr("warning"), tr("err, there is no tab"));
        return;
    }

    // 2、设置选中第一行
    int tableViewCount      = tableViewArr[curTabIndex]->model()->rowCount();
    if (0 < tableViewCount)
    {
        tableViewArr[curTabIndex]->selectRow(0);
    }

}

// 上一个
void Dialog::TopToolBtnSlotUp()
{
    // 1、获取当前选择的是哪一个tab
    int curTabIndex         = tabWidget->currentIndex();
    // 若没有tab
    if (-1                  == curTabIndex)
    {
        QMessageBox::warning(this, tr("warning"), tr("err, there is no tab"));
        return;
    }

    // 2、获取当前行
    int curRowIndex         = tableViewArr[curTabIndex]->currentIndex().row();
#ifdef QT_DEBUG
    qDebug() << "上一个 ， 当前行索引 = "  << curRowIndex;
#endif //QT_DEBUG

    // 若没有选择，则默认选择第一个
    if (-1                  == curRowIndex)
    {
        curRowIndex         = 0;
    }
    else
    {
        if  (0 < curRowIndex)
        {
            curRowIndex         -= 1;
        }

    }
    tableViewArr[curTabIndex]->selectRow(curRowIndex);
}

// 下一个
void Dialog::TopToolBtnSlotDown()
{
    // 1、获取当前选择的是哪一个tab
    int curTabIndex         = tabWidget->currentIndex();
    // 若没有tab
    if (-1                  == curTabIndex)
    {
        QMessageBox::warning(this, tr("warning"), tr("err, there is no tab"));
        return;
    }

    // 2、获取当前行
    int curRowIndex         = tableViewArr[curTabIndex]->currentIndex().row();
    int totalCount          = tableViewArr[curTabIndex]->model()->rowCount();

    // 若没有选择行
    if (-1                  == curRowIndex)
    {
        curRowIndex         = 0;
    }
    else
    {
#ifdef QT_DEBUG
        qDebug() << "下一个，当前行 = " << curRowIndex << "    总行数 = " << totalCount;
#endif
        if  (curRowIndex < totalCount - 1)
        {
            curRowIndex         += 1;
        }
    }

#ifdef QT_DEBUG
    qDebug() << "下一个， +1 后 = " << curRowIndex;
#endif
    tableViewArr[curTabIndex]->selectRow(curRowIndex);

}

// 最后
void Dialog::TopToolBtnSlotLast()
{
    // 1、获取当前选择的是哪一个tab
    int curTabIndex         = tabWidget->currentIndex();
    // 若没有tab
    if (-1                  == curTabIndex)
    {
        QMessageBox::warning(this, tr("warning"), tr("err, there is no tab"));
        return;
    }

    // 2、设置选中最后一行
    int totalCount          = tableViewArr[curTabIndex]->model()->rowCount();
    tableViewArr[curTabIndex]->selectRow(totalCount - 1);
}

// 删除
void Dialog::TopToolBtnSlotDelete()
{

    // 1、得到当前选择的是哪一个tab
    int curTabIndex                 = tabWidget->currentIndex();
    if (-1                          == curTabIndex)
    {
        QMessageBox::warning(this, tr("warning"), tr("delete failed, because there is no tab"));
        return;
    }

    // 2、得到选中的行
    QItemSelectionModel *selections = tableViewArr[curTabIndex]->selectionModel();
    QModelIndexList selected        = selections->selectedIndexes();

    // 保存选择了哪些行
    QVector <int> delRowsArr;
    delRowsArr.clear();
    foreach (QModelIndex index, selected)
    {
        int rowIndex    = index.row();
#ifdef QT_DEBUG
        qDebug() << "当前行为 = " << rowIndex + 1;
#endif
        int size = delRowsArr.count();
        bool isExist        = false;
        for (int i = 0; i < size; i++)
        {
            // 若删除行已经存在
            if (delRowsArr[i]   == rowIndex)
            {
                isExist         = true;
                break;
            }
        }

        // 若已经存在，则不加入到删除数组中
        if (true                == isExist)
        {

        }
        else
        {
            delRowsArr.append(rowIndex);
        }

    }

    const int delArrCount           = delRowsArr.count();
    if (0                           == delArrCount)
    {
        QMessageBox::warning(this, tr("warning"), tr("delete failed, because there is no selected row"));
        return ;
    }

    // 3、确认是否删除
    int isOk                        = QMessageBox::warning(this, tr("warning"), tr("Are u sure del the selected row"), QMessageBox::Yes,    QMessageBox::No);
    if (QMessageBox::No             == isOk)
    {
        return;
    }

#ifdef QT_DEBUG
    qDebug() << "删除的行总数 = " << delArrCount;
#endif
    for (int i; i < delArrCount; i++)
    {
        sqlTableModel[curTabIndex]->removeRow(delRowsArr[i]);
    }

    // 4、提交
    bool isSuccess                  =  sqlTableModel[curTabIndex]->submitAll();//tableViewArr[curTabIndex]->model()->submit();
    if (false                       == isSuccess)
    {
        // 删除提交失败，则回滚
        sqlTableModel[curTabIndex]->revertAll();

        // 提示删除失败
        QMessageBox::warning(this, tr("warning"), tr("delete falied , submit failed"));
        return;
    }

#ifdef QT_DEBUG
    qDebug() << "delete success , submit success";
#endif
}

// 添加
void Dialog::TopToolBtnSlotAdd()
{

    // 1、获取当前选择行
    int curTabIndex                 = tabWidget->currentIndex();
    // 1.1 若没有选择行
    if (-1                          == curTabIndex)
    {

        QMessageBox::information(this, tr("tip"), tr("err, threre is no tab"));
        return;
    }

    // 2、获取当前选择行
    int rowIndex                    = tableViewArr[curTabIndex]->currentIndex().row();

    // 添加到哪一行
    int addRowIndex                 = 0;
    // 2.1 若没有选择行
    if (-1                          == rowIndex)
    {
        // 默认设置选择当前tableview的最后一行
        addRowIndex                 = tableViewArr[curTabIndex]->model()->rowCount();
    }
    // 2.2 若已经选择行
    else
    {
        // 添加到选择行的下一行
        addRowIndex                 = rowIndex + 1;
    }

    // 3、向sqltableModel中添加一行
    bool isAdd                      = sqlTableModel[curTabIndex]->insertRow(addRowIndex);
    // 3.1 若添加失败
    if (false                       == isAdd)
    {
        // 提示添加失败
        QMessageBox::warning(this, tr("tip"), tr("add failed, add a new row failed"));
        return;
    }
}

// 提交
void Dialog::TopToolBtnSlotSubmit()
{
    // 1、获取当前选择的是哪一个tab
    int curTabIndex                 = tabWidget->currentIndex();

    // 若没有tab
    if (-1                          == curTabIndex)
    {
        QMessageBox::warning(this, tr("tip"), tr("err,  there is no tab"));
        return;
    }

    // 2、提交当前tab下的新增记录
    int isSubmitSuccess             = sqlTableModel[curTabIndex]->submitAll();
    // 2.1 提交失败
    if (false                       == isSubmitSuccess)
    {
        QMessageBox::warning(this, tr("submit"), tr("the behavior of submit has failed, Please  Check new line's Id"));
        return;
    }

}

// 哪一列查询变化槽函数
void Dialog::ComboQueryCurTextChangedSlot(const QString &text)
{
#ifdef QT_DEBUG
    qDebug() << "选择查找的列是 = " << text;
#endif

    if (true    == text.isEmpty())
    {
        return;
    }

    QString keyStr  = lineEditQuery->text();
    ShowDataOfQuery(text, keyStr);

}

// 查找输入框内容变化了
void Dialog::LineEditQueryTextChangedSlot(const QString &text)
{
    QString str = text;
#ifdef QT_DEBUG
    qDebug() << "输入框内容 = " << str;
#endif
    QString columnName  = comboQeury->currentText();
    if (true    == columnName.isEmpty())
    {
        return;
    }

    ShowDataOfQuery(columnName, str);

}

// 选择哪一列排序
void Dialog::ComboSortCurTextChangedSlot(const QString &text)
{
    // 0、哪一个tab被选中
    int curTabIndex = tabWidget->currentIndex();
    if (-1          == curTabIndex)
    {
        return;
    }


    QString curStr  = text;

    // 1、每一项都是 列名 + 空格 + 排序方式
    QString columnName("");
    QString sortType("");

    int spacePos    = curStr.indexOf(" ");
    // 2、列名
    columnName      = curStr.mid(0, spacePos);

#ifdef QT_DEBUG
//    qDebug() << "列名= " << columnName;
#endif
    // 3、排序方式
    sortType        = curStr.mid(spacePos + 1, curStr.length());
#ifdef QT_DEBUG
//    qDebug() << "排序方式= " << sortType;
#endif

    // 4、按照选择的排序方式排序
    // 4.1 按照哪一列排序
    int sortIndex = 0;
    for (int i = 0; i < curTableViewColumnNameList.count(); i++)
    {
        // 若相等
        if (columnName  == curTableViewColumnNameList.at(i))
        {
            break;
        }

        sortIndex       += 1;
    }


    // 升序还是降序
    // 0 - 升序， 1 降序
    if ("ASC"           == sortType)
    {
        // 5、排序
        sqlTableModel[curTabIndex]->setSort(sortIndex, Qt::AscendingOrder);
    }
    else if ("DESC"     == sortType)
    {
        // 5、排序
        sqlTableModel[curTabIndex]->setSort(sortIndex, Qt::DescendingOrder);
    }


    // 查询现实
    sqlTableModel[curTabIndex]->select();

}

// tab哪一个标签被激活了
void Dialog::TabWidgetWhichTabIsDown(int index)
{
#ifdef QT_DEBUG
    qDebug() << "选择的是 = " << index;
#endif

    bool isAddDevFlag   = GetAddDevTabFlag();
    int curTabCount     = tabWidget->count();


    if (0 > index)
    {
        return;
    }


    //--------------------------------------------------------------------

    // 2、已经添加外设标签了
    if (true            == isAddDevFlag)
    {
        // 2.1 当前选择的是否为最后一个tab， 即 dev 标签
        if ( (index + 1)       == curTabCount)
        {
            // 设置工具栏控件为只读
            SetToolBarPosCtrlIsEnable(false);
            return;
        }
        else
        {
            // 虽然添加了 外设 tab, 但选择的不是最后一个tab
            // 什么也不做
        }
    }

    // 3、外设tab 没有添加
    // 3.1 设置工具栏控件为可用，
    SetToolBarPosCtrlIsEnable(true);

    // 3.2 下拉框重新添加当前选择的tab下的列名
    ComboInsertItem(index);
}

// 定时器槽函数
void Dialog::TimeOutUpdateSlot()
{
    UpdateStatusBarData();
}




//---------------------------------------------------------------------------

// 退出窗体前，
int Dialog::PreDestroyDialog()
{
    int retVal          = 0;

    // 3、文件存在，判断是否打开
    bool isOpen         = sqliteDatabase.isOpen();
    // 3.1 若没有打开
    if (false           == isOpen)
    {
#ifdef QT_DEBUG
        qDebug() << "数据库没有打开";
        retVal          = 0;
        return retVal;
#endif
    }
    // 3.2 打开了数据库，再关闭
    else
    {
        sqliteDatabase.close();
#ifdef QT_DEBUG
        qDebug() << "数据库已经打开，现在已经关了";
#endif
    }

    // 4、移除数据库连接
    QSqlDatabase::removeDatabase("QSQLITE");


    // 5、停止定时器
    bool isActive   = timer->isActive();
    if (true        == isActive)
    {
        timer->stop();
    }

    // 6、是否添加外设标签
   // 没有添加外设
    bool isAddDevTab    = GetAddDevTabFlag();
    if (false           == isAddDevTab)
    {
        retVal          = 0;
        return retVal;
    }

    // 7、添加了外设tab ，
    devDialog->CheckFileClose();
}

// 更新状态栏
void Dialog::UpdateStatusBarData()
{
    // 1、刷新当前时间
    QDateTime time      = QDateTime::currentDateTime();
    QString labelStr    = time.toString("hh:mm:ss ddd MM/dd/yyyy");
    labelShowTime->setText(labelStr);
}

// 根据条件查找显示数据
void Dialog::ShowDataOfQuery(const QString columnNameStr, const QString keyStr)
{

    QString str = keyStr;
#ifdef QT_DEBUG
    qDebug() << "输入框内容 = " << str;
#endif

    // 1、获取当前选择的是哪一个tab
    int curTabIndex = tabWidget->currentIndex();

    // 若没有tab页
    if (-1          == curTabIndex)
    {
        return;
    }

    // 2、取从哪一列开始查找
    QString columnName  = columnNameStr;

    // 3、若输入框没有输入
    // 输入框有内容，则应该按照选择来筛选
     QString filterStr("");
    filterStr = QString(tr("%1 like '%%2%%'")).arg(columnName).arg(str);
    // 根据条件查找
    sqlTableModel[curTabIndex]->setFilter(filterStr);

    // 4、刷新显示界面数据
    sqlTableModel[curTabIndex]->select();

}

// 打开数据库文件
void Dialog::PreOpenFile()
{


    // 1、 清理tab页
    ClearTab();

    // 2、检查数据库是否关闭
    bool isOpen         = sqliteDatabase.isOpen();

    // 1.1 若已经打开数据库
    if (true            == isOpen)
    {
        // 1.1.1 关闭数据库
        sqliteDatabase.close();
    }
    else
    {
        // 1.1.2 数据库没有打开
#ifdef QT_DEBUG
        qDebug() << "数据库没有打开, 什么也不做";
#endif
    }



    // 3、标记外设窗体没有添加
//    devDialogHasAdded   = false;
    SetAddDevTabFlag(false);

    // 4、清理保存数据库中表名字的数组
    realTableName.clear();
}

//
void Dialog::OpenFile(QString fileName)
{

    // 1、尝试打开新选择的文件，
    sqliteDatabase.setDatabaseName(fileName);

    // 1.1 尝试打开数据库
    bool openFlag           = sqliteDatabase.open();

    // 1.2  若打开失败
    if (false               == openFlag)
    {
        QMessageBox::critical(this, tr("warning"),  tr("open database file err"));
        return;
    }

    // 2、读取数据库中的所有表名， 排除重复的表名 和 sqlite_sequence
    QStringList tableNameList   = sqliteDatabase.tables();
#ifdef QT_DEBUG
    qDebug() << "查询的表名的个数= " << tableNameList.count();
#endif

    ///------------------------------------------------------------------
    // 读取数据库中的表名
    realTableName.clear();
    int realTableNameCount  = 0;
    bool isFind             = false;
    foreach (QString name, tableNameList)
    {
        isFind              = false;
        // sqlite_sequence 排除
        if ( QString("sqlite_sequence") == name)
        {
            continue;
        }

        realTableNameCount  = realTableName.count();
        // 若已经存在了，跳过
        for (int i = 0; i < realTableNameCount; i++)
        {
            // 已经存在
            if (name    == realTableName.at(i))
            {
                isFind  = true;
                break;
            }
        }
        // 若没找到， 说名name是唯一存在的， 则添加到数组
        if (false       == isFind)
        {
            // 若没有找到，则添加到列表中
            realTableName << name;
        }
    }

    // 3、读取数据库表中的数据，添加到tableView中显示
    int sqlTableModelIndex  = 0;
    int index    = 0;
    foreach (QString name, realTableName)
    {
        QSqlTableModel *pModel      = sqlTableModel[sqlTableModelIndex++];

        // 设置连接的数据库名
        pModel->setTable(name);

        pModel->setSort(0,  Qt::AscendingOrder);

        // 设置手动提交
        pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        // 查询数据
        pModel->select();
        // 添加到tableview
        tableViewArr[index]->setModel(pModel);

        // 添加到tabwidget
        tabWidget->addTab(tableViewArr[index], name);

        // 设置tab
        tabWidget->setTabIcon(index, QIcon(":/res/ico/tab/tab"));

        // 解除对256行的限制
        while(pModel->canFetchMore())
        {
            pModel->fetchMore();
        }

        // 计数器 +1
        index   += 1;

        // 只显示第一张表的内容
        if (1 == index)
        {
            break;
        }
    }

    // 4、选择tab默认选择第一个tab
    tabWidget->setCurrentIndex(0);

    // 5、设置工具栏位置的控件可用
    SetToolBarPosCtrlIsEnable(true);

    // 6、设置tabwidget显示
    tabWidget->setVisible(true);

    // 7、显示 开始 - 设置
    topMenuActionStart[1]->setVisible(true);

    // 保存当前选择打开的数据库名
    SetDatabaseFilePath(fileName);

    // 9、标题栏显示当前选择的文件
    QString titleStr    = fileName + QString(" - DataHelper");
    setWindowTitle(titleStr);
}

// 显示外设tab
void Dialog::AddDevDialog()
{
    // 将外设窗体添加到tab中
    tabWidget->addTab(devDialog, "dev");

    int curCount        = tabWidget->count();
    qDebug() << "当前tab个数 = "  << curCount;
    tabWidget->setTabIcon(curCount - 1, QIcon(":/res/ico/tab/dev"));


    // 标记添加了外设窗体
    SetAddDevTabFlag(true);
}


// 移除主窗体上的tab页
void Dialog::ClearTab()
{
    // 检查是添加过外设窗体
    // 1、若已经添加了外设窗体， 先处理外设床体上的情况
    bool isAdd              = GetAddDevTabFlag();
    if (true                == isAdd)
    {
        // 检查外设窗体上文件关闭
        devDialog->CheckFileClose() ;
    }
    // 2、外设tab没有添加
    else
    {
        // 什么也不做
#ifdef QT_DEBUG
            qDebug() << "什么也不做";
#endif
    }


    // 3、获取当前tab页的总页数
    int curTabCount     = tabWidget->count();

    // 4、删除tab页
    while (0 != curTabCount)
    {
        tabWidget->removeTab(0);
        curTabCount = tabWidget->count();
    }

    // 标记外设窗标签没有添加
    SetAddDevTabFlag(false);
}

// 保存添加外设标签
void Dialog::SetAddDevTabFlag(bool isAdd)
{
    devDialogHasAdded   = isAdd;
}

// 获取是否添加外设标签
bool Dialog::GetAddDevTabFlag()
{
    return devDialogHasAdded;
}

// 保存当前选择的数据库文件
void Dialog::SetDatabaseFilePath(QString newPath)
{
    configFilePath  = newPath;
}


// 取当前选择打开的数据库文件
QString Dialog::GetatabaseFilePath()
{
    return configFilePath;
}




// 重写关闭消息
void Dialog::closeEvent(QCloseEvent *e)
{
    PreDestroyDialog();
}

// 按键
void Dialog::keyPressEvent(QKeyEvent *e)
{
#ifdef Q_OS_MAC
    if(e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Period) {
        reject();
    } else
#endif

    // ctrl + o 打开弹出选择选择文件夹
    if ((e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_O))
    {
//        QMessageBox::aboutQt(NULL, "aboutQt");
        TopMenuStartSlotOpen();

        return;
    }


    // 下面是为了过滤 回车 和 ESC， 避免按下时关闭窗体
    if (e->key())
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
                QDialog::keyPressEvent(e);
            return;
        }
    }
    else
    {
        e->ignore();
    }
}

void Dialog::paintEvent(QPaintEvent *e)
{

}

// 拖动进入事件
void Dialog::dragEnterEvent(QDragEnterEvent *event)
{
    // 这里，只接受 sqlite3 格式的数据库文件 *。db
    if(!event->mimeData()->urls()[0].fileName().right(2).compare("db") )
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();//否则不接受鼠标事件
    }

}

void Dialog::dropEvent(QDropEvent *event)
{

    /// 下面为自定义代码
    ///---------------------------------------------------------

    // 1、若配置文件路径变量不为空，则说明已经有数据库文件了
    if (false                   == configFilePath.isEmpty())
    {
        QFileInfo fileInfo(configFilePath);
        bool isExistConfigFile  = fileInfo.isFile();
         // 1.1 若不存在该文件
        if (false               == isExistConfigFile)
        {
#ifdef QT_DEBUG
            qDebug() << "不存在该文件";
#endif  //QT_DEBUG

        }

        // 1.2  存在该文件
        else
        {
            // 若数据库打开，则关闭
            bool isOpen         = sqliteDatabase.isOpen();
            if (true            == isOpen)
            {
                sqliteDatabase.close();
#ifdef QT_DEBUG
                qDebug() << "数据库已经打开，现在已经关闭";
#endif  //QT_DEBUG
            }

            // 将数据库路径设置空
            configFilePath      = QString("");
        }

    }



    // 获取MIMEData
    const QMimeData *qm     = event->mimeData();

    // 得到拖拽的文件路径
    QString dragFilePath   = qm->urls()[0].toLocalFile();

    //-----------------------------------------------------------------

    // 检查dev tab
    // 0、检查外设标签是否添加
    bool isAddDev               = GetAddDevTabFlag();

    // 0.1 添加了
    if (true                    == isAddDev)
    {
        // 外设窗体中文件打开没有关闭
        bool devFileIsOepn      = devDialog->GetFileIsOpen();
        if (true                == devFileIsOepn)
        {
            int okcancel        = QMessageBox::warning(this, tr("tip"), tr("dev file has opended, do U wanna go?"), QMessageBox::Ok | QMessageBox::Cancel);

            // 若选择否
            if  (QMessageBox::Cancel    == okcancel)
            {
                // 选择否，不继续打开新的文件
                return;
            }

            // 选择是，则继续

        }

        // 检查文件是否关闭
        devDialog->CheckFileClose();
    }


    // 尝试打开文件
    PreOpenFile();

    // 3、尝试打开并获取文件内容
    OpenFile(dragFilePath);

}













