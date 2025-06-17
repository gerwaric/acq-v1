// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <poe/item.h>
#include <poe/itemjeweldata.h>
#include <poe/passivenode.h>

#include <QString>

#include <optional>
#include <unordered_map>
#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-Character

    struct Character
    {
        struct Passives
        {
            std::vector<unsigned> hashes;    // array of uint
            std::vector<unsigned> hashes_ex; // array of uint
            std::unordered_map<QString, int>
                mastery_effects; // dictionary of int the key is the string value of the mastery node skill hash and the value is the selected effect hash
            std::unordered_map<QString, poe::PassiveNode>
                skill_overrides; // dictionary of PassiveNode the key is the string value of the node identifier being replaced
            std::optional<QString> bandit_choice; // ? string one of Kraityn, Alira, Oak, or Eramir
            std::optional<QString>
                pantheon_major; // ? string one of TheBrineKing, Arakaali, Solaris, or Lunaris
            std::optional<QString>
                pantheon_minor; // ? string one of Abberath, Gruthkul, Yugul, Shakari, Tukohama, Ralakesh, Garukhan, or Ryslatha
            std::unordered_map<QString, poe::ItemJewelData>
                jewel_data; // dictionary of ItemJewelData the key is the string value of the x property of an item from the jewels array in this request
            std::optional<QString> alternate_ascendancy; // ? string Warden, Warlock, or Primalist
        };

        struct Metadata
        {
            std::optional<QString> version; // ? string game version for the character's realm
        };

        inline bool operator<(const Character &other) const { return name < other.name; };

        QString id;                    // string a unique 64 digit hexadecimal string
        QString name;                  // string
        QString realm;                 // string pc, xbox, or sony
        QString class_;                // string
        std::optional<QString> league; // ? string
        unsigned level{0};             // uint
        unsigned experience{0};        // uint
        std::optional<bool> ruthless;  // ? bool always true if present
        std::optional<bool> expired;   // ? bool always true if present
        std::optional<bool> deleted;   // ? bool always true if present
        std::optional<bool> current;   // ? bool always true if present
        std::optional<std::vector<poe::Item>> equipment; // ? array of Item
        std::optional<std::vector<poe::Item>> inventory; // ? array of Item
        std::optional<std::vector<poe::Item>>
            rucksack; // ? array of Item items stored in the Primalist's Rucksack
        std::optional<std::vector<poe::Item>> jewels;     // ? array of Item
        std::optional<poe::Character::Passives> passives; // ? object
        std::optional<poe::Character::Metadata> metadata; // ? object
    };

    struct CharacterListWrapper
    {
        std::vector<poe::Character> characters;
    };

    struct CharacterWrapper
    {
        std::optional<poe::Character> character;
    };

} // namespace poe
