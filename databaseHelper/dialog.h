#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "constdef.h"
#include <QSqlDatabase>
#include "devdialog.h"


class QMenu;
class QMenuBar;
class QAction;
class QVBoxLayout;
class QToolBar;
class QToolButton;
class QTabWidget;
class QTableView;
class QSqlTableModel;
class QLabel;
class QLineEdit;
class QComboBox;
class QHBoxLayout;
class QGridLayout;
class QStatusBar;
class QTimer;
class QDragEnterEvent;
class QDropEvent;

#ifdef he_use_log
#include "hloghelper.h"
#endif // he_use_log



//----------------------------------------
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();


///---------------------------------------------------------------------

private:
    // 初始化创建对话框
    void InitUI();

    // 创建菜单
    void InitMenuBar();

    // 创建工具栏
    void InitToolBar();

    // 创建tableview
    void InitTableView();

    // 指定数据库的驱动
    void InitSqliteDatabase();

    // 创建sqltableModel
    void InitSqlTableModel();

    // 初始化对话框属性
    void InitOther();

    // 创建床体上 的tabwidget
    void InitTabWidget();

    // 读取数据库内容
    void ReadDbData();

    // 添加最后一个外设tab
    void InsertDevTab();

    // 添加下拉项
    void ComboInsertItem(int whichTabIsSelected);

    // 添加外设窗体
    void InitDevDialog();

    // 创建状态栏
    void InitStatusBar();

    // 设置工具栏位置上的控件的状态
    // isEnable ：是否启用， true - 启用， false = 禁用
    void SetToolBarPosCtrlIsEnable(bool isEnable = true);


public slots:

    // 菜单槽函数
    //---------------------------------------------------------------
    // open slot
    void TopMenuStartSlotOpen();


    // set slot
    void TopMenuStartSlotSetting();

    // exit slot
    void TopMenuStartSlotExit();

    // help slot
    void TopMenuHelpSlotAbout();

    // tr("top") << tr("up") << tr("down")  << tr("last") << tr("delete") << tr("add") << tr("submit");
    // 工具栏按钮响应槽函数
    //---------------------------------------------------------------
    // 最前
    void TopToolBtnSlotTop();

    // 上一个
    void TopToolBtnSlotUp();

    // 下一个
    void TopToolBtnSlotDown();

    // 最后
    void TopToolBtnSlotLast();

    // 删除
    void TopToolBtnSlotDelete();

    // 添加
    void TopToolBtnSlotAdd();

    // 提交
    void TopToolBtnSlotSubmit();

    // 哪一列查询变化槽函数
    void ComboQueryCurTextChangedSlot(const QString &text);

    // 查找输入框内容变化了
    void LineEditQueryTextChangedSlot(const QString &text);

    // 选择哪一列排序
    void ComboSortCurTextChangedSlot(const QString &text);

    // tab哪一个标签被激活了
    void TabWidgetWhichTabIsDown( int index);

    // 定时器槽函数
    void TimeOutUpdateSlot();


public:
    // 退出窗体前，
    /*
     * 返回值
     * 0 - 正常退出
     * X > 0 - 表示不退出程序
    */
    int PreDestroyDialog();


private:
    // 更新状态栏
    void UpdateStatusBarData();

    // 根据条件查找显示数据
    // columnNameStr: 从哪一列查找
    // keyStr:模糊查找的条件是什么
    void ShowDataOfQuery(const QString columnNameStr, const QString keyStr);

    // 打开数据库，读取数据库文件
    // filePath： 数据库文件路径
    void PreOpenFile();

    void OpenFile(QString fileName);

    // 添加外设窗体
    void AddDevDialog();


    // 移除主窗体上的tab页
    void ClearTab();

    // 保存添加外设标签
    void SetAddDevTabFlag(bool isAdd);

    // 获取是否添加外设标签
    bool GetAddDevTabFlag();

    // 保存当前选择的数据库文件
    void SetDatabaseFilePath(QString newPath);

    // 取当前选择打开的数据库文件
    QString GetatabaseFilePath();


protected:
    // 重写关闭消息
    void closeEvent(QCloseEvent *e);

    // 按键
    void keyPressEvent(QKeyEvent *e);

    // 重绘
    void paintEvent( QPaintEvent* e);


protected:
    // 为了支持拖拽打开文件。重写下面的函数
    // -----------------------------------------
    //拖动进入事件
    void dragEnterEvent(QDragEnterEvent*event);
    void dropEvent(QDropEvent*event);




private:
    Ui::Dialog *ui;

    /// 菜单
    ///----------------------------------------------------------------
    QMenuBar       *topMenuBar;


    // 1、菜单栏， 开始、和帮助
    QMenu           *topMenuStart;
    QMenu           *topMenuHelp;

    // 2、菜单按钮
    // 打开和退出
    QAction         *topMenuActionStart[he_menu_start_action_3];
    QAction         *topMenuActionHelp[he_menu_help_action_1];

    ///----------------------------------------------------------------


    ///----------------------------------------------------------------

    // 3、工具栏
    QToolBar        *topToolBar;
    // 最前、上一个、下一个、最后、添加、删除、提交
    QToolButton     *topToolBtnArr[he_tool_bar_btn_7];

    // 4、tab 主窗体
    QTabWidget      *tabWidget;

    // 5、暂定一个10个tableview
   QTableView      *tableViewArr[he_tableview_count_10];

    /// 垂直布局
    ///----------------------------------------------------------------
    QVBoxLayout     *vLayoutMain;

    // 配置文件所在路径
    QString         configFilePath;

    // 数据库连接名
    QString         databaseConnectionName;

    // 数据库连接
    QSqlDatabase    sqliteDatabase;

    // 保存数据库中的表名
    QStringList     realTableName;


    QSqlTableModel  *sqlTableModel[he_tableview_count_10];

    //------------------------------------------------------------------

    // 工具栏查找 和 排序

    // 查找combo
    QComboBox       *comboQeury;

    // 查找输入框
    QLineEdit       *lineEditQuery;

    // 查找button

    // 排序label
    QLabel          *labelSort;

    // 排序combo
    QComboBox       *comboSort;

    // 工具栏水平布局
    QHBoxLayout     *topToolWidgetHLayout;

    // 2018年6月24日 23:12:18 主界面改用 栅格布局
    QGridLayout     *gridLayout;

    // 当前选择的tab下的表的列名
    QStringList     curTableViewColumnNameList;


    // 最后一个tab， 外设窗体
    DevDialog       *devDialog;

    // 是否已经添加了外设窗体 , false -没有， true - 添加过了
    bool            devDialogHasAdded;

    //----------------------------------------
    //
    // 状态栏
    QStatusBar      *statusBar;

    // 显示时间
    QLabel          *labelShowTime;

    // 定时器
    QTimer          *timer;



#ifdef he_use_log
    HLogHelper      m_Log;
#endif//he_use_log
};

#endif // DIALOG_H











