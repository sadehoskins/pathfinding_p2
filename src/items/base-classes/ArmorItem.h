//

#ifndef RAYLIBSTARTER_ARMORITEM_H
#define RAYLIBSTARTER_ARMORITEM_H

#include "ItemBase.h"

// ******************** ARMOR ITEM CLASS ********************

class ArmorItem : public ItemBase {
public:
    // Constructor
    ArmorItem(const std::string& name, const std::string& description,
              float weight, int value, ItemRarity rarity,
              int strength_bonus, float damage_reduction);

    // Destructor
    virtual ~ArmorItem() = default;

    // Override virtual methods from ItemBase
    bool IsStackable() const override { return false; }
    int GetStackLimit() const override { return 1; }
    std::string GetTypeDescription() const override { return "Armor"; }
    void Use() override;

    // Armor-specific getters
    int GetStrengthBonus() const { return strength_bonus_; }
    float GetDamageReduction() const { return damage_reduction_; }

private:
    int strength_bonus_;
    float damage_reduction_; // Percentage (0.0 to 1.0)
};

#endif //RAYLIBSTARTER_ARMORITEM_H
