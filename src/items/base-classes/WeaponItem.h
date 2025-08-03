
#ifndef RAYLIBSTARTER_WEAPONITEM_H
#define RAYLIBSTARTER_WEAPONITEM_H

#include "ItemBase.h"

// ******************** WEAPON ITEM CLASS ********************

class WeaponItem : public ItemBase {
public:
    WeaponItem(const std::string& name, const std::string& description,
               float weight, int value, ItemRarity rarity,
               int damage, int strength_bonus);

    // Override virtual methods
    bool IsStackable() const override { return false; }
    int GetStackLimit() const override { return 1; }
    std::string GetTypeDescription() const override { return "Weapon"; }
    void Use() override; // Weapons are equipped, not consumed

    // Weapon-specific getters
    int GetDamage() const { return damage_; }
    int GetStrengthBonus() const { return strength_bonus_; }

private:
    int damage_;
    int strength_bonus_;
};

#endif //RAYLIBSTARTER_WEAPONITEM_H
