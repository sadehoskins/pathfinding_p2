
#include "ItemBase.h"
#include <iostream>

// ******************** ITEMBASE IMPLEMENTATION ********************

ItemBase::ItemBase(const std::string& name, const std::string& description,
                   float weight, int value, ItemRarity rarity)
        : name_(name), description_(description), weight_(weight),
          value_(value), rarity_(rarity) {
    // Ensure weight is always > 0 as required
    if (weight_ <= 0.0f) {
        weight_ = 0.1f; // Minimum weight
    }
}

std::string ItemBase::GetRarityName() const {
    return GetRarityName(rarity_);
}

std::string ItemBase::GetRarityName(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::COMMON: return "Common";
        case ItemRarity::UNCOMMON: return "Uncommon";
        case ItemRarity::RARE: return "Rare";
        case ItemRarity::LEGENDARY: return "Legendary";
        default: return "Unknown";
    }
}