#include "parkinghall.h"
#include "weaponparser.h"

#include <QDebug>
#include <QQueue>

#include <cmath>
#include <stdlib.h>


ParkingHall::ParkingHall(QGraphicsScene &scene, const WeaponParser &weps,
                         ParkingData &pd, QObject *parent) :
    QObject{parent}, scene_{&scene}, weps_{&weps}, pd_{&pd},
    inventory_{"knife", "knife", "knife", "knife"} {

    // Connect player signals
    bool success = connect(&player_, &Character::killed,
                           this, &ParkingHall::characterKilled);
    Q_ASSERT(success);
    success = connect(&player_, &Character::healthChanged,
                [this](Character *c, int health) {
                    Q_UNUSED(c);
                    emit healthChanged(0, health);
                });
    Q_ASSERT(success);

    scene_->addItem(&highlight_);
    scene_->addItem(&player_);
}

void ParkingHall::loadLevel(const LevelParser::Level &lvl) {
    qDebug() << "clear previous";
    clearLevel();

    // Reset player
    player_.resetHealth();
    player_.setWeapon(getInventorySlot(0));

    int y_counter{0};
    int x_counter{0};
    qDebug() << "inserting tiles";

    prize_ = 0;
    for (auto &row: lvl.data()) {
        for (auto c: row) {
            if (c == CHARACTER_PLAYER) {

                qDebug() << "player spawner placed";
                // Add ground under player spawn and move player
                tiles_.push_back(new Tile{x_counter, y_counter, TILE_GROUND});
                player_.move(x_counter, y_counter);
            }
            else if (c == CHARACTER_ENEMY) {
                // Add ground under enemy spawn
                tiles_.push_back(new Tile{x_counter, y_counter, TILE_GROUND});

                addEnemy(x_counter, y_counter, HEALTH_ENEMY);
            }
            else if (c == CHARACTER_NPC) {
                // Add ground under npc spawn
                tiles_.push_back(new Tile{x_counter, y_counter, TILE_GROUND});

                addNpc(x_counter, y_counter);
            }
            else if (c == RESOURCE_SPAWNER) {
                // Add ground under resource spawn
                tiles_.push_back(new Tile{x_counter, y_counter, TILE_GROUND});

                addResource(x_counter, y_counter);
            }
            else {
                tiles_.push_back(new Tile{x_counter, y_counter, c});
            }

            scene_->addItem(tiles_.back()); // Scene takes ownership here

            // Connect needed tile signals
            bool success = connect(tiles_.back(), &Entity::clicked, this, &ParkingHall::tileClicked);
            Q_ASSERT(success);
            success = connect(tiles_.back(), &Tile::hoverEnter, this, &ParkingHall::showHighlight);
            Q_ASSERT(success);
            success = connect(tiles_.back(), &Tile::hoverLeave, this, &ParkingHall::hideHighlight);
            Q_ASSERT(success);

            ++x_counter;
        }
        x_counter = 0;
        ++y_counter;
    }

    qDebug() << "hall loaded";
    lvl_ = &lvl;
}

void ParkingHall::loadMission(const Mission &m) {
    // Get mission data
    const int objective{m.value(QString{"objective"})};
    const int min_random{m.value(QString{"randomEnemiesMin"})};
    const int max_random{m.value(QString{"randomEnemiesMax"})};

    // use parkingdata to vary amount of random enemies
    double multiplier{pd_->occupiedSpots() / (double)pd_->totalSpots()};
    if (multiplier < 0.3) {
        multiplier *= 1.6;
    }
    if (multiplier < 0.2) {
        multiplier = 0.2;
    }
    const int randoms = round((max_random - min_random) * multiplier + min_random);

    objective_ = static_cast<MissionType>(objective);

    if (objective_ == MissionType::MISSION_RESOURCE) {
        emit logMessage("Tehtävä: Kerää kaikki resurssit");
        // Set reward to 100 per resource
        prize_ = 100 * resources_.size();
    }
    else if (objective_ == MissionType::MISSION_ELIMINATE) {
        emit logMessage("Tehtävä: Tuhoa kaikki viholliset");
        prize_ = 15 * enemies_.size();
    }

    spawnEnemies(randoms);
}

