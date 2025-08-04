
#include "ArmorItem.h"
#include <iostream>

// ******************** ARMOR ITEM IMPLEMENTATION ********************

ArmorItem::ArmorItem(const std::string& name, const std::string& description,
                     float weight, int value, ItemRarity rarity,
                     int strength_bonus, float damage_reduction)
        : ItemBase(name, description, weight, value, rarity),
          strength_bonus_(strength_bonus), damage_reduction_(damage_reduction) {

    // Ensure damage reduction is within valid range (0-100%)
    if (damage_reduction_ < 0.0f) {
        damage_reduction_ = 0.0f;
    }
    if (damage_reduction_ > 1.0f) {
        damage_reduction_ = 1.0f;
    }
}

void ArmorItem::Use() {
    std::cout << "Armor items are equipped for protection, not consumed!" << std::endl;
    std::cout << name_ << " provides +" << strength_bonus_ << " strength and "
              << (damage_reduction_ * 100.0f) << "% damage reduction." << std::endl;
}