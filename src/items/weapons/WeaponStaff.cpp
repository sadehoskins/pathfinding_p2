#include "WeaponStaff.h"

// ******************** WEAPON STAFF IMPLEMENTATION ********************

WeaponStaff::WeaponStaff()
        : WeaponItem("Wooden Staff",
                     "A mystical wooden staff imbued with magic that strengthens the user.",
                     1.8f,                          // Weight: 1.8 kg
                     120,                          // Value: 120 kitty coins
                     ItemRarity::RARE,             // Rarity: Rare
                     15,                           // Damage: 15
                     2) {                          // **NEW** Strength bonus: +2
    // Implementation: 15 damage, +2 Strength bonus
}