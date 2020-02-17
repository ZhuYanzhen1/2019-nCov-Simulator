#include "mainwindow.h"
#include "ui_mainwindow.h"
static int died_num = 0;
static int total_infected = 1;
static int infected_num = 1;
static unsigned long round_counter = 0;
static unsigned long curve_counter = 0;
static int last_x = 740;
static int last_y = 400;
static QList<double> xList_die;
static QList<double> yList_die;
static QList<double> xList_infect;
static QList<double> yList_infect;
void MainWindow::InitParam(){
    protect_level = 1.0f;
    flowpurpose = 0.7f;
    virus.prob.young_intensive = 0.2f;
    virus.prob.young_critical = 0.2f;
    virus.prob.young_died = 0.05f;
    virus.prob.mid_intensive = 0.3f;
    virus.prob.mid_critical = 0.2f;
    virus.prob.mid_died = 0.08f;
    virus.prob.old_intensive = 0.5f;
    virus.prob.old_critical = 0.3f;
    virus.prob.old_died = 0.1f;
    virus.mild_heal = 0.1f;
    virus.intensive_heal = 0.08f;
    virus.critical_heal = 0.02f;
    virus.area = 10;
    virus.hide_time = 7;
    hospital.diagnosis_time = 1;
    hospital.vacancy = 100;
    move_speed = 8;
}
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
    fTimer = new QTimer(this);
    int i,j;
    QList<int> numbersList_x,numbersList_y;
    qsrand((unsigned int)QTime(0,0,0).secsTo(QTime::currentTime()));
    for(i=0;i<population;i++){
        numbersList_x.append(rand()%700);
        numbersList_y.append(rand()%700);
    }
    for(i=0;i<population;i++){
        people[i].pos.x = numbersList_y[i];
        people[i].pos.y = numbersList_x[i];
        people[i].infect = 0;       //healthy
        people[i].protect = protect_level;
        people[i].is_admit = false;
        people[i].ill_time = 0;
        people[i].age = qrand()%100;
        people[i].is_new_infect = false;
    }
    people[0].infect = 1;
    InitParam();
    fTimer->stop();
    fTimer->setInterval(100);//设置定时周期，单位：毫秒
    connect(fTimer,SIGNAL(timeout()),this,SLOT(TimerTick()));
    fTimer->start();
}
MainWindow::~MainWindow(){
    delete ui;
}
void MainWindow::DrawCurve(){
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(3);
    painter.setPen(pen);
    painter.fillRect(740, 100, 300, 120, Qt::white);
    painter.fillRect(740, 250, 300, 120, Qt::white);
    painter.drawLine(QPointF(750, 100), QPointF(750, 210));   // y
    painter.drawLine(QPointF(750, 210), QPointF(1040, 210));     // x
    painter.drawLine(QPointF(750, 250), QPointF(750, 360));   // y
    painter.drawLine(QPointF(750, 360), QPointF(1040, 360));     // x
    if(died_num != 0){
        curve_counter++;
        xList_die.append(curve_counter/3 + 750);
        yList_die.append(210 - (((double)died_num/(double)total_infected) * 200));
        pen.setColor(Qt::red);
        pen.setWidthF(2);
        painter.setPen(pen);
        for(int i = 0; i < yList_die.count(); i++){
            if(i == 0)
                painter.drawPoint(QPointF(xList_die[i], yList_die[i]));
            else
                painter.drawLine(QPointF(xList_die[i-1], yList_die[i-1]), QPointF(xList_die[i], yList_die[i]));
        }
    }
    xList_infect.append(round_counter/3 + 750);
    yList_infect.append(360 - infected_num / 3);
    pen.setColor(Qt::yellow);
    pen.setWidthF(2);
    painter.setPen(pen);
    for(int i = 0; i < yList_infect.count(); i++){
        if(i == 0)
            painter.drawPoint(QPointF(xList_infect[i], yList_infect[i]));
        else
            painter.drawLine(QPointF(xList_infect[i-1], yList_infect[i-1]), QPointF(xList_infect[i], yList_infect[i]));
    }
}
void MainWindow::DrawString(){
    QString str;
    QPainter painter(this);
    QFont font("宋体", 16, QFont::Bold, true);
    painter.setViewport(0, 0, width(), height());
    painter.setWindow(0, 0, width(), height());
    painter.drawRect(QRect(0,1,710,710));
    font.setLetterSpacing(QFont::AbsoluteSpacing, 3);
    painter.setFont(font);
    painter.setPen(Qt::black);
    str.sprintf("Day: %ld",(round_counter/10));
    painter.drawText(740,30,str);
    str.sprintf("Vacancy: %d",hospital.vacancy);
    painter.drawText(740,55,str);
    str.sprintf("Died Rate: %.1f%",(((double)died_num/(double)total_infected)*100));
    painter.drawText(740,80,str);
}
void MainWindow::DrawPoint(int x,int y,const QColor &color){
    x += 10;
    y += 10;
    QPainter painter(this);
    painter.setBrush(color);
    painter.setViewport(0, 0, width(), height());
    painter.setWindow(0, 0, width(), height()); //窗口
    painter.drawEllipse(x-2,y-2,4,4);
}
void MainWindow::DrawPeople(){
    for (int counter=0 ; counter<population ; counter++) {
        if(people[counter].infect == 0)                                         //healthy
            DrawPoint(people[counter].pos.x,people[counter].pos.y,Qt::green);
        else if (people[counter].infect == 1)                                   //infected
            DrawPoint(people[counter].pos.x,people[counter].pos.y,Qt::yellow);
        else if(people[counter].infect == 2)                                    //appeared(mildly)
            DrawPoint(people[counter].pos.x,people[counter].pos.y,Qt::red);
        else if(people[counter].infect == 3)                                    //intensive
            DrawPoint(people[counter].pos.x,people[counter].pos.y,Qt::gray);
        else if(people[counter].infect == 4)                                    //critical
            DrawPoint(people[counter].pos.x,people[counter].pos.y,Qt::black);
        else if(people[counter].infect == 5)                                       //die
            DrawPoint(people[counter].pos.x,people[counter].pos.y,Qt::white);
    }
}
void MainWindow::PeopleFlow(){
    for (int counter = 0; counter < population; counter++) {
        qsrand((unsigned int)QTime(0,0,0).secsTo(QTime::currentTime()));
        if((rand()%100) < flowpurpose * 100 && people[counter].is_admit == false){
            people[counter].move.speed = move_speed;
            people[counter].move.direction = (rand()%4);
            switch (people[counter].move.direction) {
                case 0:
                    if((people[counter].pos.x + people[counter].move.speed) < 700)
                        people[counter].pos.x += people[counter].move.speed;
                    if((people[counter].pos.y + people[counter].move.speed) < 700)
                        people[counter].pos.y += people[counter].move.speed;
                break;
                case 1:
                    if((people[counter].pos.x - people[counter].move.speed) > 0)
                        people[counter].pos.x -= people[counter].move.speed;
                    if((people[counter].pos.y + people[counter].move.speed) < 700)
                        people[counter].pos.y += people[counter].move.speed;
                break;
                case 2:
                    if((people[counter].pos.x + people[counter].move.speed) < 700)
                        people[counter].pos.x += people[counter].move.speed;
                    if((people[counter].pos.y - people[counter].move.speed) > 0)
                        people[counter].pos.y -= people[counter].move.speed;
                break;
                case 3:
                    if((people[counter].pos.x - people[counter].move.speed) > 0)
                        people[counter].pos.x -= people[counter].move.speed;
                    if((people[counter].pos.y - people[counter].move.speed) > 0)
                        people[counter].pos.y -= people[counter].move.speed;
                break;
            }
        }else {
            people[counter].move.speed = 0;
        }
    }
}
void MainWindow::PeopleInfect(){
    qsrand((unsigned int)QTime(0,0,0).secsTo(QTime::currentTime()));
    for (int counter = 0; counter < population; counter++) {
        if(people[counter].infect != 0 && people[counter].is_new_infect == false){     //yesterday ill person
            for (int counter2 = 0; counter2 < population; counter2++) {
                if((people[counter].pos.x + virus.area) > people[counter2].pos.x && counter != counter2
                        && (people[counter].pos.x - virus.area) < people[counter2].pos.x
                        && (people[counter].pos.y + virus.area) > people[counter2].pos.y
                        && (people[counter].pos.y - virus.area) < people[counter2].pos.y){//the person is too close to illness
                    if((rand()%100) < (people[counter2].protect * 100) && people[counter2].infect == 0){
                        people[counter2].infect = 1;
                        people[counter2].is_new_infect = true;
                        total_infected ++;
                    }
                }
            }
        }
    }
}
void MainWindow::TimerTick(){
    update();
    //label->setText("Hello World");
}
void MainWindow::PeopleEvolute(){
    qsrand((unsigned int)QTime(0,0,0).secsTo(QTime::currentTime()));
    for (int counter = 0; counter < population; counter++) {
        if(people[counter].infect != 0 && people[counter].is_admit == false)
            people[counter].ill_time ++ ;
        if(people[counter].infect != 0 && people[counter].is_new_infect == true)
            people[counter].is_new_infect = false;
        if(people[counter].infect == 1 && people[counter].ill_time >= virus.hide_time)
            people[counter].infect = 2;
        if(people[counter].infect >= 2 && people[counter].is_admit == false
                && (people[counter].ill_time - virus.hide_time) > hospital.diagnosis_time
                && hospital.vacancy >= 1){
            hospital.vacancy --;
            people[counter].is_admit = true;
            people[counter].pos.x = last_x;
            people[counter].pos.y = last_y;
            last_y += 7;
            if(last_y > (690)){
                last_y = 400;
                last_x +=7;
            }
        }
        if(people[counter].infect == 2){
            if(people[counter].age < 20 && (rand()%100) < (virus.prob.young_intensive * 100))
                people[counter].infect = 3;
            if(people[counter].age > 20 && people[counter].age < 50 && (rand()%100) < (virus.prob.mid_intensive * 100))
                people[counter].infect = 3;
            if(people[counter].age > 50 && (rand()%100) < (virus.prob.old_intensive * 100))
                people[counter].infect = 3;
        }else if (people[counter].infect == 3) {
            if(people[counter].age < 20 && (rand()%100) < (virus.prob.young_critical * 100))
                people[counter].infect = 4;
            if(people[counter].age > 20 && people[counter].age < 50 && (rand()%100) < (virus.prob.mid_critical * 100))
                people[counter].infect = 4;
            if(people[counter].age > 50 && (rand()%100) < (virus.prob.old_critical * 100))
                people[counter].infect = 4;
        }else if (people[counter].infect == 4) {
            if(people[counter].age < 20 && (rand()%100) < (virus.prob.young_died * 100))
                people[counter].infect = 5;
            if(people[counter].age > 20 && people[counter].age < 50 && (rand()%100) < (virus.prob.mid_died * 100))
                people[counter].infect = 5;
            if(people[counter].age > 50 && (rand()%100) < (virus.prob.old_died * 100))
                people[counter].infect = 5;
            if(people[counter].infect == 5){
                hospital.vacancy ++;
                died_num ++;
            }
        }
    }
}
void MainWindow::HospitalHeal(){
    for (int counter = 0; counter < population; counter++) {
        if(people[counter].is_admit == true){
            switch (people[counter].infect) {
                case 2:
                    if((rand()%100) < (virus.mild_heal * 100)){
                        people[counter].pos.x = rand()%700;
                        people[counter].pos.y = rand()%700;
                        people[counter].infect = 0;
                        people[counter].is_admit = false;
                        people[counter].is_new_infect = false;
                        people[counter].protect = 0;                //it has anti-body
                        hospital.vacancy++;
                    }
                break;
                case 3:
                    if((rand()%100) < (virus.intensive_heal * 100)){
                        people[counter].pos.x = rand()%700;
                        people[counter].pos.y = rand()%700;
                        people[counter].infect = 0;
                        people[counter].is_admit = false;
                        people[counter].is_new_infect = false;
                        people[counter].protect = 0;                //it has anti-body
                        hospital.vacancy++;
                    }
                break;
                case 4:
                    if((rand()%100) < (virus.critical_heal * 100)){
                        people[counter].pos.x = rand()%700;
                        people[counter].pos.y = rand()%700;
                        people[counter].infect = 0;
                        people[counter].is_admit = false;
                        people[counter].is_new_infect = false;
                        people[counter].protect = 0;                //it has anti-body
                        hospital.vacancy++;
                    }
                break;
            }
        }
    }
}
void MainWindow::CheckRate(){
    bool check_flag = false;
    infected_num = 0;
    for (int counter = 0;counter < population;counter++) {
        if(people[counter].is_admit != true && people[counter].infect != 0)
            check_flag = true;
        if(people[counter].infect >= 1 && people[counter].infect != 5)
            infected_num ++;
    }
    if(check_flag == false)
        fTimer->stop();
}
void MainWindow::paintEvent(QPaintEvent *)
{
    round_counter++;
    CheckRate();
    DrawCurve();
    DrawString();
    PeopleFlow();
    if(round_counter%10 == 0){
        PeopleEvolute();
        HospitalHeal();
    }
    PeopleInfect();
    DrawPeople();
}
