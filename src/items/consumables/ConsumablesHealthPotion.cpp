#include "ConsumablesHealthPotion.h"

// ******************** CONSUMABLE HEALTH POTION IMPLEMENTATION ********************

ConsumablesHealthPotion::ConsumablesHealthPotion()
        : ConsumableItem("Health Potion", "A purple potion that restores health when consumed.",
                         0.5f, 50, ItemRarity::UNCOMMON,
                         EffectType::HEALING, 50) {
}