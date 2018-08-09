#include "mainwindow.h"

#include <QLabel>
#include <QDebug>
#include <QLayout>
#include "common.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    qmlWidget_{new QQuickWidget{QUrl{"qrc:/sidebar.qml"}}},
    pd_{DATA_URL, DATA_PASS},
    tampere_{mapScene_},
    hall_{gameScene_, weps_, pd_},
    shop_{&weps_}
{

    setFixedSize(1280, 720);

    QGridLayout *windowLayout = new QGridLayout{};
    windowLayout->setSpacing(0);
    windowLayout->setMargin(0);

    // Disable scrollbars
    graphicsView_.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView_.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    graphicsView_.setFixedSize(960, 720);
    graphicsView_.setScene(&mapScene_);

    gameScene_.setBackgroundBrush(QBrush(Qt::blue, Qt::SolidPattern));
    windowLayout->addWidget(&graphicsView_, 0, Qt::AlignLeft);
    windowLayout->addWidget(qmlWidget_, 0, Qt::AlignRight);

    QQuickItem *sidebar = qmlWidget_->rootObject();
    // There's really no easy way to connect QML-items with the new signal-slot syntax...
    // Connect hall entering button to QML
    bool success = connect(sidebar, SIGNAL(enterClicked()), this, SLOT(enterButtonPressed()));
    Q_ASSERT(success);
    // Connect hall leaving button to QML
    success = connect(sidebar, SIGNAL(leaveClicked()), this, SLOT(leaveButtonPressed()));
    Q_ASSERT(success);
    // Connect Herwood button as well
    success = connect(sidebar, SIGNAL(herwClicked()), this, SLOT(herwButtonPressed()));
    Q_ASSERT(success);

    // On clearing hall, run our cleared handler
    success = connect(&hall_, &ParkingHall::cleared, this, &MainWindow::hallCleared);
    Q_ASSERT(success);
    // Similarly for failed
    success = connect(&hall_, &ParkingHall::failed, this, &MainWindow::hallFailed);
    Q_ASSERT(success);

    // Tie shopping UI to shopping backend
    success = connect(sidebar, SIGNAL(buyWeapon(int, QString)), &shop_,
                      SLOT(buyWeapon(int, QString)));
    Q_ASSERT(success);
    success = connect(&shop_, SIGNAL(weaponBoughtReadable(QVariant, QVariant)), sidebar,
                      SLOT(weaponChanged(QVariant, QVariant)));
    Q_ASSERT(success);
    success = connect(&shop_, SIGNAL(cashChanged(QVariant)), sidebar, SLOT(cashChanged(QVariant)));
    Q_ASSERT(success);
    success = connect(&shop_, &Shopping::weaponBought, &hall_, &ParkingHall::setInventorySlot);
    Q_ASSERT(success);

    QWidget *mainWidget = new QWidget{this};
    mainWidget->setFixedSize(1280, 720);
    mainWidget->setLayout(windowLayout);
    mainWidget->show();

    init();
}

MainWindow::~MainWindow() {}

void MainWindow::init() {
    // Load levels, missions, weapons
    levs_.loadFolder("../lev/");
    miss_.loadFolder("../mis/");
    weps_.loadFolder("../wep/");

    missions_completed_ = 0;
    inPark_ = false;

    QQuickItem *sidebar = qmlWidget_->rootObject();
    bool success = connect(&hall_, SIGNAL(logMessage(QVariant)), sidebar, SLOT(logMessage(QVariant)));
    Q_ASSERT(success);

    success = connect(&hall_, SIGNAL(healthChanged(QVariant, QVariant)),
                      sidebar, SLOT(healthChanged(QVariant, QVariant)));
    Q_ASSERT(success);

    success = connect(&hall_, SIGNAL(inventoryChanged(QVariant, QVariant)),
                      sidebar, SLOT(weaponChanged(QVariant, QVariant)));
    Q_ASSERT(success);

    // Init cash and cash UI
    shop_.addCash(STARTING_CASH);
}


