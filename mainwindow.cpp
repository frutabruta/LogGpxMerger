#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDomDocument>
#include <QFileDialog>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_gpxPathSelect_clicked()
{
    ui->lineEdit_gpxPathSelect->setText(openXmlSelectDialogue(ui->lineEdit_gpxPathSelect->text()));


}



QString MainWindow::openXmlSelectDialogue(QString cesta)
{
    qDebug() <<  Q_FUNC_INFO;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Otevři soubor"), cesta,
                                                    tr("Záznam trasy (*.gpx);;All Files (*)"));
    return fileName;
}

void MainWindow::on_pushButton_zpracovat_clicked()
{
    fillCoordinatesList();

    popUpMessage("GPX positions parsed");

    /*if(xmlko.firstChild().isElement())
    {
        QVector<GpxObject> data=parseGpx.parseXml(xmlko);
    }*/
}

void MainWindow::fillCoordinatesList()
{
    QDomDocument xmlko=parseGpx.openFile(ui->lineEdit_gpxPathSelect->text());
    coordinatesList=parseGpx.parseXml(xmlko);
}

QString MainWindow::openDbSelectDialogue(QString cesta)
{
    qDebug() <<  Q_FUNC_INFO;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Otevři db"), cesta,
                                                    tr("Databáze (*.sqlite);;All Files (*)"));
    return fileName;
}


QString MainWindow::openXmlJrSelectDialogue(QString cesta)
{
    qDebug() <<  Q_FUNC_INFO;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Otevři JŘ"), cesta,
                                                    tr("XML JŘ (*.xml);;All Files (*)"));
    return fileName;
}

QString MainWindow::openGeoJsonSelectDialogue(QString cesta)
{
    qDebug() <<  Q_FUNC_INFO;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Otevři Json"), cesta,
                                                    tr("GeoJSON (*.geojson);;All Files (*)"));
    return fileName;
}




void MainWindow::on_pushButton_dbPathSelect_clicked()
{
    ui->lineEdit_dbPathSelect->setText(openDbSelectDialogue(ui->lineEdit_dbPathSelect->text()));
}




void MainWindow::on_pushButton_findCoordinates_clicked()
{
    fillCoordinatesList();

    QDateTime selectedTime=QDateTime::fromString(ui->lineEdit_selectedTime->text(),Qt::ISODateWithMs);

    GpxObject objBefore;
    GpxObject objAfter;

    if(findBeforeAfter(selectedTime,objBefore,objAfter))
    {
        qDebug()<<"casy before:"<<objBefore.time<<" obj after:"<<objAfter.time;

        GpxObject interpolated=createAverageGpxObject(selectedTime,objBefore,objAfter);



        ui->label_coordinatesResult->setText("lat: "+QString::number(interpolated.lat)+" lon:  "+QString::number(interpolated.lon));

        qDebug()<<"lat: "<<interpolated.lat<<" lon:  "<<interpolated.lon;
    }
    else
    {
        popUpMessage("not found");
        qDebug()<<"looking failed";
        ui->label_coordinatesResult->setText("not found");
    }



}

GpxObject MainWindow::createAverageGpxObject(QDateTime &dateToFind, GpxObject &objBefore, GpxObject &objAfter)
{
    GpxObject output;

    /*
    output.ele=(objBefore.ele+objAfter.ele)/2;
    output.lat=(objAfter.lat+objBefore.lat)/2;
    output.lon=(objAfter.lon+objBefore.lon)/2;
    */

    float deltaBeforeAfter=objBefore.time.secsTo(objAfter.time);
    float deltaBeforeSelected=objBefore.time.secsTo(dateToFind);
    double coeficient=deltaBeforeSelected/deltaBeforeAfter;

    double deltaLat=objAfter.lat-objBefore.lat;
    double deltaLng=objAfter.lon-objBefore.lon;
    float deltaEle=objAfter.ele-objAfter.ele;

    output.lat=objBefore.lat+deltaLat*coeficient;
    output.lon=objBefore.lon+deltaLng*coeficient;
    output.ele=objBefore.ele+deltaEle*coeficient;

    return output;
}

