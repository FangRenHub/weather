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

    //创建网络通道管理者
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
        parseJson(byteArr);
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

void MainWindow::parseJson(QByteArray &jsonByteArray)
{
    QJsonParseError err;
    QJsonDocument jDoc = QJsonDocument::fromJson(jsonByteArray, &err);
    if(err.error != QJsonParseError::NoError){
        return;
    }

    QJsonObject rootObj = jDoc.object();
    QJsonObject objData = rootObj.value("data").toObject();
    qDebug() << "message:" << rootObj.value("message").toString();

    //解析日期，城市
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

    //解析data forcast[0]代表昨天
    QJsonObject objYesterday = objData.value("yesterday").toObject();
    mForecast[0].week = objYesterday.value("week").toString();
    mForecast[0].date = objYesterday.value("ymd").toString();
    mForecast[0].type = objYesterday.value("type").toString();

    QString s;
    s = objYesterday.value("high").toString().split(" ").at(1);
    s.chop(1);
    mForecast[0].high = s.toInt();

    s = objYesterday.value("low").toString().split(" ").at(1);
    s.chop(1);
    mForecast[0].low = s.toInt();

    mForecast[0].fx = objYesterday.value("fx").toString();
    mForecast[0].fx = objYesterday.value("fl").toString();

    mForecast[0].aqi = objYesterday.value("fx").toDouble();

    //解析forecast
    QJsonArray forecastArr = objData.value("forecast").toArray();
    for(int i = 1; i <= 5; i++){
        QJsonObject objForecast = forecastArr[i].toObject();

        mForecast[i].week = objForecast.value("week").toString();
        mForecast[i].date = objForecast.value("ymd").toString();
        mForecast[i].type = objForecast.value("type").toString();

        QString s;
        s = objForecast.value("high").toString().split(" ").at(1);
        s.chop(1);
        mForecast[i].high = s.toInt();

        s = objForecast.value("low").toString().split(" ").at(1);
        s.chop(1);
        mForecast[i].low = s.toInt();

        mForecast[i].fx = objForecast.value("fx").toString();
        mForecast[i].fx = objForecast.value("fl").toString();

        mForecast[i].aqi = objForecast.value("fx").toDouble();

        qDebug() << "forecasting test" << i << ":" << mForecast[i].date;
    }

    //解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();
    mToday.wendu = objData.value("wendu").toString().toInt();
    mToday.shidu = objData.value("shidu").toString();
    mToday.quality = objData.value("quality").toString();
    mToday.pm25 = objData.value("pm25").toDouble();
    //forcast[1]是今天的数据
    mToday.type = mForecast[1].type;
    mToday.fx = mForecast[1].fx;
    mToday.fl = mForecast[1].fl;
    mToday.high = mForecast[1].high;
    mToday.low = mForecast[1].low;
}