void MainWindow::enterPark() {
    QString name{tampere_.selectedPark()};
    QColor bg_col;
    qDebug() << "entering" << name;
    Q_ASSERT(levs_.exists(name));

    qDebug() << "loading level";
    pd_.setCurrent(name);
    hall_.loadLevel(levs_.level(name));
    qDebug() << "loading mission";
    hall_.loadMission(miss_.mission(name));
    switch (miss_.mission(name).value("objective")) {
        case MISSION_ELIMINATE:
            bg_col.setRgb(62,2,2); break; //RED AS BLOOD
        case MISSION_RESOURCE:
            bg_col.setRgb(2,52,2); break; //GREEN HIPPIES
        default:
            bg_col.setRgb(2,2,62);
    }
    gameScene_.setBackgroundBrush(QBrush(bg_col, Qt::SolidPattern));

    graphicsView_.setScene(&gameScene_);
    setFocus();
    qDebug() << "scene up";

    inPark_ = true;
}

void MainWindow::leavePark() {
    graphicsView_.setScene(&mapScene_);

    inPark_ = false;
}

void MainWindow::enterButtonPressed() {
    if (!tampere_.isParkSelected()) {
        emit hall_.logMessage("Valitse ensin parkkihalli!");
    } else if (! pd_.dataReady) {
        emit hall_.logMessage("Odottaa Finnparkin APIa!");
    }
    else {
        enterPark();

        // Disable enter button and enable leave
        QQuickItem *root = qmlWidget_->rootObject();
        QObject *enterButton = root->findChild<QObject *>("enterButton");
        QObject *leaveButton = root->findChild<QObject *>("leaveButton");
        Q_ASSERT(enterButton);
        Q_ASSERT(leaveButton);
        enterButton->setProperty("enabled", false);
        leaveButton->setProperty("enabled", true);
    }
}

void MainWindow::leaveButtonPressed() {
    leavePark();

    // Enable enter button and disable leave-button
    QQuickItem *root = qmlWidget_->rootObject();
    QObject *enterButton = root->findChild<QObject *>("enterButton");
    QObject *leaveButton = root->findChild<QObject *>("leaveButton");
    Q_ASSERT(enterButton);
    Q_ASSERT(leaveButton);
    enterButton->setProperty("enabled", true);
    leaveButton->setProperty("enabled", false);
}

void MainWindow::herwButtonPressed() {
    // Enters the boss hall
    tampere_.enterHerw();

    // Disable button
    QQuickItem *root = qmlWidget_->rootObject();
    QObject *herwButton = root->findChild<QObject *>("herwButton");
    Q_ASSERT(herwButton);
    herwButton->setProperty("enabled", false);
}

void MainWindow::hallCleared(const QString &hallName, int prize) {
    qDebug() << "hall" << hallName << "cleared!";
    tampere_.removePark(hallName);
    missions_completed_++;

    shop_.addCash(prize);

    if (missions_completed_ == levs_.count() - 1) {
        // Enable herwood hall
        QQuickItem *root = qmlWidget_->rootObject();
        QObject *herwButton = root->findChild<QObject *>("herwButton");
        Q_ASSERT(herwButton);
        herwButton->setProperty("enabled", true);
    } else if (missions_completed_ == levs_.count()) {
        // If herwood is beaten, game is won

        tampere_.gameWon();
    }

    leaveButtonPressed();
}

void MainWindow::hallFailed() {
    // TODO: Game over?
    leaveButtonPressed();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (inPark_) {
        switch (event->key()) {
        case Qt::Key_W: {
            hall_.movePlayerToDirection(0, -1);
            break;
        }
        case Qt::Key_A: {
            hall_.movePlayerToDirection(-1, 0);
            break;
        }
        case Qt::Key_S: {
            hall_.movePlayerToDirection(0, 1);
            break;
        }
        case Qt::Key_D: {
            hall_.movePlayerToDirection(1, 0);
            break;
        }
        default: {
            QMainWindow::keyPressEvent(event);
            break;
        }
        }
    }
}
