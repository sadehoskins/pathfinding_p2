#ifndef RAYLIBSTARTER_ACCESSORYITEM_H
#define RAYLIBSTARTER_ACCESSORYITEM_H

#include "ItemBase.h"

// ******************** ACCESSORY ITEM CLASS ********************

class AccessoryItem : public ItemBase {
public:
    // Special effect types for accessories (helper enum)
    enum class SpecialEffectType {
        MAGIC_RESISTANCE,
        CRITICAL_CHANCE,
        NONE
    };

    // Constructor
    AccessoryItem(const std::string& name, const std::string& description,
                  float weight, int value, ItemRarity rarity,
                  int strength_bonus, SpecialEffectType effect_type, float effect_value);

    // Destructor
    virtual ~AccessoryItem() = default;

    // Override virtual methods from ItemBase
    bool IsStackable() const override { return false; }
    int GetStackLimit() const override { return 1; }
    std::string GetTypeDescription() const override { return "Accessory"; }
    void Use() override;

    // Accessory-specific getters
    int GetStrengthBonus() const { return strength_bonus_; }
    SpecialEffectType GetSpecialEffectType() const { return effect_type_; }
    float GetSpecialEffectValue() const { return effect_value_; }
    std::string GetSpecialEffectName() const;

private:
    int strength_bonus_;
    SpecialEffectType effect_type_;
    float effect_value_;
};


#endif //RAYLIBSTARTER_ACCESSORYITEM_H
