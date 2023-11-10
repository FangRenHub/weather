#include "weathertools.h"

QMap<QString,QString> WeatherTools::cityMap = {};
void WeatherTools::initCityMap()
{
    QString filePath = ":/citycode.json";

    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray json = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json, &err);

    if(err.error != QJsonParseError::NoError) return;
    if(!doc.isArray()) return;

    QJsonArray cities = doc.array();

    for(int i = 0; i < cities.size(); i++){
        QString city = cities[i].toObject().value("city_name").toString();
        QString code = cities[i].toObject().value("city_code").toString();
        if(!code.isEmpty()){
            cityMap.insert(city, code);
        }
    }
}

QString WeatherTools::getCityCode(QString cityName)
{
    if(cityMap.isEmpty()){
        initCityMap();
    }
    if(cityMap.find(cityName) != cityMap.end()){
        return cityMap[cityName];
    }
    else if(cityMap.find(cityName.chopped(1)) != cityMap.end()){
        return cityMap[cityName.chopped(1)];
    }
    else if(cityMap.find(cityName + "市") != cityMap.end()){
        return cityMap[cityName + "市"];
    }
    else if(cityMap.find(cityName + "县") != cityMap.end()){
        return cityMap[cityName + "县"];
    }
    return "";
}
