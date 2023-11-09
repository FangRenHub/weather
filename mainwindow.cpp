#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    ui->ChildWidget->setGraphicsEffect(shadow_effect);

    ui->cityEdit->setPlaceholderText("查找城市");
    //天气图标
//    QPixmap waetMap(":/type2/riluo.png");
//    ui->weatLabel->setAlignment(Qt::AlignCenter);
//    ui->weatLabel->setPixmap(waetMap.scaled(ui->weatLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    //右键菜单
    rightMenu = new QMenu(this);
    exitAct = new QAction("退出",this);
    exitAct->setIcon(QIcon(":/close.png"));
    rightMenu->addAction(exitAct);

    //右键菜单退出
    connect(exitAct, &QAction::triggered, [=](){
       qDebug() << "exit";
       qApp->exit();
    });

    //
    myNetAccessManager = new QNetworkAccessManager(this);
    connect(myNetAccessManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplied);

    getWeatherInfo("101010100");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onReplied(QNetworkReply *reply)
{
    qDebug() << "onReplied successed";
    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "operation:" << reply->operation();
    qDebug() << "status code:" << status_code;
    qDebug() << "url:" << reply->url();
    qDebug() << "raw header:" << reply->rawHeaderList();

    if(reply->error() != QNetworkReply::NoError || status_code != 200){
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this, "天气请求", "天气请求失败", QMessageBox::Ok);
    }else{
        QByteArray byteArr = reply->readAll();
        qDebug() << "read all:" << byteArr.data();
    }

    reply->deleteLater();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << "rightMenu";
    rightMenu->exec(QCursor::pos());
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

void MainWindow::getWeatherInfo(QString cityCode)
{
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    myNetAccessManager->get(QNetworkRequest(url));
}

