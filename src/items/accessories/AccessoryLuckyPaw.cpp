#include "AccessoryLuckyPaw.h"

// ******************** ACCESSORY LUCKY PAW IMPLEMENTATION ********************

AccessoryLuckyPaw::AccessoryLuckyPaw()
        : AccessoryItem("Lucky Paw",
                        "A mystical paw charm that brings good fortune and magical protection.",
                        0.3f,                                           // Weight: 0.3 kg
                        90,                                            // Value: 90 kitty coins
                        ItemRarity::UNCOMMON,                          // Rarity: Uncommon
                        1,                                             // Strength bonus: +1
                        SpecialEffectType::MAGIC_RESISTANCE,          // Effect: Magic Resistance
                        25.0f) {                                       // Effect value: 25 points
    // Implementation: +1 Strength, +magic resistance
}