// Main class, handles switching between map and game scene
// Updates the sidebar

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QQuickWidget>
#include <QQuickItem>
#include "tampere.h"
#include "parkinghall.h"
#include "levelparser.h"
#include "missionparser.h"
#include "weaponparser.h"
#include "parkingdata.h"
#include "shopping.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    void init();
    void enterPark();
    void leavePark();

private slots:
    void enterButtonPressed();
    void leaveButtonPressed();
    void herwButtonPressed();

    void hallCleared(const QString &hallName, int prize);
    void hallFailed();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QGraphicsView graphicsView_;
    QGraphicsScene mapScene_;
    QGraphicsScene gameScene_;
    QQuickWidget *qmlWidget_;

    ParkingData pd_;

    LevelParser levs_;
    MissionParser miss_;
    WeaponParser weps_;
    Tampere tampere_;
    ParkingHall hall_;
    Shopping shop_;

    int missions_completed_;
    bool inPark_;
};

#endif // MAINWINDOW_H
