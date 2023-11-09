#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);
    this->setStyleSheet("QMainWindow{background-color:gray;"
                        "border-radius: 15px;}");

    rightMenu = new QMenu(this);
    exitAct = new QAction("退出",this);
    exitAct->setIcon(QIcon(":/close.png"));
    rightMenu->addAction(exitAct);

    //右键菜单退出
    connect(exitAct, &QAction::triggered, [=]{
       qDebug() << "exit";
       qApp->exit();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << "rightMenu";
    rightMenu->exec(QCursor::pos());
    event->accept();
}

