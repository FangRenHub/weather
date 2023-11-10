#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //无边框透明
    setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    //子窗阴影
    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    ui->ChildWidget->setGraphicsEffect(shadow_effect);

    ui->cityEdit->setPlaceholderText("查找城市");

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

    //把label装入数组
    weekList << ui->weekLab0 << ui->weekLab1 << ui->weekLab2 << ui->weekLab3 << ui->weekLab4 << ui->weekLab5 << ui->weekLab6;
    typeList << ui->typeLab0 << ui->typeLab1 << ui->typeLab2 << ui->typeLab3 << ui->typeLab4 << ui->typeLab5 << ui->typeLab6;
    typeIconList << ui->foreIcon0 << ui->foreIcon1 << ui->foreIcon2 << ui->foreIcon3 << ui->foreIcon4 << ui->foreIcon5 << ui->foreIcon6;

    //装入icon地址
    typeMap.insert("暴雪",":/type2/xue.png");
    typeMap.insert("暴雨",":/type2/dayu.png");
    typeMap.insert("暴雨到大暴雨",":/type2/baoyu.png");
    typeMap.insert("大暴雨",":/type2/baoyu.png");
    typeMap.insert("大暴雨到特大暴雨",":/type2/baoyu.png");
    typeMap.insert("大到暴雪",":/type2/xue.png");
    typeMap.insert("大雪",":/type2/xue.png");
    typeMap.insert("大雨",":/type2/zhongyu.png");
    typeMap.insert("冻雨",":/type2/xue.png");
    typeMap.insert("多云",":/type2/duoyun.png");
    typeMap.insert("浮尘",":/type2/duoyun.png");
    typeMap.insert("雷阵雨",":/type2/lzy.png");
    typeMap.insert("雷阵雨伴有冰雹",":/type2/lzy.png");
    typeMap.insert("霾",":/type2/feng.png");
    typeMap.insert("强沙尘暴",":/type2/feng.png");
    typeMap.insert("晴",":/type2/qing.png");
    typeMap.insert("沙尘暴",":/type2/feng.png");
    typeMap.insert("特大暴雨",":/type2/baoyu.png");
    typeMap.insert("雾",":/type2/feng.png");
    typeMap.insert("小到中雪",":/type2/xue.png");
    typeMap.insert("小到中雨",":/type2/zhongyu.png");
    typeMap.insert("小雪",":/type2/xue.png");
    typeMap.insert("小雨",":/type2/xiaoyu.png");
    typeMap.insert("雪",":/type2/xue.png");
    typeMap.insert("扬沙",":/type2/feng.png");
    typeMap.insert("阴",":/type2/chaoduoyun.png");
    typeMap.insert("雨",":/type2/xiaoyu.png");
    typeMap.insert("雨夹雪",":/type2/xiaoyu.png");
    typeMap.insert("阵雪",":/type2/xue.png");
    typeMap.insert("阵雨",":/type2/xiaoyu.png");
    typeMap.insert("小到大雨",":/type2/dayu.png");
    typeMap.insert("小到大雪",":/type2/xue.png");
    typeMap.insert("中雪",":/type2/xue.png");
    typeMap.insert("中雨",":/type2/zhongyu.png");

    //创建网络通道管理者
    myNetAccessManager = new QNetworkAccessManager(this);
    connect(myNetAccessManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplied);
    //getWeatherInfo("101010100"); //101010100北京编码
    getWeatherInfo(WeatherTools::getCityCode("三亚"));
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
    if(cityCode.isEmpty()){
        QMessageBox::warning(this,"获取失败","请检查输入城市是否正确", QMessageBox::Ok);
        return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    myNetAccessManager->get(QNetworkRequest(url));
}

void MainWindow::parseJson(QByteArray &jsonByteArray)
{
    this->setCursor(Qt::WaitCursor);
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
    for(int i = 1; i <= 7; i++){
        QJsonObject objForecast = forecastArr[i - 1].toObject();

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
        mForecast[i].fl = objForecast.value("fl").toString();

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
    updateUi();
}

void MainWindow::updateUi()
{
    QString iconStr;
    ui->cityLab->setText(mToday.city);
    iconStr = "image: url(" + typeMap[mToday.type] + ");";
    ui->todIcon->setStyleSheet(iconStr);
    ui->dateLab->setText(QDateTime::fromString(mToday.date, "yyyyMMdd").toString("yyyy/MM/dd") + " " + mForecast[1].week);
    ui->wenduLab->setText(QString::number(mToday.wendu) + "°");
    ui->mainTypeLab->setText(mToday.type + " " + QString::number(mToday.low) + "°~" + QString::number(mToday.high) + "°");
    ui->windLab->setText(mToday.fx + ":" + mToday.fl);
    ui->pmLab->setText("  PM2.5:" + QString::number(mToday.pm25) + "  ");
    ui->shiLab->setText("湿度:" + mToday.shidu);
    ui->qualLab->setText("空气质量:" + mToday.quality);
    ui->ganLab->setText("建议:" + mToday.ganmao);

    //更新forecast
    ui->weekLab0->setText("昨天\n" + mForecast[0].date.right(5));
    ui->weekLab1->setText("今天\n" + mForecast[1].date.right(5));
    ui->weekLab2->setText("明天\n" + mForecast[2].date.right(5));
    for(int i = 0; i < 7; i++){
        if(i > 2) weekList[i]->setText("周" + mForecast[i].week.right(1) + "\n" + mForecast[i].date.right(5));
        typeList[i]->setText(mForecast[i].type);
        iconStr = "image: url(" + typeMap[mForecast[i].type] + ");";
        typeIconList[i]->setStyleSheet(iconStr + "\nbackground-color: rgba(0, 0, 0, 0);");
    }
    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    on_searchBtn_clicked();
}

//查询按钮
void MainWindow::on_searchBtn_clicked()
{
    QString seaCity = ui->cityEdit->text();
    if(ui->cityEdit->text().isEmpty()) return;
    getWeatherInfo(WeatherTools::getCityCode(seaCity));
}
