#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//设置窗口无边框
    setFixedSize(width(),height());//设置固定窗口大小

    menuList=new QMenu(this);
    exitAct=new QAction();
    exitAct->setText("退出");
    exitAct->setIcon(QIcon(":/icons/res/close.png"));
    menuList->addAction(exitAct);
    connect(exitAct,&QAction::triggered,this,[=]{
        close();
    });


    mWeekList<<ui->labDate0<<ui->labDate1<<ui->labDate2<<ui->labDate3<<ui->labDate4<<ui->labDate5;
    mTypeList<<ui->labType0<<ui->labType1<<ui->labType2<<ui->labType3<<ui->labType4<<ui->labType5;
    mTypeIconList<<ui->labTypeIcon0<<ui->labTypeIcon1<<ui->labTypeIcon2<<ui->labTypeIcon3<<ui->labTypeIcon4<<ui->labTypeIcon5;
    mAqiList<<ui->lblQuality0<<ui->lblQuality1<<ui->lblQuality2<<ui->lblQuality3<<ui->lblQuality4<<ui->lblQuality5;
    mFlList<<ui->lblfx0<<ui->lblfx1<<ui->lblfx2<<ui->lblfx3<<ui->lblfx4<<ui->lblfx5;
    mFxList<<ui->lblFl0<<ui->lblFl1<<ui->lblFl2<<ui->lblFl3<<ui->lblFl4<<ui->lblFl5;
    mTypeMap.insert("暴雪",":/icons/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/icons/res/type/BaoYu.png");
    mTypeMap.insert("暴雨到大暴雨",":/icons/res/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/icons/res/type/DaBaoXue.png");
    mTypeMap.insert("大到暴雪",":/icons/res/type/DaDaoBaoXue.png");
    mTypeMap.insert("大雪",":/icons/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/icons/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/icons/res/type/DongYu.png");
    mTypeMap.insert("多云",":/icons/res/type/DuoYun.png");
    mTypeMap.insert("浮沉",":/icons/res/type/FuChen.png");
     mTypeMap.insert("雷阵雨",":/icons/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/icons/res/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/icons/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/icons/res/type/QianShaChenBao.png");
    mTypeMap.insert("晴",":/icons/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/icons/res/type/ShaChengBao.png");
    mTypeMap.insert("特大暴雨",":/icons/res/type/TeDaBaoXue.png");
    mTypeMap.insert("undefined",":/icons/res/type/undefined.png");
    mTypeMap.insert("雾",":/icons/res/type/Wu.png");
    mTypeMap.insert("小到中雪",":/icons/res/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小到中雨",":/icons/res/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小雪",":/icons/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/icons/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/icons/res/type/Xue.png");
    mTypeMap.insert("扬沙",":/icons/res/type/YangSha.png");
    mTypeMap.insert("阴",":/icons/res/type/Yin.png");
    mTypeMap.insert("雨",":/icons/res/type/Yu.png");
    mTypeMap.insert("雨夹雪",":/icons/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雪",":/icons/res/type/ZhenXue.png");
    mTypeMap.insert("阵雨",":/icons/res/type/ZhenYu.png");
    mTypeMap.insert("中到大雪",":/icons/res/type/ZhongDaoDaXue.png");
    mTypeMap.insert("中到大雨",":/icons/res/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪",":/icons/res/type/ZhongXue.png");
    mTypeMap.insert("中雨",":/icons/res/type/ZhongYu.png");



    //WeatherTool::filePath=QCoreApplication::applicationDirPath()+"/citycode.json";
    //QDebug()<<"filePath="<<filePath;
    mNetworkAccessManager=new QNetworkAccessManager(this);
    //关联请求数据信号
    connect(mNetworkAccessManager,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);
    connect(ui->editCity, &QLineEdit::returnPressed, [=]() {
        ui->btnSearch->clicked(true);
    });
    //获取北京天气数据
    getWeatherInfo("101010100");
    //添加事件过滤器
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//右键菜单
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    menuList->exec(QCursor::pos());//在鼠标 位置 弹出
    event->accept();
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{  if(event->button()==Qt::LeftButton){//鼠标左键按下
        mMoving=true;
        mOffset= event->globalPos()-this->pos();//鼠标与显示器左上角距离- 窗口与显示器左上角距离
    }
    //return MainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{   mMoving=false;
    //return MainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)

{
    QPoint eventPos=event->globalPosition().toPoint();
    if (mMoving && (event->buttons()&Qt::LeftButton)&& (eventPos-mOffset).manhattanLength()>QApplication::startDragDistance()){
        this->move(eventPos-mOffset);
    }

    //return MainWindow::mouseMoveEvent(event);
}

void MainWindow::getWeatherInfo(QString cityCode)
{
    QString url="http://t.weather.itboy.net/api/weather/city/"+cityCode;
    mNetworkAccessManager->get(QNetworkRequest(url));//发起请求
}



void MainWindow::onReplied(QNetworkReply *reply)
{
    //qDebug()<<"onReplied success";
    //响应状态码
    int statusCode= reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug()<<"operstion="<<reply->operation();
    qDebug()<<"statusCode="<<statusCode;
    qDebug()<<"url="<<reply->url();
    qDebug()<<"raw header="<<reply->rawHeaderList();
    if(reply->error()!=QNetworkReply::NoError||statusCode!=200){
        qDebug()<<"error="<<reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Yes,QMessageBox::NoButton);
    }else{
       QByteArray byteArr= reply->readAll();//读取请求数据
        qDebug()<<"byteArr="<<byteArr.data();
       parseJson(byteArr);
    }
    reply->deleteLater();//释放内存
}

