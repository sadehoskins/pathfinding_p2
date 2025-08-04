#include "AccessoryClawNecklace.h"

// ******************** ACCESSORY CLAW NECKLACE IMPLEMENTATION ********************

AccessoryClawNecklace::AccessoryClawNecklace()
        : AccessoryItem("Claw Necklace",
                        "A fierce necklace made from sharp claws that increases critical strikes.",
                        0.5f,                                           // Weight: 0.5 kg
                        120,                                           // Value: 120 kitty coins
                        ItemRarity::RARE,                              // Rarity: Rare
                        2,                                             // Strength bonus: +2
                        SpecialEffectType::CRITICAL_CHANCE,           // Effect: Critical Chance
                        5.0f) {                                        // Effect value: 5%
    // Implementation: +2 Strength, +5% critical chance
}