void ParkingHall::movePlayerToDirection(int dir_x, int dir_y) {
    // Check that values are correct
    if (dir_x == 0) {
        Q_ASSERT(dir_y != 0);
    }
    else if (dir_y == 0) {
        Q_ASSERT(dir_x != 0);
    }
    else {
        Q_ASSERT(false);
    }

    // Calculate position
    const int x{player_.x() + dir_x};
    const int y{player_.y() + dir_y};

    if (tileExists(x, y) && canMove(x, y)) {
        movePlayer(x, y);
    }
}

void ParkingHall::movePlayer(int x, int y) {
    player_.move(x, y);
    updateGame();
}

void ParkingHall::attack(Character &c1, Character &c2, const Weapon &wep) {
    double range_sq{distance_sq(c1, c2)};
    int dmg{wep.rollDamage(range_sq)};

    emit logMessage(QString{"%1 hyökkää aseena %4: %2 ottaa %3 lämää"}
            .arg(c1.name()).arg(c2.name()).arg(dmg).arg(wep.readableName()));

    // enemies are better rested if night loitering is not possible :D
    if (! pd_->nightParking()) {
        if (dynamic_cast<Enemy*>(&c1) && dmg > 1) {
            ++dmg;
        }
    }

    c2.decreaseHealth(dmg);
}

double ParkingHall::distance_sq(const Entity &e1, const Entity &e2) {
    return std::pow(e2.x() - e1.x(), 2) + std::pow(e2.y() - e1.y(), 2);
}

double ParkingHall::distance_sq(QPoint pos1, QPoint pos2) {
    return std::pow(pos2.x() - pos1.x(), 2) + std::pow(pos2.y() - pos1.y(), 2);
}

double ParkingHall::distance(const Entity &e1, const Entity &e2) {
    return std::sqrt(double(distance_sq(e1, e2)));
}

double ParkingHall::distance(QPoint pos1, QPoint pos2) {
    return std::sqrt(double(distance_sq(pos1, pos2)));
}

// Uses BFS (Breadth-first search) to find the path
bool ParkingHall::findPath(int x1, int y1, int x2, int y2, QPoint &pos) {
    struct Node {
        int x;
        int y;
        bool empty;
        bool visited;
        Node *parent;
    };

    // Initialize node array
    Node graph[lvl_->width()][lvl_->height()];
    for (int x{0}; x < lvl_->width(); ++x) {
        for (int y{0}; y < lvl_->height(); ++y) {
            graph[x][y].x = x;
            graph[x][y].y = y;
            graph[x][y].empty = canMove(x, y);
            graph[x][y].visited = false;
            graph[x][y].parent = nullptr;
        }
    }
    graph[x2][y2].empty = true;

    QQueue<QPoint> nodes;
    nodes.enqueue({x1, y1}); // Add root node to queue

    while (!nodes.empty()) {
        QPoint currentNode{nodes.dequeue()};
        int c_x{currentNode.x()};
        int c_y{currentNode.y()};

        if (c_x == x2 && c_y == y2) {

            // Path found, reconstruct from end
            Node *last{&graph[c_x][c_y]};
            Node *iterator{nullptr};

            while (true) {
                iterator = last->parent;

                // The root node has no parent set
                if (iterator->parent == nullptr) {
                    pos = QPoint{last->x, last->y};
                    return true;
                }
                last = iterator;
            }

            // We should never end up here
            Q_ASSERT(false);
        }

        // Not the most optimized way, but clean
        QVector<QPoint> nearTiles;
        if (c_x > 0) { // x - 1
            nearTiles.append({c_x - 1, c_y});
        }
        if (c_x < lvl_->width() - 1) { // x + 1
            nearTiles.append({c_x + 1, c_y});
        }
        if (c_y > 0) { // y - 1
            nearTiles.append({c_x, c_y - 1});
        }
        if (c_y < lvl_->height() - 1) { // y + 1
            nearTiles.append({c_x, c_y + 1});
        }

        // Traverse nearest nodes
        foreach (const QPoint node, nearTiles) {
            int x{node.x()};
            int y{node.y()};

            if (!graph[x][y].visited && graph[x][y].empty) {
                graph[x][y].visited = true;
                graph[x][y].parent = &graph[c_x][c_y];
                nodes.enqueue({x, y});
            }
        }
    }

    // No nodes left, path was not found
    return false;
}