void MainWindow::initCityMap()
{
    QString filePath=QCoreApplication::applicationDirPath()+"/citycode.json";
    qDebug()<<"filePath="<<filePath;
    //QDebug()<<"filePath="<<filePath;
    QFile file(filePath);
    //不能读取
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
       return;
    QByteArray array=file.readAll();
    QJsonParseError error;
    QJsonDocument doc=  QJsonDocument::fromJson(array,&error);
    if(error.error!=QJsonParseError::NoError){
       return;
    }
    if(!doc.isArray()){
       return;
    }
    QJsonArray jsonArray= doc.array();
    for(int i=0;i<jsonArray.size();i++){
       QJsonObject obj=jsonArray[i].toObject();
       QString city=obj.value("city_name").toString();
       QString code=obj.value("city_code").toString();
       if(code.length()>0){
           cityMap.insert(city,code);
       }
    }
    qDebug()<<"cityMap size="<<cityMap.size();
}

QString MainWindow::getCityCode(QString cityName)
{
        if(cityMap.isEmpty()){
            initCityMap();

        }
        QMap<QString,QString>::iterator it=cityMap.find(cityName);

        if(it==cityMap.end()){
            it=cityMap.find(cityName+"市");
        }
        if(it!=cityMap.end()){
            return it.value();
        }
        return "";
}
void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc=  QJsonDocument::fromJson(byteArray,&err);
    if(err.error!=QJsonParseError::NoError){
       return;
    }
    QJsonObject rootObj= doc.object();
    qDebug()<<rootObj.value("message").toString();

    mToDay.date= rootObj.value("date").toString();
    mToDay.city= rootObj.value("cityInfo").toObject().value("city").toString();

    //yesterday
    QJsonObject dataObj=rootObj.value("data").toObject();

    mToDay.ganmao=dataObj.value("ganmao").toString();
    mToDay.shidu=dataObj.value("shidu").toString();
    mToDay.pm25=dataObj.value("pm25").toDouble();
    mToDay.quality=dataObj.value("quality").toString();
    mToDay.wendu=dataObj.value("wendu").toString().toInt();

    QJsonObject yesterdayObj=dataObj.value("yesterday").toObject();

    mDay[0].week=yesterdayObj.value("week").toString();
    mDay[0].date=yesterdayObj.value("ymd").toString();
    mDay[0].type=yesterdayObj.value("type").toString();

    QString high=yesterdayObj.value("high").toString().split(" ").at(1);
    high=high.left(high.length()-1);
    mDay[0].high=high.toInt();

    QString low=yesterdayObj.value("low").toString().split(" ").at(1);
    low=low.left(low.length()-1);
    mDay[0].low=low.toInt();
    qDebug()<<"high="<<high<<"low="<<low;

    mDay[0].fx=yesterdayObj.value("fx").toString();
    mDay[0].fl=yesterdayObj.value("fl").toString();
    mDay[0].aqi=yesterdayObj.value("aqi").toDouble();

    QJsonArray forecastArray=dataObj.value("forecast").toArray();
    for(int i=0;i<5;i++){
       QJsonObject obj=forecastArray[i].toObject();
       mDay[i+1].week=obj.value("week").toString();
       mDay[i+1].date=obj.value("ymd").toString();
       mDay[i+1].type=obj.value("type").toString();

       QString high=obj.value("high").toString().split(" ").at(1);
       high=high.left(high.length()-1);
       mDay[i+1].high=high.toInt();

       QString low=obj.value("low").toString().split(" ").at(1);
       low=low.left(low.length()-1);
       mDay[i+1].low=low.toInt();

       mDay[i+1].fx=obj.value("fx").toString();
       mDay[i+1].fl=obj.value("fl").toString();
       mDay[i+1].aqi=obj.value("aqi").toDouble();
    }

    mToDay.type=mDay[1].type;
    mToDay.fx=mDay[1].fx;
    mToDay.fl=mDay[1].fl;
    mToDay.high=mDay[1].high;
    mToDay.low=mDay[1].low;
    qDebug()<<"high="<<mToDay.high<<"low="<<mToDay.low;

    updateUI();
    //主动刷新UI
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}