bool MainWindow::findBeforeAfter(QDateTime &dateToFind, GpxObject &objBefore, GpxObject &objAfter)
{
    bool selected=false;

    if(coordinatesList.isEmpty())
    {
        qDebug()<<"Empty list!";
        return false;
    }


    int iterator=0;
    while(!selected)
    {
        if(iterator>=coordinatesList.count())
        {
            qDebug()<<"Time "<<dateToFind.toLocalTime()<<" is after specified range "<<coordinatesList.value(0).time.toLocalTime()<<" "<<coordinatesList.value(coordinatesList.count()-1).time.toLocalTime();
            return false;
        }
        else
        {
            QDateTime iteratedTime=coordinatesList.value(iterator).time;
            //qDebug()<<"selected time "<<dateToFind<<" iterated time "<<iteratedTime.toLocalTime();

            if(dateToFind<=iteratedTime)
            {

                if(iterator<1)
                {
                    qDebug()<<"Time is before specified range!";
                    return false;
                }
                else
                {
                    objBefore=coordinatesList.value(iterator-1);
                    objAfter=coordinatesList.value(iterator);
                }

                selected=true;
            }
            else
            {
                iterator++;
                //qDebug()<<"iterator: "<<iterator;
            }
        }


    }


    return true;
}


void MainWindow::findDbEntries()
{
    qDebug()<< Q_FUNC_INFO;
    fillCoordinatesList();

    sqliteBase.dbFilePath=ui->lineEdit_dbPathSelect->text();

    sqliteBase.initialize();

    QString queryString= R"(
        SELECT timestamp, vehicleState,CurrentStopIndex FROM messages;
    )";

    /*  QSqlQuery query =  prepareAndExec(queryString, {
                                                      {":trip_line_c", trip.line.c},
                                                      {":trip_idRopid",trip.idRopid},
                                                      {":kj",kj}
                                                  }); */

    QString insertQueryString= R"(
    UPDATE messages
    SET lat = :lat, lng = :lng
    WHERE timestamp=:timestamp
    )";

    QSqlQuery query =  sqliteBase.prepareAndExec(queryString);

    //  qDebug()<<queryString;


    while (query.next())
    {
        if (query.value(0).toString()!="")
        {
            QString timeStampString=query.value(query.record().indexOf("timestamp")).toString();
            QDateTime timeStamp=QDateTime::fromString(timeStampString,Qt::ISODateWithMs);
            QString vehicleState=query.value(query.record().indexOf("vehicleState")).toString();
            qDebug()<<timeStamp<<" "<<vehicleState ;

            GpxObject objBefore;
            GpxObject objAfter;

            if(findBeforeAfter(timeStamp,objBefore,objAfter))
            {
                GpxObject interpolated=createAverageGpxObject(timeStamp,objBefore,objAfter);

                sqliteBase.prepareAndExec(insertQueryString,{
                                                                 {":timestamp", timeStampString},
                                                                 {":lat",interpolated.lat},
                                                                 {":lng",interpolated.lon}
                                                             });

            }
            else
            {
                qDebug()<<"interpolation failed for "<<timeStampString;
            }
        }
    }
}








void MainWindow::on_pushButton_dbStartQuery_clicked()
{
    findDbEntries();
    popUpMessage("positions inserted to DB");
}


void MainWindow::on_pushButton_geoJson_start_clicked()
{
    XmlJrToGeoJson xmlJrToGeoJson;
    xmlJrToGeoJson.convertXmlToGeoJson(ui->lineEdit_jrXmlPathSelect->text(),ui->lineEdit_geoJsonPathSelect->text());
    popUpMessage("export to GeoJSON done");

}


void MainWindow::on_pushButton_selectJrXml_clicked()
{
    ui->lineEdit_jrXmlPathSelect->setText(openXmlJrSelectDialogue(ui->lineEdit_jrXmlPathSelect->text()));
}


void MainWindow::on_pushButton_selectGeoJSON_clicked()
{
    ui->lineEdit_geoJsonPathSelect->setText(openGeoJsonSelectDialogue(ui->lineEdit_geoJsonPathSelect->text()));
}



void MainWindow::popUpMessage(QString messageText)
{

    QMessageBox msgBox;
    msgBox.setText(messageText);
    msgBox.exec();


    statusBar()->showMessage(
        tr("Data imported successfully."),
        5000); // 5 seconds
}
