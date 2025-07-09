// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe/types/item.h"

//#include "util/glaze_qt.h"
//static_assert(ACQUISITION_USE_GLAZE);

#include <QString>
#include <QStringList>

#include <optional>
#include <vector>

struct ItemData
{
    struct WeaponData
    {
        float damage{0.0};
        float criticalChance{0.0};
        float physicalDps{0.0};
        float attacksPerSecond{0.0};
        float totalDps{0.0};
        float elementalDps{0.0};
        float chaosDps{0.0}; // custom acquisition extension
        float weaponRange{0.0};
    };

    struct ArmourData
    {
        int armour{0};
        int energyShield{0};
        int block{0};
        int evasionRating{0};
        int ward{0};
        int baseBercentile{0};
    };

    struct SocketGroup
    {
        unsigned red{0};
        unsigned green{0};
        unsigned blue{0};
        unsigned white{0};
        unsigned abyss{0};
    };

    struct SocketData
    {
        std::vector<SocketGroup> socketGroups;
        unsigned redSockets{0};
        unsigned greenSockets{0};
        unsigned blueSockets{0};
        unsigned whiteSockets{0};
        unsigned abyssSockets{0}; // custom acquisition extension
        unsigned totalSockets{0};
    };

    struct HeistData
    {
        int wings{0};
        int wingsRevealed{0};
        int escapeRoutes{0};
        int escapeRoutesRevealed{0};
        int rewardRooms{0};
        int rewardRoomsRevealed{0};
        int lockpickingLevel{0};
        int bruteForceLevel{0};
        int perceptionLevel{0};
        int demolutionLevel{0};
        int counterThaumaturgyLevel{0};
        int trapDisarmamentLevel{0};
        int agilityLevel{0};
        int deceptionLevel{0};
        int engineeringLevel{0};
    };

    ItemData(const poe::Item &item);

    QString id;
    QString name;
    QString typeLine;
    QString prettyName;
    QString baseType;
    QString itemCategory;
    QString icon;

    QString propertiesText1;
    QString propertiesText2;
    QString requirementsText;

    QStringList implicitMods;
    QStringList enchantMods;
    QStringList fracturedMods;
    QStringList explicitMods;
    QStringList craftedMods;

    QString requiredClass;
    QString corpseType;
    QString alternateArt;
    QString foilVariation;

    std::optional<WeaponData> weaponData;
    std::optional<ArmourData> armourData;
    std::optional<SocketData> socketData;
    std::optional<HeistData> heistData;

    poe::FrameType frameType;

    int w{0};
    int h{0};

    int requiredLevel{0};
    int requiredStrength{0};
    int requiredDexterity{0};
    int requiredIntelligence{0};

    int quality{0};
    int itemLevel{0};
    int gemLevel{0};
    int gemExperience{0};

    int talismanTier{0};
    int storedExperience{0};
    int stackSize{0};
    int stackSizeMax{0};
    int scourgeTier{0};

    // Influences
    bool shaper{false};
    bool elder{false};
    bool crusader{false};
    bool redeemer{false};
    bool hunter{false};
    bool warlord{false};

    // Other boolean flags
    bool transiguredGem{false};
    bool vaalGem{false};
    bool crucible{false};
    bool fractured{false};
    bool synthesised{false};
    bool searingExarch{false};
    bool eaterOfWorlds{false};
    bool identified{false};
    bool corrupted{false};
    bool mirrored{false};
    bool split{false};
    bool crafted{false};
    bool veiled{false};
    bool forseeing{false};

    struct Column
    {
        QString name;
        std::function<QVariant(const ItemData &)> getter;
    };

    static const std::vector<Column> Columns;
    static const int ColumnCount;

private:
    // These helpers save boilerplate.
    WeaponData &weapon();
    ArmourData &armour();
    SocketData &sockets();
    HeistData &heist();

    static void loadSockets(const poe::Item &item, ItemData &i);
    static void loadProperties(const poe::Item &item, ItemData &i);
    static void loadRequirements(const poe::Item &item, ItemData &i);

    static std::vector<Column> createColumnsInfo();

    static QStringList formatProperties(const poe::Item &item);
    static QString formatRequirements(const poe::Item &item);
    static QStringList getMods(const std::optional<std::vector<QString>> mods);

    static float average(const QString &ranged_value, bool *ok);

};