void MainWindow::updateUI()
{
    QString dateStr=QDateTime::fromString(mToDay.date,"yyyyMMdd").toString("yyyy/MM/dd")+" "+mDay[1].week;
    ui->labDate->setText(dateStr);
    ui->lblCity->setText(mToDay.city);

    int w= ui->lblTypeIcon->width();
    QPixmap mPixmap;
    mPixmap.load(mTypeMap[mToDay.type]);
    QPixmap pix= mPixmap.scaledToWidth(w);
    ui->lblTypeIcon->setPixmap(pix);

    ui->lblTemp->setText(QString::number(mToDay.wendu)+"°");
    ui->lblType->setText(mToDay.type);
    ui->lblLowHigh->setText(QString::asprintf("%1d°C ~ %2d°C",mToDay.low,mToDay.high));
    ui->lblGanMao->setText("感冒指数："+mToDay.ganmao);

    ui->lblWindFl->setText(mToDay.fl);
    ui->lblWindFx->setText(mToDay.fx);
    ui->lblPM25->setText(QString::number(mToDay.pm25));
    ui->lblShiDu->setText(mToDay.shidu);
    ui->lblQuality->setText(mToDay.quality);

    QStringList list;
    list<<"昨天"<<"今天"<<"明天";
    for(int i=0;i<6;i++){
       if(i<list.length()){
           QString dates=list[i]+"\n";
           dates+="周"+mDay[i].week.right(1);
           mWeekList[i]->setText(dates);
       }else{
           QString dates=QDateTime::fromString(mDay[i].date,"yyyy-MM-dd").toString("MM/dd")+"\n";
           dates+="周"+mDay[i].week.right(1);
           mWeekList[i]->setText(dates);
       }
       mTypeList[i]->setText(mDay[i].type);
       mFlList[i]->setText(mDay[i].fl);
       mFxList[i]->setText(mDay[i].fx);

       //int w= mTypeIconList[i]->width();
       QPixmap mPixmap;
       mPixmap.load(mTypeMap[mDay[i].type]);
       //QPixmap pix= mPixmap.scaledToWidth(w);
       mTypeIconList[i]->setPixmap(mPixmap);


       int aqi=mDay[i].aqi;
       if(aqi>=0&&aqi<=50){
           mAqiList[i]->setText("优");
           mAqiList[i]->setStyleSheet("background-color:rgb(121,184,0)");
       }else if(aqi>=50&&aqi<=100){
           mAqiList[i]->setText("良");
           mAqiList[i]->setStyleSheet("background-color:rgb(255,187,23)");
       }else if(aqi>=100&&aqi<=150){
           mAqiList[i]->setText("轻度");
           mAqiList[i]->setStyleSheet("background-color:rgb(255,87,97)");
       }else if(aqi>=150&&aqi<=200){
           mAqiList[i]->setText("中度");
           mAqiList[i]->setStyleSheet("background-color:rgb(235,17,27)");
       }else if(aqi>=200&&aqi<=250){
           mAqiList[i]->setText("重度");
           mAqiList[i]->setStyleSheet("background-color:rgb(170,0,0)");
       }else {
           mAqiList[i]->setText("严重");
           mAqiList[i]->setStyleSheet("background-color:rgb(110,0,0)");
       }
    }
}

