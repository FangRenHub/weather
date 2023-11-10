#ifndef WEATHERTOOLS_H
#define WEATHERTOOLS_H

#include <QMap>
#include <QFile>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

class WeatherTools{
private:
    static QMap<QString,QString> cityMap;

    static void initCityMap();
public:
    static QString getCityCode(QString cityName);
};



#endif // WEATHERTOOLS_H
