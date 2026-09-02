#ifndef PARSEGPX_H
#define PARSEGPX_H

#include <QObject>
#include <QDomDocument>
#include <QtXml>

class GpxObject
{
public:
    double lat=0.0;
    double lon=0.0;
    QDateTime time;
    double ele=0;

private:
};

class ParseGpx : public QObject

{
      Q_OBJECT
public:
    ParseGpx();
    QDomDocument openFile(QString path);
    QVector<GpxObject> parseXml(QDomDocument doc);
public slots:
    void slotDataParsed(QDomDocument doc);
signals:
    void signalError(QString errorString);
};

#endif // PARSEGPX_H
