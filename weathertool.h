#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include <QString>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class WeatherTool{
private:
    static QMap<QString,QString>cityMap;

    static void initCityMap(){

        //QDebug()<<"filePath="<<filePath;
        QFile file("D:\qt_demo\qt_sample\weather\citycode.json");
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

    }
public:
    static QString getCityCode(QString cityName){
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

};
QMap<QString,QString> WeatherTool::cityMap={};
#endif // WEATHERTOOL_H
