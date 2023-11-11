#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QContextMenuEvent>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QPainter>

#include "weatherdate.h"
#include "weathertools.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QMenu *rightMenu;
    QAction *exitAct;

    QPoint mOffset; // 挪动窗口移动时，鼠标的偏移

    QList<QLabel*> weekList;

    QList<QLabel*> typeList;
    QList<QLabel*> typeIconList;

    QMap<QString, QString> typeMap;

private slots:
    void onReplied(QNetworkReply *reply);

    void on_searchBtn_clicked();

    void on_pushButton_clicked();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void getWeatherInfo(QString cityCode);

    QNetworkAccessManager *myNetAccessManager;

    void parseJson(QByteArray& jsonByteArray);

    void updateUi();

    void keyPressEvent(QKeyEvent * event);

    bool eventFilter(QObject *watched, QEvent *event);

    void closeEvent(QCloseEvent *event);

    //绘制高低温曲线
    void paintHighCurve();
    void paintLowCurve();

    Today mToday;
    Forecast mForecast[8];

};
#endif // MAINWINDOW_H
