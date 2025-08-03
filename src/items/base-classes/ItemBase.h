#ifndef RAYLIBSTARTER_ITEMBASE_H
#define RAYLIBSTARTER_ITEMBASE_H

#include <string>
#include <memory>

// ******************** ITEM RARITY ENUM ********************
// (Helper enum - allowed in same file per project rules)

enum class ItemRarity {
    COMMON,
    UNCOMMON,
    RARE,
    LEGENDARY
};

// ******************** ITEM BASE CLASS ********************

class ItemBase {
public:
    // Constructor
    ItemBase(const std::string& name, const std::string& description,
             float weight, int value, ItemRarity rarity);

    // Virtual destructor for polymorphism
    virtual ~ItemBase() = default;

    // Getters
    const std::string& GetName() const { return name_; }
    const std::string& GetDescription() const { return description_; }
    float GetWeight() const { return weight_; }
    int GetValue() const { return value_; }
    ItemRarity GetRarity() const { return rarity_; }

    // Virtual methods for different item behaviors
    virtual bool IsStackable() const = 0;
    virtual int GetStackLimit() const = 0;
    virtual std::string GetTypeDescription() const = 0;
    virtual void Use() = 0; // For consumables, weapons might override differently

    // Utility methods
    std::string GetRarityName() const;
    static std::string GetRarityName(ItemRarity rarity);

protected:
    std::string name_;
    std::string description_;
    float weight_;        // Must be > 0
    int value_;          // In kitty coins
    ItemRarity rarity_;
};

#endif //RAYLIBSTARTER_ITEMBASE_H
