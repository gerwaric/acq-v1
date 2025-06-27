// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <poe/types/item.h>
#include <poe/types/itemjeweldata.h>
#include <poe/types/passivenode.h>

#include <glaze/glaze.hpp>

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

template<>
struct glz::meta<poe::Character>
{
    // clang-format off
    using T = poe::Character;
    static constexpr auto value = glz::object(
        "id",         &T::id,
        "name",       &T::name, // string
        "realm",      &T::realm, // string pc, xbox, or sony
        "class",      &T::class_, // string
        "league",     &T::league, // ? string
        "level",      &T::level, // uint
        "experience", &T::experience, // uint
        "ruthless",   &T::ruthless, // ? bool always true if present
        "expired",    &T::expired, // ? bool always true if present
        "deleted",    &T::deleted, // ? bool always true if present
        "current",    &T::current, // ? bool always true if present
        "equipment",  &T::equipment, // ? array of Item
        "inventory",  &T::inventory, // ? array of Item
        "rucksack",   &T::rucksack, // ? array of Item items stored in the Primalist's Rucksack
        "jewels",     &T::jewels, // ? array of Item
        "passives",   &T::passives, // ? object
        "metadata",   &T::metadata // ? object
    );
    // clang-format on
};
