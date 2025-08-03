#include "ConsumablesBomb.h"

// ******************** CONSUMABLES BOMB IMPLEMENTATION ********************

ConsumablesBomb::ConsumablesBomb()
        : ConsumableItem("Explosive Bomb", "A dangerous bomb that deals area damage.",
                         1.2f, 75, ItemRarity::UNCOMMON,
                         EffectType::DAMAGE, 100) {
}