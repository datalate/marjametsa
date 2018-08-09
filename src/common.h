// Includes common variables used in multiple places

#ifndef COMMON_H
#define COMMON_H

#include <QString>

static const QString PARK_FNPK1{"FNPK.1"};
static const QString PARK_FNPK12{"FNPK.12"};
static const QString PARK_FNPK13{"FNPK.13"};
static const QString PARK_FNPK16{"FNPK.16"};
static const QString PARK_FNPK25{"FNPK.25"};
static const QString PARK_FNPK4{"FNPK.4"};
static const QString PARK_TRE48{"TRE.48"};

static const char TILE_GROUND{' '};
static const char TILE_PILLAR{'.'};
static const char TILE_WALL_CORNER{'+'};
static const char TILE_WALL_SOUTHFACING{'_'};
static const char TILE_WALL_WESTFACING{'|'};
static const char TILE_WALL_INVISIBLE{'x'};

static const char CHARACTER_PLAYER{'o'};
static const char CHARACTER_ENEMY{'e'};
static const char CHARACTER_NPC{'n'};

static const char RESOURCE_SPAWNER{'r'};

static const QString WEAPON_KNIFE{"knife"};
static const QString WEAPON_PISTOL{"pistol"};
static const QString WEAPON_SMG{"smg"};
static const QString WEAPON_SHOTGUN{"shotgun"};
static const QString WEAPON_RIFLE{"rifle"};

static const int HEALTH_PLAYER{100};
static const int HEALTH_ENEMY{50};

enum MissionType {
    MISSION_ELIMINATE = 0,
    MISSION_RESOURCE
};

static const QString DATA_URL{"http://parkingdata.finnpark.fi:8080/Datex2/ParkingFacilities"};
static const QString DATA_PASS{"user:pass"};
static const unsigned int REFRESH_INTERVAL{60000};

static const int STARTING_CASH{400};

#endif // COMMON_H
