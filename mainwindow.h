#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
class QMouseEvent;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "weatherdata.h"
class QLabel;

const int increment = 3;
const int pointRadius = 3;
const int textOffsetX = 12;
const int textOffsetY = 12;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMenu *menuList;
    QAction *exitAct;
    QPoint mOffset;
    bool mMoving;
    QNetworkAccessManager *mNetworkAccessManager;
    ToDay mToDay;
    Day mDay[6];

    QList<QLabel*> mWeekList;
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;
    QList<QLabel*> mAqiList;

    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;
    QMap<QString,QString> mTypeMap;
    QMap<QString,QString> cityMap;
    void onReplied(QNetworkReply *reply);
    void initCityMap();
    QString getCityCode(QString cityName);
    // QWidget interface
protected:
    virtual void contextMenuEvent(QContextMenuEvent *event) override;

private slots:


    // QWidget interface
    void on_btnSearch_clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    void getWeatherInfo(QString cityCode);
    void parseJson(QByteArray &byteArray);
    void updateUI();

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    //绘制高低温曲线
    void paintHighCurve();
    void paintLowCurve();
};





#endif // MAINWINDOW_H
