#include "mainwindow.h"
#include <QApplication>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // Init random seed
    srand(time(NULL));

    return a.exec();
}
