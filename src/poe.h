// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "util/spdlog_qt.h"

#include <optional>
#include <unordered_map>
#include <vector>

#include <QObject>
#include <QString>

namespace poe {

    Q_NAMESPACE

    enum class DisplayMode : unsigned {
        NameFirst = 0,      // Name should be followed by values
        ValuesFirst = 1,    // Values should be followed by name
        ProgressBar = 2,    // Progress bar
        InsertedValues = 3, // Values should be inserted into the string by index
        Separator = 4       // Separator
    };
    Q_ENUM_NS(DisplayMode)

    enum class FrameType : unsigned {
        Normal = 0,
        Magic = 1,
        Rare = 2,
        Unique = 3,
        Gem = 4,
        Currency = 5,
        DivinationCard = 6,
        Quest = 7,
        Prophecy = 8,
        Foil = 9,
        SupporterFoil = 10,
        Necropolis = 11
    };
    Q_ENUM_NS(FrameType)

    enum class ItemPropertyType : unsigned {
        MapTier = 1,
        ItemQuantity = 2,
        ItemRarity = 3,
        MonsterPackSize = 4,
        Level = 5,
        Quality = 6,
        PhysicalDamage = 9,
        ElementalDamage = 10,
        ChaosDamage = 11,
        CriticalStrikeChance = 12,
        AttacksPerSecond = 13,
        WeaponRange = 14,
        ChanceToBlock = 15,
        Armour = 16,
        EvasionRating = 17,
        EnergyShield = 18,
        BeastiaryGenus = 21,
        BeastiaryGroup = 22,
        BeastieryFamily = 23,
        Radius = 24,
        StackSize = 32,
        AreaLevel = 34,
        WingsRevealed = 35,
        EscapeRoutesRevealed = 36,
        RewardRoomsRevealed = 37,
        RequiredLockingLevel = 38,
        RequiredBruteForceLevel = 39,
        RequiredPerceptionLevel = 40,
        RequiredDemolutionLevel = 41,
        RequiredCounterThaumaturgyLevel = 42,
        RequiredTrapDisarmamentLevel = 43,
        RequiredAgilityLevel = 44,
        RequiredDeceptionLevel = 45,
        RequiredEngineeringLevel = 46,
        HeistTarget = 47,
        UltimatumSacrifice = 51,
        UltimatumReward = 52,
        Ward = 54,
        RequiredClass = 57,
        RequiredLevel = 62,
        RequiredStrength = 63,
        RequiredDexterity = 64,
        RequiredIntelligence = 65,
        ValdoMapReward = 76,
        ValdoMapConvert = 77,
        ValdoShaperReward = 80,    // What is this?
        ValdoElderReward = 81,     // What is this?
        ValdoConquerorReward = 82, // What is this?
        ValdoUniqueReward = 83,    // What is this?
        ValdoScarabReward = 84,    // What is this?
        Limit = 87,
        MoreMaps = 88,
        MoreScarabs = 89
    };
    Q_ENUM_NS(ItemPropertyType)

    struct CrucibleNode
    {
        std::optional<unsigned> skill;                    // ? uint	mod hash
        std::optional<unsigned> tier;                     // ? uint	mod tier
        std::optional<QString> icon;                      // ? string
        std::optional<bool> allocated;                    // ? bool	always true if present
        std::optional<bool> isNotable;                    // ? bool	always true if present
        std::optional<bool> isReward;                     // ? bool	always true if present
        std::optional<std::vector<QString>> stats;        // ? array of string	stat descriptions
        std::optional<std::vector<QString>> reminderText; // ? array of string
        std::optional<unsigned> orbit;                    // ? uint	the column this node occupies
        std::optional<unsigned> orbitIndex; // ? uint	the node's position within the column
        std::vector<QString> out; //	array of string	node identifiers of nodes this one connects to
        std::vector<QString> in;  //	array of string	node identifiers of nodes connected to this one
    };

    struct ItemProperty
    {
        QString name;                                 // string
        std::vector<std::tuple<QString, int>> values; // array of array
        //std::optional<poe::DisplayMode> displayMode;  // ? uint as DisplayMode
        //std::optional<double> progress;               // ? double rounded to 2 decimal places
        std::optional<poe::ItemPropertyType> type; // ? uint
        //std::optional<QString> suffix;                // ? string
    };

    struct ItemSocket
    {
        unsigned group;                 // uint
        std::optional<QString> attr;    // ? string S, D, I, G, A, or DV
        std::optional<QString> sColour; // ? string R, G, B, W, A, or DV
    };

    struct Item
    {
        struct Influences
        {
            std::optional<bool> shaper;
            std::optional<bool> elder;
            std::optional<bool> crusader;
            std::optional<bool> redeemer;
            std::optional<bool> hunter;
            std::optional<bool> warlord;
        };

        struct Rewards
        {
            QString label; // string
            std::unordered_map<QString, QString>
                rewards; // dictionary of int the key is a string representing the type of reward.The value is the amount
        };

