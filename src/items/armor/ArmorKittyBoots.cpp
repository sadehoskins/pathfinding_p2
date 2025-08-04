#include "ArmorKittyBoots.h"

// ******************** ARMOR KITTY BOOTS IMPLEMENTATION ********************

ArmorKittyBoots::ArmorKittyBoots()
        : ArmorItem("Kitty Boots",
                    "Adorable boots with tiny paw prints that boost your strength and provide protection.",
                    1.2f,                           // Weight: 1.2 kg
                    80,                            // Value: 80 kitty coins
                    ItemRarity::UNCOMMON,          // Rarity: Uncommon
                    2,                             // Strength bonus: +2
                    0.05f) {                       // Damage reduction: +5%
    // Implementation: +2 Strength, +5% damage reduction
}