bool ParkingHall::tileExists(int x, int y) const {
    return (x < lvl_->width() && y < lvl_->height() &&
            x >= 0 && y >= 0);
}

bool ParkingHall::canMove(int x, int y) const {
    // Don't allow moving on top of enemies/npc/player
    foreach (const Enemy *e, enemies_) {
        if (x == e->x() && y == e->y()) {
            return false;
        }
    }
    foreach (const Npc *n, npcs_) {
        if (x == n->x() && y == n->y()) {
            return false;
        }
    }
    if (player_.x() == x && player_.y() == y) {
        return false;
    }

    // Checks if there is a free ground file
    static const QString floorTiles{QString() + TILE_GROUND + RESOURCE_SPAWNER +
                                    CHARACTER_PLAYER + CHARACTER_ENEMY +  + CHARACTER_NPC};
    return (floorTiles.contains(lvl_->data()[y][x]));
}

void ParkingHall::tileClicked(Entity *e) {
    Tile *const tile{dynamic_cast<Tile *>(e)};
    Q_ASSERT(tile);

    if (!tile->solid()) {
        // Move to/towards tile if it isn't solid
        if (canMove(tile->x(), tile->y())) {
            if (distance(player_, *tile) == 1) {
                movePlayer(tile->x(), tile->y());
            }
            else {
                QPoint nextPos;
                if (findPath(player_.x(), player_.y(), tile->x(), tile->y(), nextPos)) {
                    movePlayer(nextPos.x(), nextPos.y());
                }
            }
        }
    }
}

void ParkingHall::enemyClicked(Entity *e) {
    Enemy *const enemy{dynamic_cast<Enemy *>(e)};
    Q_ASSERT(enemy);

    const Weapon &currentWeapon{weps_->weapon(player_.weapon())};
    const double enemyDistance{distance(player_, *enemy)};

    if (currentWeapon.range() >= enemyDistance) {
        // Attack enemy if weapon has enough range
        attack(player_, *enemy, currentWeapon);
        updateGame();
    }
    else {
        // Move towards enemy, if not enough range and path is found
        QPoint nextPos;
        if (findPath(player_.x(), player_.y(), enemy->x(), enemy->y(), nextPos)) {
            movePlayer(nextPos.x(), nextPos.y());
        }
    }
}

void ParkingHall::showHighlight(QPoint pos) {
    highlight_.show();
    highlight_.move(pos.x(), pos.y());
}

void ParkingHall::hideHighlight() {
    highlight_.hide();
}

// Checks character type and acts accordingly
void ParkingHall::characterKilled(Character *c) {
    Enemy *enemy{dynamic_cast<Enemy *>(c)};
    if (enemy) {
        qDebug() << "Enemy was killed";
        emit logMessage("Vihollinen tuhottu!");

        // Remove enemy from the game
        scene_->removeItem(enemy);
        enemy->deleteLater();
        enemies_.removeAll(enemy);

        if (enemies_.empty() && objective_ == MISSION_ELIMINATE) {
            emit logMessage("Kaikki viholliset tuhottu!");
            emit logMessage("Halli vallattu");
            emit cleared(lvl_->name(), prize_);
        }

        return;
    }

    Npc *npc{dynamic_cast<Npc *>(c)};
    if (npc) {
        qDebug() << "Npc was killed";
        emit logMessage("Apuri kuoli");

        // Remove npc from the game
        scene_->removeItem(npc);
        npc->deleteLater();
        npcs_.removeAll(npc);

        return;
    }

    Player *player{dynamic_cast<Player *>(c)};
    if (player) {
        // Player is never removed from the scene

        emit logMessage("Pelaaja kuoli...");
        emit failed();
    }
}

