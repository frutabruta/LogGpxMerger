#include "parsegpx.h"
#include <QtDebug>


ParseGpx::ParseGpx() {}


QDomDocument ParseGpx::openFile(QString path)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomDocument doc;//("mydocument");
    // QFile file("xml_zdroje/XML_Zlicin_20200702_20200705.xml");


    QString kompletCesta=path;
    QFile file(kompletCesta);

    qDebug()<<"opening file "<<kompletCesta;


    if (!file.open(QIODevice::ReadOnly))
    {
        emit signalError("announcementList file could not be loaded");
        qDebug()<<"fail1";
        file.close();
        return doc;
    }
    qDebug()<<"config file size is "<<file.size();

    // https://stackoverflow.com/questions/42117178/qdomdocument-setcontent-return-false
    QString errorStr;
    int errorLine;
    int errorColumn;
    if(!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {

        emit signalError("announcement list file is invalid:"+errorStr+" at line "+QString::number(errorLine)+" column "+QString::number(errorColumn ));

        file.close();
        return doc;
    }

    qDebug()<<"uspech";


    file.close();
    return doc;
}

void ParseGpx::slotDataParsed(QDomDocument doc)
{

}

QVector<GpxObject> ParseGpx::parseXml(QDomDocument doc)
{
    qDebug()<<Q_FUNC_INFO;
    QVector<GpxObject> result;
    QDomElement root=doc.firstChildElement();
    qDebug()<<root.tagName();

    QDomNodeList trkptElements=root.elementsByTagName("trkpt");

    for(int i=0; i<trkptElements.length();i++)
    {
        QDomElement selectedElement=trkptElements.item(i).toElement();
        GpxObject outputElement;
        outputElement.lat=selectedElement.attribute("lat").toDouble();
        outputElement.lon=selectedElement.attribute("lon").toDouble();
        outputElement.ele=selectedElement.firstChildElement("ele").text().toDouble();
        outputElement.time=QDateTime::fromString(selectedElement.firstChildElement("time").text(),Qt::ISODate );

        qDebug()<<QString("time:%0 lat:%1 lon:%2").arg(outputElement.time.toString(Qt::ISODate) ).arg(outputElement.lat).arg(outputElement.lon);
        result<<outputElement;
    }

    return result;
}


