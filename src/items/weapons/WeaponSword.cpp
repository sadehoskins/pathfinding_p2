#include "WeaponSword.h"

// ******************** WEAPON SWORD IMPLEMENTATION ********************

WeaponSword::WeaponSword()
        : WeaponItem("Iron Sword",
                     "A sturdy iron sword with a sharp blade that enhances the wielder's strength.",
                     2.5f,                          // Weight: 2.5 kg
                     150,                          // Value: 150 kitty coins
                     ItemRarity::RARE,             // Rarity: Rare
                     25,                           // Damage: 25
                     3) {                          // **NEW** Strength bonus: +3
    // Implementation: 25 damage, +3 Strength bonus
}