// Remove all tiles, enemies, npcs and resources
void ParkingHall::clearLevel() {
    foreach (Tile *tile, tiles_) {
        scene_->removeItem(tile);

        // Since the ownership is lost with removeItem-call, delete the tile
        tile->deleteLater();
    }
    tiles_.clear();

    foreach (Enemy *enemy, enemies_) {
        scene_->removeItem(enemy);
        enemy->deleteLater();
    }
    enemies_.clear();

    foreach (Npc *npc, npcs_) {
        scene_->removeItem(npc);
        npc->deleteLater();
    }
    npcs_.clear();

    foreach (Resource *resource, resources_) {
        scene_->removeItem(resource);
        resource->deleteLater();
    }
    resources_.clear();

    hideHighlight();
}

// Spawns number amount of enemies and adjusts their HP depending on park trend
void ParkingHall::spawnEnemies(int number) {
    qDebug() << "Spawning" << number << "random enemies";
    int n{0}, e{0}, hp{HEALTH_ENEMY};
    switch (pd_->trend()) {
        case 1:  hp += 15; break;
        case -1: hp -= 15; break;
    }

    while (n < number) {
        int x{rand() % lvl_->width()};
        int y{rand() % lvl_->height()};

        if (canMove(x, y)) {
            addEnemy(x, y, hp);
            ++n;
        } else if (e > 3 * number) break;
          else ++e; //prevent 911 I mean infinite loop
    }
}

// Simulates AI and collects resources
void ParkingHall::updateGame() {
    foreach (Enemy *e, enemies_) {
        // Enemy attacking
        const Weapon &wep = weps_->weapon(e->weapon());
        if (distance(*e, player_) <= wep.range()) {
            attack(*e, player_, wep);
        }
        // Enemy movement
        else if (rand() % 2 == 0) {
            QPoint nextPos;
            if (findPath(e->x(), e->y(), player_.x(), player_.y(), nextPos)) {
                e->move(nextPos.x(), nextPos.y());
            } else {
                // Attack NPC if player isn't reachable

                Npc *npc = nearestNpc(*e);
                if (npc == nullptr) {
                    break;
                }

                if (distance(*e, *npc) <= wep.range()) {
                    attack(*e, *npc, wep);
                }
                else {
                    QPoint nextPos;
                    if (findPath(e->x(), e->y(), npc->x(), npc->y(), nextPos)) {
                        e->move(nextPos.x(), nextPos.y());
                    }
                }
            }
        }
    }

    foreach (Npc *n, npcs_) {
        // Attack nearest enemy from player
        Enemy *enemy = nearestEnemy(player_);

        const Weapon &wep = weps_->weapon(n->weapon());
        if ((enemy != nullptr) && (distance(*n, *enemy) <= wep.range())) {
            // NPC attacking
            attack(*n, *enemy, wep);
        }
        else if (distance(*n, player_) > 5) {
            // Don't move too far from player
            QPoint nextPos;
            if (findPath(n->x(), n->y(), player_.x(), player_.y(), nextPos)) {
                n->move(nextPos.x(), nextPos.y());
            }
        }
        else {
            // Move closer to enemy
            if (enemy != nullptr) {
                QPoint nextPos;
                if (findPath(n->x(), n->y(), enemy->x(), enemy->y(), nextPos)) {
                    n->move(nextPos.x(), nextPos.y());
                }
            }
        }
    }

    foreach (Resource *r, resources_) {
        if (player_.x() == r->x() && player_.y() == r->y()) {
            // Collect resource
            qDebug() << "Resource was collected";

            scene_->removeItem(r);
            r->deleteLater();
            resources_.removeAll(r);

            emit logMessage("Resurssi kerätty");
            if (resources_.empty() && objective_ == MISSION_RESOURCE) {
                emit logMessage("Kaikki resurssi kerätty!");
                emit logMessage("Halli vallattu");
                // Done with this
                emit cleared(lvl_->name(), prize_);
            }

            break; // Only one res per tile
        }
    }
}