void MainWindow::on_btnSearch_clicked()
{
    QString city= ui->editCity->text();
    if(city.isEmpty()){
       return;
    }
    QString cityCode=getCityCode(city);
    if(cityCode.length()>0){
       getWeatherInfo(cityCode);
    }
}
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->lblHighCurve&&event->type()==QEvent::Paint){
       paintHighCurve();
    }
    if(watched==ui->lblLowCurve&&event->type()==QEvent::Paint){
       paintLowCurve();
    }
    return QWidget::eventFilter(watched,event);
}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    int pointX[6];
    for(int i=0;i<6;i++){
       pointX[i]=mWeekList[i]->pos().x()+mWeekList[i]->width()/2;
    }
    int temSum=0;
    int tempAverage=0;
    for(int i=0;i<6;i++){
       temSum+=mDay[i].high;
    }
    tempAverage=temSum/6;
    int pointY[6]={0};
    int yCenter=ui->lblHighCurve->height()/2;
    for(int i=0;i<6;i++){
       pointY[i]=yCenter-((mDay[i].high-tempAverage)*increment);
    }
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(255,170,0));
    painter.setPen(pen);
    QBrush  brush;
    brush.setColor(QColor(255,170,0)); //画刷颜色
    brush.setStyle(Qt::SolidPattern); //画刷填充样式
    painter.setBrush(brush);

    for(int i=0;i<6;i++){
       painter.drawEllipse(QPoint(pointX[i],pointY[i]),pointRadius,pointRadius);
       painter.drawText(pointX[i]-textOffsetX,pointY[i]-textOffsetY,QString::number(mDay[i].high)+"°");
    }

    pen.setCapStyle(Qt::FlatCap);       //线端点样式
    pen.setJoinStyle(Qt::BevelJoin);    //线的连接点样式
    for(int i=0;i<5;i++){
       if(i==0){
           pen.setStyle(Qt::DashLine);       //线的类型，实线、虚线等
       }else{
           pen.setStyle(Qt::SolidLine);
       }
       painter.setPen(pen);
       painter.drawLine(pointX[i],pointY[i],pointX[i+1],pointY[i+1]);
    }


}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    int pointX[6];
    for(int i=0;i<6;i++){
       pointX[i]=mWeekList[i]->pos().x()+mWeekList[i]->width()/2;
    }
    int temSum=0;
    int tempAverage=0;
    for(int i=0;i<6;i++){
       temSum+=mDay[i].low;
    }
    tempAverage=temSum/6;
    int pointY[6]={0};
    int yCenter=ui->lblLowCurve->height()/2;
    for(int i=0;i<6;i++){
       pointY[i]=yCenter-((mDay[i].low-tempAverage)*increment);
    }
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0,255,255));
    painter.setPen(pen);
    QBrush  brush;
    brush.setColor(QColor(0,255,255)); //画刷颜色
    brush.setStyle(Qt::SolidPattern); //画刷填充样式
    painter.setBrush(brush);

    for(int i=0;i<6;i++){
       painter.drawEllipse(QPoint(pointX[i],pointY[i]),pointRadius,pointRadius);
       painter.drawText(pointX[i]-textOffsetX,pointY[i]-textOffsetY,QString::number(mDay[i].low)+"°");
    }

    pen.setCapStyle(Qt::FlatCap);       //线端点样式
    pen.setJoinStyle(Qt::BevelJoin);    //线的连接点样式
    for(int i=0;i<5;i++){
       if(i==0){
           pen.setStyle(Qt::DashLine);       //线的类型，实线、虚线等
       }else{
           pen.setStyle(Qt::SolidLine);
       }
       painter.setPen(pen);
       painter.drawLine(pointX[i],pointY[i],pointX[i+1],pointY[i+1]);
    }
}

