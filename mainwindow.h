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

private slots:
    void onReplied(QNetworkReply *reply);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void getWeatherInfo(QString cityCode);

    QNetworkAccessManager *myNetAccessManager;

};
#endif // MAINWINDOW_H
