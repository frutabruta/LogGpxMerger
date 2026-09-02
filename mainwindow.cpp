#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDomDocument>
#include <QFileDialog>

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
    QDomDocument xmlko=parseGpx.openFile(ui->lineEdit_gpxPathSelect->text());
    coordinatesList=parseGpx.parseXml(xmlko);

    /*if(xmlko.firstChild().isElement())
    {
        QVector<GpxObject> data=parseGpx.parseXml(xmlko);
    }*/
}


void MainWindow::on_pushButton_findCoordinates_clicked()
{
    QDateTime selectedTime=QDateTime::fromString(ui->lineEdit_selectedTime->text(),Qt::ISODateWithMs);

    GpxObject objBefore;
    GpxObject objAfter;

    if(findBeforeAfter(selectedTime,objBefore,objAfter))
    {
        qDebug()<<"casy before:"<<objBefore.time<<" obj after:"<<objAfter.time;

        GpxObject interpolated=createAverageGpxObject(selectedTime,objBefore,objAfter);




        qDebug()<<"lat: "<<interpolated.lat<<" lon:  "<<interpolated.lon;
    }
    else
    {
        qDebug()<<"looking failed";
    }



}

GpxObject MainWindow::createAverageGpxObject(QDateTime &dateToFind, GpxObject &objBefore, GpxObject &objAfter)
{
    GpxObject output;
    output.ele=(objBefore.ele+objAfter.ele)/2;
    output.lat=(objAfter.lat+objBefore.lat)/2;
    output.lon=(objAfter.lon+objBefore.lon)/2;
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
            qDebug()<<"Time is after specified range!";
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
            }
        }


    }


    return true;
}



