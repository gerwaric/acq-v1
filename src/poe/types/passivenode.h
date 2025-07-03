// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <optional>
#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-PassiveNode

    struct PassiveNode
    {
        struct Mastery
        {
            unsigned effect;                                  // uint effect hash
            std::vector<QString> stats;                       // array of string stat descriptions
            std::optional<std::vector<QString>> reminderText; // ? array of string
        };

        struct ClusterJewel
        {
            // WARNING: TODO: the proxy and parent fields are supposed to be ?uint, but the api return strings
            std::optional<unsigned> size;   // ? uint
            std::optional<unsigned> index;  // ? uint
            std::optional<QString> proxy;   // ? uint the proxy node identifier
            std::optional<QString> parent;  // ? uint the parent node identifier
        };

        std::optional<QString> skill; // ? uint skill hash TODO: WARNING: supposed to be a ?uint
        std::optional<QString> name;  // ? string
        std::optional<QString> icon;              // ? string
        std::optional<bool> isKeystone;           // ? bool always true if present
        std::optional<bool> isNotable;            // ? bool always true if present
        std::optional<bool> isMastery;            // ? bool always true if present
        std::optional<QString> inactiveIcon;      // ? string inactive mastery image
        std::optional<QString> activeIcon;        // ? string active mastery image
        std::optional<QString> activeEffectImage; // ? string active mastery or tattoo background image
        std::optional<std::vector<poe::PassiveNode::Mastery>> masteryEffects; // ? array of object
        std::optional<bool> isBlighted;    // ? bool always true if present
        std::optional<bool> isTattoo;      // ? bool always true if present
        std::optional<bool> isProxy;       // ? bool always true if present
        std::optional<bool> isJewelSocket; // ? bool always true if present
        std::optional<poe::PassiveNode::ClusterJewel>
            expansionJewel; // ? object cluster jewel information
        std::optional<std::vector<QString>>
            recipe; // ? array of string components required for Blight crafting this node.
        // each string is one of ClearOil, SepiaOil, AmberOil, VerdantOil,
        // TealOil, AzureOil, IndigoOil, VioletOil, CrimsonOil,
        // BlackOil, OpalescentOil, SilverOil, GoldenOil, or PrismaticOil
        std::optional<unsigned> grantedStrength; // ? uint sum of stats on this node that grant strength
        std::optional<unsigned>
            grantedDexterity; // ? uint sum of stats on this node that grant dexterity
        std::optional<unsigned>
            grantedIntelligence; // ? uint sum of stats on this node that grant intelligence
        std::optional<QString> ascendancyName;            // ? string
        std::optional<bool> isAscendancyStart;            // ? bool always true if present
        std::optional<bool> isMultipleChoice;             // ? bool always true if present
        std::optional<bool> isMultipleChoiceOption;       // ? bool always true if present
        std::optional<unsigned> grantedPassivePoints;     // ? uint
        std::optional<std::vector<QString>> stats;        // ? array of string stat descriptions
        std::optional<std::vector<QString>> reminderText; // ? array of string
        std::optional<std::vector<QString>> flavourText;  // ? array of string
        std::optional<unsigned> classStartIndex;          // ? uint
        std::optional<QString> group;       // ? string the key value to look up in the groups table
        std::optional<unsigned> orbit;      // ? uint the orbit this node occupies within it's group
        std::optional<unsigned> orbitIndex; // ? uint the index of this node in the group's orbit
        std::vector<QString> out; // array of string node identifiers of nodes this one connects to
        std::vector<QString> in;  // array of string node identifiers of nodes connected to this one
    };

} // namespace poe