Enemy *ParkingHall::nearestEnemy(const Entity &entity) const {
    Enemy *nearestEnemy{nullptr};
    int nearestDistance{99999};

    foreach (Enemy *e, enemies_) {
        const double dist{distance(entity, *e)};
        if (dist < nearestDistance) {
            nearestEnemy = e;
            nearestDistance = dist;
        }
    }

    return nearestEnemy;
}

Npc *ParkingHall::nearestNpc(const Entity &entity) const {
    Npc *nearestNpc{nullptr};
    int nearestDistance{99999};

    foreach (Npc *n, npcs_) {
        const double dist{distance(entity, *n)};
        if (dist < nearestDistance) {
            nearestNpc = n;
            nearestDistance = dist;
        }
    }

    return nearestNpc;
}

void ParkingHall::addEnemy(int x, int y, int health) {
    //if the facilities are open, some of the enemies might be drunk or caffeinated
    if (pd_->isOpen()) health += 6 - (rand() % 10 + 1);

    enemies_.push_back(new Enemy{x, y, health});
    scene_->addItem(enemies_.back()); // Scene takes ownership here

    // Connect enemy signals
    bool success = connect(enemies_.back(), &Entity::clicked, this, &ParkingHall::enemyClicked);
    Q_ASSERT(success);
    success = connect(enemies_.back(), &Character::killed, this, &ParkingHall::characterKilled);
    Q_ASSERT(success);

    qDebug() << "Enemy (" << enemies_.back()->weapon() << ") added to" << QPoint{x, y};
}

void ParkingHall::addNpc(int x, int y) {
    npcs_.push_back(new Npc{x, y});
    scene_->addItem(npcs_.back()); // Scene takes ownership here

    int npc_number{npcs_.size()};

    // Connect NPC signals
    bool success = connect(npcs_.back(), &Character::killed, this, &ParkingHall::characterKilled);
    Q_ASSERT(success);

    // Adapt the healthChanged signal for QML
    success = connect(npcs_.back(), &Character::healthChanged,
            [this, npc_number](Character *c, int health) {
                Q_UNUSED(c);
                emit healthChanged(npc_number, health);
            });
    Q_ASSERT(success);
    npcs_.back()->resetHealth();

    success = connect(npcs_.back(), &Character::weaponChanged,
            [this, npc_number](Character *c, QString weapon) {
                Q_UNUSED(c);
                emit inventoryChanged(npc_number, weps_->weapon(weapon).readableName());
            });
    Q_ASSERT(success);

    // This, like the previous, works because once we insert an NPC, the sizes
    // of the vector and as such npc_numbers are 1, 2, 3
    npcs_.back()->setWeapon(getInventorySlot(npc_number));

    qDebug() << "Npc added to" << QPoint{x, y};
}

void ParkingHall::addResource(int x, int y) {
    resources_.push_back(new Resource{x, y});
    scene_->addItem(resources_.back());

    qDebug() << "Resource added to" << QPoint{x, y};
}

void ParkingHall::setInventorySlot(int slot, QString weapon) {
    Q_ASSERT(slot < 4);

    inventory_[slot] = weapon;

    emit inventoryChanged(slot, weapon);
}

QString ParkingHall::getInventorySlot(int slot) {
    Q_ASSERT(slot < 4);

    return inventory_[slot];
}
