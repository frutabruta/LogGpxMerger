#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parsegpx.h"

#include "XmlRopidImportStream/sqlitebase.h"
#include "xmljrtogeojson.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();        

private slots:
    void on_pushButton_gpxPathSelect_clicked();

    void on_pushButton_zpracovat_clicked();

    void on_pushButton_findCoordinates_clicked();

    void on_pushButton_dbPathSelect_clicked();

    void on_pushButton_dbStartQuery_clicked();

    void on_pushButton_geoJson_start_clicked();

    void on_pushButton_selectJrXml_clicked();

    void on_pushButton_selectGeoJSON_clicked();

private:
    Ui::MainWindow *ui;
    ParseGpx parseGpx;

    QVector<GpxObject> coordinatesList;
    QString openXmlSelectDialogue(QString cesta);
    bool findBeforeAfter(QDateTime &dateToFind, GpxObject &objBefore, GpxObject &objAfter);
    GpxObject createAverageGpxObject(QDateTime &dateToFind, GpxObject &objBefore, GpxObject &objAfter);

    SqLiteBase sqliteBase;

    void findDbEntries();
    QString openDbSelectDialogue(QString cesta);
    QString openXmlJrSelectDialogue(QString cesta);
    QString openGeoJsonSelectDialogue(QString cesta);
};
#endif // MAINWINDOW_H
