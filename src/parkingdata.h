#ifndef PARKINGDATA_H
#define PARKINGDATA_H

#include <QString>
#include <QObject>
#include <QHeaderView>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QFile>
#include <QTimer>
#include <QSet>
#include <QDirIterator>
#include <QMap>
#include "common.h"

//I keep a local copy of the parking data reasonably up-to-date,
//or serve predownloaded data if finnpark API unavailable.
//Then you can query me stuff about a specific hall by setting it via
//setCurrent() and using the cute functions like isFull() and totalSpots().
class ParkingData : public QObject {
    Q_OBJECT

public:
    explicit ParkingData(QUrl xmlUrl, QString passwd = "",
                         QWidget *parent = 0);
    virtual ~ParkingData();
    //changes which hall the functions below query
    bool setCurrent(QString id);

    int trend(); // dec stable increasing -1 0 +1
    int vacantSpots();
    int occupiedSpots();
    int totalSpots();
    bool isFull();
    bool isOpen();
    bool nightParking();
    QPointF location();

    QString currentId = PARK_FNPK12;
    //At first no data has been parsed
    bool dataReady = false;

private slots:
    void processData(QNetworkReply* reply);
    void pull(); //send the http request

private:
    bool parseXml(bool offline = false); //xml -> hallData_

    QNetworkRequest request_;
    QString passwd_;
    QByteArray rawData_;
    QNetworkAccessManager manager_;
    QTimer pullTimer_;

    QDateTime dataAge_ = QDateTime::fromMSecsSinceEpoch(0);

    QSet<QString> allowedIds_;
    QMap<QString, QMap<QString, QString>> hallData_;
};

#endif // PARKINGDATA_H
