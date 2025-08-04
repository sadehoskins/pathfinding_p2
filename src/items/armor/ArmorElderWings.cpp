#include "ArmorElderWings.h"

// ******************** ARMOR ELDER WINGS IMPLEMENTATION ********************

ArmorElderWings::ArmorElderWings()
        : ArmorItem("Elder Wings",
                    "Ancient wings that grant both strength and exceptional protection from harm.",
                    2.8f,                           // Weight: 2.8 kg
                    250,                           // Value: 250 kitty coins
                    ItemRarity::RARE,              // Rarity: Rare
                    3,                             // Strength bonus: +3
                    0.15f) {                       // Damage reduction: +15%
    // Implementation: +3 Strength, +15% damage reduction
}