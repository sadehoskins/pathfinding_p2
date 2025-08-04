#include "AccessoryItem.h"
#include <iostream>

// ******************** ACCESSORY ITEM IMPLEMENTATION ********************

AccessoryItem::AccessoryItem(const std::string& name, const std::string& description,
                             float weight, int value, ItemRarity rarity,
                             int strength_bonus, SpecialEffectType effect_type, float effect_value)
        : ItemBase(name, description, weight, value, rarity),
          strength_bonus_(strength_bonus), effect_type_(effect_type), effect_value_(effect_value) {
    // Constructor initialization
}

void AccessoryItem::Use() {
    std::cout << "Accessory items are equipped for special effects, not consumed!" << std::endl;
    std::cout << name_ << " provides +" << strength_bonus_ << " strength";

    if (effect_type_ != SpecialEffectType::NONE) {
        std::cout << " and " << GetSpecialEffectName() << ": " << effect_value_;
        if (effect_type_ == SpecialEffectType::CRITICAL_CHANCE) {
            std::cout << "%";
        }
    }
    std::cout << std::endl;
}

std::string AccessoryItem::GetSpecialEffectName() const {
    switch (effect_type_) {
        case SpecialEffectType::MAGIC_RESISTANCE:
            return "Magic Resistance";
        case SpecialEffectType::CRITICAL_CHANCE:
            return "Critical Chance";
        case SpecialEffectType::NONE:
        default:
            return "No Special Effect";
    }
}