        struct LogbookFaction
        {
            QString id;   // string Faction1, Faction2, Faction3, or Faction4
            QString name; // string
        };

        struct LogbookMods
        {
            QString name;                      // string area name
            poe::Item::LogbookFaction faction; // object
            std::vector<QString> mods;         // array of string
        };

        struct UltimatumMods
        {
            QString type;  // string text used to display ultimatum icons
            unsigned tier; // uint
        };

        struct IncubatingInfo
        {
            QString name;      // string
            unsigned level;    // uint monster level required to progress
            unsigned progress; // uint
            unsigned total;    // uint
        };

        struct ScourgedInfo
        {
            unsigned tier;                    // uint 1 - 3 for items, 1 - 10 for maps
            std::optional<unsigned> level;    // ? uint monster level required to progress
            std::optional<unsigned> progress; // ? uint
            std::optional<unsigned> total;    // ? uint
        };

        struct CrucibleInfo
        {
            // TODO: WARNING: crucible nodes are usually a dictionary, but I've seen
            // them as an array as well.
            QString layout; // string URL to an image of the tree layout
            std::unordered_map<QString, CrucibleNode>
                nodes; // dictionary of CrucibleNode the key is the string value of the node index
        };

        struct HybridInfo
        {
            std::optional<bool> isVaalGem;                            // ? bool
            QString baseTypeName;                                     // string
            std::optional<std::vector<poe::ItemProperty>> properties; // ? array of ItemProperty
            std::optional<std::vector<QString>> explicitMods;         // ? array of string
            std::optional<QString> secDescrText;                      // ? string
        };

        struct ExtendedInfo
        {
            std::optional<QString> category;                   // ? string
            std::optional<std::vector<QString>> subcategories; // ? array of string
            std::optional<unsigned> prefixes;                  // ? uint
            std::optional<unsigned> suffixes;                  // ? uint
        };

        inline bool operator<(const Item &other) const
        {
            const QString a = name.isEmpty() ? typeLine : name + " " + typeLine;
            const QString b = other.name.isEmpty() ? other.typeLine
                                                   : other.name + " " + other.typeLine;
            return a < b;
        };

        //bool verified;                   // bool
        unsigned w;                      // uint
        unsigned h;                      // uint
        QString icon;                    // string
        std::optional<bool> support;     // ? bool always true if present
        std::optional<int> stackSize;    // ? int
        std::optional<int> maxStackSize; // ? int
        //std::optional<QString> stackSizeText; // ? string
        std::optional<QString> league; // ? string
        std::optional<QString> id;     // ? string a unique 64 digit hexadecimal string
        std::optional<poe::Item::Influences> influences;     // ? object
        std::optional<bool> elder;                           // ? bool always true if present
        std::optional<bool> shaper;                          // ? bool always true if present
        std::optional<bool> searing;                         // ? bool always true if present
        std::optional<bool> tangled;                         // ? bool always true if present
        std::optional<bool> abyssJewel;                      // ? bool always true if present
        std::optional<bool> delve;                           // ? bool always true if present
        std::optional<bool> fractured;                       // ? bool always true if present
        std::optional<bool> synthesised;                     // ? bool always true if present
        std::optional<std::vector<poe::ItemSocket>> sockets; // ? array of ItemSocket
        std::optional<std::vector<poe::Item>> socketedItems; // ? array of Item
        QString name;                                        // string
        QString typeLine;                                    // string
        QString baseType;                                    // string
        std::optional<QString> rarity; // ? string Normal, Magic, Rare, or Unique
        bool identified;               // bool
        std::optional<int> itemLevel;  // ? int used for items that always display their item level
        int ilvl;                      // int
        std::optional<QString> note;   // ? string user - generated text
        std::optional<QString> forum_note; // ? string user - generated text
        //std::optional<bool> lockedToCharacter;                    // ? bool always true if present
        //std::optional<bool> lockedToAccount;                      // ? bool always true if present
        std::optional<bool> duplicated;                           // ? bool always true if present
        std::optional<bool> split;                                // ? bool always true if present
        std::optional<bool> corrupted;                            // ? bool always true if present
        std::optional<bool> unmodifiable;                         // ? bool always true if present
        std::optional<bool> cisRaceReward;                        // ? bool always true if present
        std::optional<bool> seaRaceReward;                        // ? bool always true if present
        std::optional<bool> thRaceReward;                         // ? bool always true if present
        std::optional<std::vector<poe::ItemProperty>> properties; // ? array of ItemProperty
        std::optional<std::vector<poe::ItemProperty>> notableProperties; // ? array of ItemProperty
        std::optional<std::vector<poe::ItemProperty>> requirements;      // ? array of ItemProperty
        std::optional<std::vector<poe::ItemProperty>> additionalProperties; // ? array of ItemProperty
        //std::optional<std::vector<poe::ItemProperty>> nextLevelRequirements; // ? array of ItemProperty
        std::optional<int> talismanTier; // ? int
        //std::optional<std::vector<poe::Item::Rewards>> rewards; // ? array of object
        //std::optional<QString> secDescrText;                                 // ? string
        //std::optional<std::vector<QString>> utilityMods;                     // ? array of string
        //std::optional<std::vector<poe::Item::LogbookMods>> logbookMods;      // ? array of object
        std::optional<std::vector<QString>> enchantMods;  // ? array of string
        std::optional<std::vector<QString>> scourgeMods;  // ? array of string
        std::optional<std::vector<QString>> implicitMods; // ? array of string
        //std::optional<std::vector<poe::Item::UltimatumMods>> ultimatumMods; // ? array of object
        std::optional<std::vector<QString>> explicitMods;  // ? array of string
        std::optional<std::vector<QString>> craftedMods;   // ? array of string
        std::optional<std::vector<QString>> fracturedMods; // ? array of string
        std::optional<std::vector<QString>> crucibleMods;  // ? array of string only allocated mods
        std::optional<std::vector<QString>> cosmeticMods;  // ? array of string
        std::optional<std::vector<QString>> veiledMods; // ? array of string random video identifier
        std::optional<bool> veiled;                     // ? bool always true if present
        //std::optional<QString> descrText;               // ? string
        //std::optional<std::vector<QString>> flavourText; // ? array of
        // WARNING: flavourTextParsed might sometimes be an object
        //std::optional<std::vector<QString>> flavourTextParsed; // ? array of string or object
        //std::optional<std::vector<QString>> flavourTextNote;    // ? string user - generated text
        //std::optional<std::vector<QString>> prophecyText;       // ? string
        std::optional<bool> isRelic;      // ? bool always true if present
        std::optional<int> foilVariation; // ? int
        std::optional<bool> replica;      // ? bool always true if present
        std::optional<bool> foreseeing;   // ? bool always true if present
        //std::optional<poe::Item::IncubatingInfo> incubatedItem; // ? object
        //std::optional<poe::Item::ScourgedInfo> scourged;        // ? object
        //std::optional<poe::Item::CrucibleInfo> crucible; // ? object
        //std::optional<bool> ruthless;                           // ? bool always true if present
        poe::FrameType frameType;                    // ? uint as FrameType
        std::optional<QString> artFilename;          // ? string
        std::optional<poe::Item::HybridInfo> hybrid; // ? object
        //std::optional<poe::Item::ExtendedInfo> extended;        // ? object only in Public Stash API
        std::optional<unsigned> x;          // ? uint
        std::optional<unsigned> y;          // ? uint
        std::optional<QString> inventoryId; // ? string
        std::optional<unsigned> socket;     // ? uint
        std::optional<QString> colour;      // ? string S, D, I, or G
    };

