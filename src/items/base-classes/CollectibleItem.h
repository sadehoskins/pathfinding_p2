#ifndef RAYLIBSTARTER_COLLECTIBLEITEM_H
#define RAYLIBSTARTER_COLLECTIBLEITEM_H

#include "ItemBase.h"

// ******************** COLLECTIBLE ITEM CLASS ********************

class CollectibleItem : public ItemBase {
public:
    CollectibleItem(const std::string& name, const std::string& description,
                    float weight, int value, ItemRarity rarity,
                    int collection_value);

    // Override virtual methods
    bool IsStackable() const override { return false; }
    int GetStackLimit() const override { return 1; }
    std::string GetTypeDescription() const override { return "Collectible"; }
    void Use() override; // Collectibles are kept, not consumed

    // Collectible-specific getter
    int GetCollectionValue() const { return collection_value_; }

private:
    int collection_value_;
};

#endif //RAYLIBSTARTER_COLLECTIBLEITEM_H