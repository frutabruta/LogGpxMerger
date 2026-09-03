#ifndef XMLJRTOGEOJSON_H
#define XMLJRTOGEOJSON_H

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDomDocument>
#include <QFile>
#include <QObject>

class XmlJrToGeoJson : public QObject
{
    Q_OBJECT
public:
    XmlJrToGeoJson();

    void convertXmlToGeoJson(QString xmlPath, QString jsonPath);
    int xmlToJson(QDomDocument xmlko, QString jsonPath);

signals:
    void signalError(QString errorMessage);
private:
    QJsonArray parsePolygonCoordinates(QDomElement parentElement, QString elementName);
};

#endif // XMLJRTOGEOJSON_H
