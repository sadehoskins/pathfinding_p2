#include "CollectibleItem.h"
#include <iostream>

// ******************** COLLECTIBLE ITEM IMPLEMENTATION ********************

CollectibleItem::CollectibleItem(const std::string& name, const std::string& description,
                                 float weight, int value, ItemRarity rarity,
                                 int collection_value)
        : ItemBase(name, description, weight, value, rarity),
          collection_value_(collection_value) {
}

void CollectibleItem::Use() {
    std::cout << "Collectible items are kept for collection value!" << std::endl;
}