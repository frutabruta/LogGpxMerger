#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parsegpx.h"

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

private:
    Ui::MainWindow *ui;
    ParseGpx parseGpx;

    QVector<GpxObject> coordinatesList;
    QString openXmlSelectDialogue(QString cesta);
    bool findBeforeAfter(QDateTime &dateToFind, GpxObject &objBefore, GpxObject &objAfter);
    GpxObject createAverageGpxObject(QDateTime &dateToFind, GpxObject &objBefore, GpxObject &objAfter);
};
#endif // MAINWINDOW_H
