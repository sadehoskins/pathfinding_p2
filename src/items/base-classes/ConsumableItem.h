#ifndef RAYLIBSTARTER_CONSUMABLEITEM_H
#define RAYLIBSTARTER_CONSUMABLEITEM_H

#include "ItemBase.h"

// ******************** CONSUMABLE ITEM CLASS ********************

class ConsumableItem : public ItemBase {
public:
    // Effect type enum (helper enum)
    enum class EffectType {
        HEALING,
        DAMAGE,
        BUFF,
        DEBUFF
    };

    ConsumableItem(const std::string& name, const std::string& description,
                   float weight, int value, ItemRarity rarity,
                   EffectType effect_type, int effect_value);

    // Override virtual methods
    bool IsStackable() const override { return true; }
    int GetStackLimit() const override { return 99; }
    std::string GetTypeDescription() const override { return "Consumable"; }
    void Use() override; // Apply effect and consume

    // Consumable-specific getters
    EffectType GetEffectType() const { return effect_type_; }
    int GetEffectValue() const { return effect_value_; }
    std::string GetEffectName() const;

private:
    EffectType effect_type_;
    int effect_value_;
};

#endif //RAYLIBSTARTER_CONSUMABLEITEM_H