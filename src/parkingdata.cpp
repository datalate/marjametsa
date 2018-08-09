#include "parkingdata.h"

ParkingData::ParkingData(QUrl xmlUrl, QString passwd, QWidget *parent):
    QObject{parent}, request_{xmlUrl}, passwd_{passwd} {

    //get list of parking hall ids we care about
    QDirIterator* it = new QDirIterator("../lev");
    while (it->hasNext()) {
        if (it->next().endsWith(".")) continue; //ignore .. and .
        allowedIds_.insert(it->fileName());
    }
    delete it; it = 0;
    Q_ASSERT(allowedIds_.size() > 3);

    bool success = connect(&manager_, &QNetworkAccessManager::finished,
                           this, &ParkingData::processData);
    Q_ASSERT(success);

    if (! passwd_.isEmpty()) {
        passwd_ = "Basic " + passwd_.toLocal8Bit().toBase64();
        request_.setRawHeader("Authorization", passwd_.toLocal8Bit());
    }
    manager_.get(request_); //start first download

    pullTimer_.setInterval(REFRESH_INTERVAL);
    pullTimer_.start();
    // begin pulling new data every REFRESH_INTERVAL
    success = connect(&pullTimer_, &QTimer::timeout, this, &ParkingData::pull);
    Q_ASSERT(success);
}

ParkingData::~ParkingData() {}

bool ParkingData::setCurrent(QString id) {
    if (allowedIds_.contains(id)) { //we have a map for valid values
        currentId = id;
        return true;
    }
    return false;
}

int ParkingData::trend() {
    QString t = hallData_[currentId]["parkingFacilityOccupancyTrend"];
    if (t == "stable") return 0;
    if (t == "increasing") return 1;
    else return -1;
}

int ParkingData::vacantSpots() {
    return hallData_[currentId]["totalNumberOfVacantParkingSpaces"].toInt();
}

int ParkingData::occupiedSpots() {
    return hallData_[currentId]["totalNumberOfOccupiedParkingSpaces"].toInt();
}

int ParkingData::totalSpots() {
    return vacantSpots() + occupiedSpots();
}

bool ParkingData::isFull() {
    return hallData_[currentId]["spacesAvailable"] == "false";
}

bool ParkingData::isOpen() {
    return hallData_[currentId]["isOpen"] == "true";
}

bool ParkingData::nightParking() {
    return hallData_[currentId]["overnightParkingPermitted"] == "true";
}

QPointF ParkingData::location() {
    double lat = hallData_[currentId]["latitude"].toDouble();
    double lon = hallData_[currentId]["longitude"].toDouble();
    return QPointF{lon, lat};
}

void ParkingData::processData(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Download failed:" << reply->errorString();
        //download failed, use local data if needed
        if (! dataReady) parseXml(true);
    } else {
        rawData_ = reply->readAll();
        reply->deleteLater();
        qDebug() << "Download succeeded, parsing.";

        // parse local data if downloaded doesn't parse
        if(! parseXml()) {
            if (! dataReady) Q_ASSERT(parseXml(true));
        }
        dataReady = true;
    }
}

void ParkingData::pull() {
    manager_.get(request_);
    if (dataReady) qDebug() << currentId << "trend is: " << trend();
}

bool ParkingData::parseXml(bool offline) {
    QXmlStreamReader* reader;
    QFile* localFile;
    if (offline) {
        localFile = new QFile("../dat/offline_copy.xml");
        localFile->open(QIODevice::ReadOnly | QIODevice::Text);
        reader = new QXmlStreamReader(localFile);
    } else {
        reader = new QXmlStreamReader(rawData_);
    }

    int mergeCount = 0;
    while (! reader->atEnd()) {
        QXmlStreamReader::TokenType token = reader->readNext();

        if (token == QXmlStreamReader::StartElement) {
            //save data publication time
            if (reader->name() == "publicationTime") {
                QDateTime a{ QDateTime::fromString(reader->readElementText(), Qt::ISODate) };
                qDebug() << "Data Age: " << a.toMSecsSinceEpoch();
                if (a <= dataAge_) return false;
                else dataAge_ = a;
            }

            // Read data from inside a parkingFacility element
            if (reader->name() == "parkingFacility"
                && reader->attributes().hasAttribute("id")) {
                QString id = reader->attributes().first().value().toString();
                if ( ! allowedIds_.contains(id) ) continue;

                bool parkingSet = false;
                // Read next element so the loop condition doesn't immediately fail
                reader->readNextStartElement();

                // Loop till we reach the next facility or facilitystatus
                while (reader->name() != "parkingFacility" && reader->name() != "parkingFacilityStatus") {
                    reader->readNextStartElement();
                    if (reader->name() == "overnightParkingPermitted") {
                        hallData_[id]["overnightParkingPermitted"] = reader->readElementText();
                        parkingSet = true;
                    }

                    // Location
                    if (reader->name() == "latitude") {
                        hallData_[id]["latitude"] = reader->readElementText();
                    }
                    if (reader->name() == "longitude") {
                        hallData_[id]["longitude"] = reader->readElementText();
                    }
                }
                if (! parkingSet) hallData_[id]["overnightParkingPermitted"] = "false";
            }

            //read the second set of info per parking hall
            if (reader->name() == "parkingFacilityStatus") {
                QString id;
                QMap<QString, QString> tmpHall;
                QXmlStreamReader::TokenType tt = reader->readNext();

                while (! (reader->name().toString() == "parkingFacilityStatus"
                          && reader->tokenType() == QXmlStreamReader::EndElement)) {

                    if (reader->name() == "parkingFacilityReference") {
                        id = reader->attributes().first().value().toString();
                        // We don't care about this hall; next
                        if (! allowedIds_.contains(id)) { id.clear(); break; }
                    }
                    if (tt == QXmlStreamReader::StartElement ) {
                        // parkingFacilityStatus is used for both spaces and open/closed
                        QString elementName = reader->name().toString();
                        if (elementName == "parkingFacilityStatus") {
                            QString text = reader->readElementText();
                            if (text == "full" || text == "fullAtEntrance") {
                                tmpHall["spacesAvailable"] = "false";
                            } else if (text == "spacesAvailable") {
                                tmpHall["spacesAvailable"] = "true";
                            } else if (text == "open") {
                                tmpHall["isOpen"] = "true";
                            } else {
                                tmpHall["isOpen"] = "false";
                            }
                        } else {
                            tmpHall[elementName] = reader->readElementText();
                        }
                    }
                    tt = reader->readNext(); //tt tokentoken D;
                }
                if (! id.isEmpty()) {
                    hallData_[id].unite(tmpHall);
                    ++mergeCount;
                    if (mergeCount == allowedIds_.count()) break; // all data gathered
                }
            }
        }
    }

    delete reader; reader = 0;
    if (offline) { delete localFile; localFile = 0; }

    return mergeCount == allowedIds_.count(); //data was insufficient if false
}