    struct Character
    {
        QString id;
        QString name;
        QString realm;
        std::optional<QString> league;
        std::optional<std::vector<poe::Item>> equipment;
        std::optional<std::vector<poe::Item>> inventory;
        std::optional<std::vector<poe::Item>> rucksack;
        std::optional<std::vector<poe::Item>> jewels;
    };

    struct League
    {
        QString id;
        QString name;
        QString realm;
    };

    struct StashTab
    {
        struct MetaData
        {
            std::optional<bool> folder;
            std::optional<QString> colour;
        };

        QString id;
        std::optional<QString> parent;
        std::optional<QString> folder;
        QString name;
        QString type;
        std::optional<unsigned> index;
        poe::StashTab::MetaData metadata;
        std::optional<std::vector<poe::StashTab>> children;
        std::optional<std::vector<poe::Item>> items;
    };

    // Typedefs.

    using LeagueList = std::vector<League>;
    using LeagueListPtr = std::shared_ptr<const LeagueList>;

    using CharacterList = std::vector<Character>;
    using CharacterListPtr = std::shared_ptr<const CharacterList>;
    using CharacterPtr = std::shared_ptr<const Character>;

    using StashList = std::vector<StashTab>;
    using StashListPtr = std::shared_ptr<const StashList>;
    using StashTabPtr = std::shared_ptr<const StashTab>;

    // API wrappers.

    struct LeagueListWrapper
    {
        LeagueList leagues;
    };

    struct CharacterListWrapper
    {
        CharacterList characters;
    };

    struct StashListWrapper
    {
        StashList stashes;
    };

    struct CharacterWrapper
    {
        std::optional<Character> character;
    };

    struct StashTabWrapper
    {
        std::optional<poe::StashTab> stash;
    };

} // namespace poe

// ----- Formatters for spdlog ------------------------------------------------------

namespace fmt {

    template<>
    struct formatter<poe::DisplayMode, char> : QtEnumFormatter<poe::DisplayMode>
    {};

    template<>
    struct formatter<poe::FrameType, char> : QtEnumFormatter<poe::FrameType>
    {};

    template<>
    struct formatter<poe::ItemPropertyType, char> : QtEnumFormatter<poe::ItemPropertyType>
    {};

} // namespace fmt
