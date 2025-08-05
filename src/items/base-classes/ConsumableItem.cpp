#include "ConsumableItem.h"
#include <iostream>

// ******************** CONSUMABLE ITEM IMPLEMENTATION ********************

ConsumableItem::ConsumableItem(const std::string& name, const std::string& description,
                               float weight, int value, ItemRarity rarity,
                               EffectType effect_type, int effect_value)
        : ItemBase(name, description, weight, value, rarity),
          effect_type_(effect_type), effect_value_(effect_value) {
}

void ConsumableItem::Use() {
    std::cout << "Using " << name_ << " - " << GetEffectName()
              << " effect: " << effect_value_ << std::endl;
    // Effect application logic for future tasks
}

std::string ConsumableItem::GetEffectName() const {
    switch (effect_type_) {
        case EffectType::HEALING: return "Healing";
        case EffectType::DAMAGE: return "Damage";
        case EffectType::BUFF: return "Buff";
        case EffectType::DEBUFF: return "Debuff";
        default: return "Unknown Effect";
    }
}