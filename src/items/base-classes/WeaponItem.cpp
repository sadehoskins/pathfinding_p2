#include "WeaponItem.h"
#include <iostream>

// ******************** WEAPON ITEM IMPLEMENTATION ********************

WeaponItem::WeaponItem(const std::string& name, const std::string& description,
                       float weight, int value, ItemRarity rarity,
                       int damage, int strength_bonus)
        : ItemBase(name, description, weight, value, rarity),
          damage_(damage), strength_bonus_(strength_bonus) {
    // Constructor implementation
}

void WeaponItem::Use() {
    std::cout << "Equipment items are equipped, not consumed!" << std::endl;
    std::cout << name_ << " deals " << damage_ << " damage and provides +"
              << strength_bonus_ << " strength." << std::endl;
}