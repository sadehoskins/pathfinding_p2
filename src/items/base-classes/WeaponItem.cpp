#include "WeaponItem.h"
#include <iostream>

// ******************** WEAPON ITEM IMPLEMENTATION ********************

WeaponItem::WeaponItem(const std::string& name, const std::string& description,
                       float weight, int value, ItemRarity rarity,
                       int damage, int strength_bonus)
        : ItemBase(name, description, weight, value, rarity),
          damage_(damage), strength_bonus_(strength_bonus) {
}

void WeaponItem::Use() {
    std::cout << "Equipment items are equipped, not consumed!" << std::endl;
}