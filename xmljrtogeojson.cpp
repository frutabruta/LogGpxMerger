#include "xmljrtogeojson.h"

XmlJrToGeoJson::XmlJrToGeoJson() {}



void XmlJrToGeoJson::convertXmlToGeoJson(QString xmlPath, QString jsonPath)

{
    qDebug()<<Q_FUNC_INFO;
    QFile xmlFile(xmlPath);
    QFile jsonFile(jsonPath);

    QDomDocument xmlDocument;



    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        emit signalError("announcementList file could not be loaded");
        qDebug()<<"fail1";
        xmlFile.close();
        return;
    }


    QString errorStr="";
    int errorLine=0;
    int errorColumn=0;


    if(!xmlDocument.setContent(&xmlFile, false, &errorStr, &errorLine, &errorColumn))
    {

        emit signalError("announcement list file is invalid:"+errorStr+" at line "+QString::number(errorLine)+" column "+QString::number(errorColumn ));

        xmlFile.close();
        return;
    }

    xmlToJson(xmlDocument,jsonPath);

}


int XmlJrToGeoJson::xmlToJson(QDomDocument xmlko, QString jsonPath)
{
    qDebug()<<Q_FUNC_INFO;

    QDomNodeList domZElementList=xmlko.elementsByTagName("z");
    QJsonArray features;


    if(domZElementList.isEmpty())
    {
        return 0;
    }

    for(int i=0; i<domZElementList.count(); i++)
    {
        QDomElement element=domZElementList.at(i).toElement();

        QJsonObject properties;
        properties["cis"]=element.attribute("cis");
        properties["u"]=element.attribute("u");
        properties["n"]=element.attribute("n");
        properties["rdisp"]=element.attribute("rdisp");
        properties["shape"]="polout";


        QJsonObject geometryRadius;
        geometryRadius["type"]="Point";

        // bod
        QJsonArray coordinates;
        double lat=element.attribute("lat").toDouble();
        double lng=element.attribute("lng").toDouble();
        geometryRadius["coordinates"] = QJsonArray{lng, lat};


        //polygon
        QJsonObject geometryPol;
        geometryPol["type"]="Polygon";
        QJsonArray polygonPoints=parsePolygonCoordinates(element,"pol");
        geometryPol["coordinates"]=QJsonArray{polygonPoints};


        //polygon out
        QJsonObject geometryPolOut;
        geometryPolOut["type"]="Polygon";
        QJsonArray polygonOutPoints=parsePolygonCoordinates(element,"polout");
        geometryPolOut["coordinates"]=QJsonArray{polygonOutPoints};


        // all
        QJsonObject featureRadius;
        featureRadius["type"]="Feature";
        featureRadius["properties"]=properties;
        featureRadius["geometry"]=geometryRadius;

        features.append(featureRadius);


        if(!polygonPoints.isEmpty())
        {
            QJsonObject featurePol=featureRadius;
            QJsonObject propertiesPol=properties;
            propertiesPol["shape"]="pol";
            featurePol["geometry"]=geometryPol;
            featurePol["properties"]=propertiesPol;
            features.append(featurePol);
        }

        if(!polygonOutPoints.isEmpty())
        {
            QJsonObject featurePolOut=featureRadius;
            QJsonObject propertiesPolout=properties;
            propertiesPolout["shape"]="polout";
            featurePolOut["geometry"]=geometryPolOut;
            featurePolOut["properties"]=propertiesPolout;
            features.append(featurePolOut);
        }
    }


    QJsonObject root;
    root["type"] = "FeatureCollection";
    root["features"] = features;

    QJsonDocument jsonDocument(root);


    qDebug().noquote()<<jsonDocument;

    QFile file(jsonPath);

    if (file.open(QIODevice::WriteOnly))
    {
            file.write(jsonDocument.toJson(QJsonDocument::Indented));
    }

    return 1;
}


QJsonArray  XmlJrToGeoJson::parsePolygonCoordinates(QDomElement parentElement, QString elementName)
{
    qDebug()<<Q_FUNC_INFO<<" element name "<<elementName;
    QJsonObject geometryPol;
    geometryPol["type"]="Polygon";
    QDomNodeList polBodList=parentElement.firstChildElement(elementName).elementsByTagName("bod");
    QJsonArray polygonPoints;

    for(int j=0;j<polBodList.count();j++)
    {
        QDomElement selectedPoint=polBodList.at(j).toElement();
        double latPol=selectedPoint.attribute("lat").toDouble();
        double lngPol=selectedPoint.attribute("lon").toDouble();
        polygonPoints.append(QJsonArray{lngPol, latPol});

    }
    if(!polygonPoints.isEmpty())
    {
        polygonPoints.append(polygonPoints.first());
    }

    geometryPol["coordinates"]=QJsonArray{polygonPoints};


    return polygonPoints;


}


