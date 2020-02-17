#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parameters.h"
#include <QPainter>
#include <QPointF>
#include <QPen>
#include <QtGlobal>
#include <QTime>
#include <QTimer>
#include <stdio.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void TimerTick();

private:
    QTimer *fTimer;//计时器
    void InitParam();
    void DrawPoint(int x,int y,const QColor &color);
    void DrawPeople();
    void PeopleFlow();
    void PeopleInfect();
    void PeopleEvolute();
    void HospitalHeal();
    void CheckRate();
    void DrawCurve();
    void DrawString();
    Ui::MainWindow *ui;

protected:
    void paintEvent(QPaintEvent *);
};

#endif // MAINWINDOW_H
