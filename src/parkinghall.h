// Handles the game scene

#ifndef PARKINGHALL_H
#define PARKINGHALL_H

#include <QObject>
#include <QGraphicsScene>
#include <QVector>
#include "levelparser.h"
#include "weaponparser.h"
#include "mission.h"
#include "tile.h"
#include "player.h"
#include "enemy.h"
#include "npc.h"
#include "resource.h"
#include "tilehighlight.h"
#include "parkingdata.h"
#include "common.h"

class ParkingHall : public QObject {
    Q_OBJECT

signals:
    void cleared(const QString &hallName, int prize); // emitted, when the objective is done
    void failed(); // emitted, when the player is killed
    void logMessage(QVariant msg);
    void healthChanged(QVariant npc, QVariant health);
    void playerHealth(int health);
    void inventoryChanged(QVariant slot, QVariant weapon);

public:
    explicit ParkingHall(QGraphicsScene &scene, const WeaponParser &weps,
                         ParkingData &pd, QObject *parent = 0);

    void loadLevel(const LevelParser::Level &lvl); // must be called before loadMission
    void loadMission(const Mission &m);

    //dir_x: -1 moves west, 1 moves east
    //dir_y: -1 moves south, 1 moves north
    // One must be zero and other nonzero
    void movePlayerToDirection(int dir_x, int dir_y);

    // Moves player and simulates a turn, position must be valid
    void movePlayer(int x, int y);

    // c1 attacks c2 with wep
    void attack(Character &c1, Character &c2, const Weapon &wep);

    // Used to calculate distances between entities/tiles
    static double distance_sq(const Entity &e1, const Entity &e2);
    static double distance_sq(QPoint pos1, QPoint pos2);

    static double distance(const Entity &e1, const Entity &e2);
    static double distance(QPoint pos1, QPoint pos2);

    // Tries to find path from point 1 to 2
    // Returns true and sets next tile to pos if path was found
    bool findPath(int x1, int y1, int x2, int y2, QPoint &pos);

    bool tileExists(int x, int y) const;
    bool canMove(int x, int y) const; // Position must be valid

    QString getInventorySlot(int slot);

public slots:
    void setInventorySlot(int slot, QString weapon);

private slots:
    void tileClicked(Entity *e); // Entity must be of type Tile
    void enemyClicked(Entity *e); // Entity must be of type Enemy

    void showHighlight(QPoint pos);
    void hideHighlight();

    void characterKilled(Character *c);

private:
    void clearLevel();
    void spawnEnemies(int number);
    void updateGame(); // Should be called after any player action

    // Returns nearest character to entity or nullptr if there are none
    Enemy *nearestEnemy(const Entity &entity) const;
    Npc *nearestNpc(const Entity &entity) const;

    void addEnemy(int x, int y, int health); // Doesn't check position validity
    void addNpc(int x, int y); // Same here
    void addResource(int x, int y); // Same here

    const LevelParser::Level *lvl_; // Non-owning pointer
    QGraphicsScene *const scene_; // Non-owning pointer
    const WeaponParser *const weps_; // Non-owning pointer

    QVector<Tile*> tiles_; // No actual ownership, just to remember addresses
    QVector<Enemy*> enemies_; // Same goes here
    QVector<Npc*> npcs_; // Same goes here
    QVector<Resource*> resources_; // Same goes here

    Player player_;
    TileHighlight highlight_;
    // Mission objective (destroy or gather resource)
    MissionType objective_;

    ParkingData *pd_;
    // Inventory in use in this mission
    QVector<QString> inventory_;

    // The reward for completing this mission
    int prize_;
};

#endif // PARKINGHALL_H
