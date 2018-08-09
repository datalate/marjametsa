#include "tampere.h"

#include <QDebug>
#include "common.h"

TamperePark::TamperePark(int x, int y, const QString &name) :
    QObject{}, QGraphicsPixmapItem{}, name_{name} {

    setAcceptedMouseButtons(Qt::LeftButton);
    setFlags(QGraphicsItem::ItemIsSelectable);
    setPixmap(QPixmap("../gfx/p.png"));
    setPos(x, y);
    setOffset(-0.5 * QPointF(pixmap().width(), pixmap().height())); // center offset
}

QString TamperePark::name() const {
    return name_;
}

void TamperePark::mousePressEvent(QGraphicsSceneMouseEvent *e) {
    Q_UNUSED(e);

    QGraphicsPixmapItem::mousePressEvent(e);
    emit clicked(name());
}

void TamperePark::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
    Q_UNUSED(e);

    QGraphicsPixmapItem::mouseReleaseEvent(e);
    // Comment to override the default event
}


Tampere::Tampere(QGraphicsScene &scene) :
    scene_{&scene} {

    const QImage map_image{"../gfx/map1.png"};
    scene_->setBackgroundBrush(QBrush(map_image));
    scene_->setSceneRect(map_image.rect());

    insertPark(530, 480, PARK_FNPK12);
    insertPark(180, 325, PARK_FNPK1);
    insertPark(275, 155, PARK_FNPK16);
    insertPark(375, 350, PARK_FNPK25);
    insertPark(580, 460, PARK_FNPK4);
    insertPark(240, 450, PARK_TRE48);
}

void Tampere::enterHerw() {
    const QImage herw_map{"../gfx/map_herw.png"};
    scene_->setBackgroundBrush(QBrush(herw_map));
    scene_->setSceneRect(herw_map.rect());

    insertPark(480, 360, PARK_FNPK13);
}

QString Tampere::selectedPark() const {
    return selectedPark_;
}

void Tampere::removePark(const QString &name) {
    selectedPark_.clear();
    scene_->removeItem(parks_[name]);
    parks_.remove(name);
}

void Tampere::gameWon() {
    scene_->removeItem(parks_[PARK_FNPK13]);
    parks_.remove(PARK_FNPK13);

    const QImage winner_map{"../gfx/map_winner.png"};
    scene_->setBackgroundBrush(QBrush(winner_map));
    scene_->setSceneRect(winner_map.rect());
}

bool Tampere::isParkSelected() const {
    return !selectedPark_.isEmpty();
}

void Tampere::parkClicked(const QString &name) {
    selectedPark_ = name;
}

void Tampere::insertPark(int x, int y, const QString &name) {
    TamperePark *park = new TamperePark{x, y, name};
    bool success = connect(park, &TamperePark::clicked, this, &Tampere::parkClicked);
    Q_ASSERT(success);

    parks_.insert(name, park);
    scene_->addItem(park); // Scene gets ownership
}

