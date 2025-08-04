#ifndef RAYLIBSTARTER_ITEMMANAGER_H
#define RAYLIBSTARTER_ITEMMANAGER_H

#include "base-classes/ItemBase.h"
#include "weapons/WeaponSword.h"
#include "weapons/WeaponStaff.h"
#include "currency/CurrencyKittyCoin.h"
#include "consumables/ConsumablesHealthPotion.h"
#include "consumables/ConsumablesBomb.h"
#include "collectibles/GemstoneBlue.h"
#include "armor/ArmorKittyBoots.h"
#include "armor/ArmorElderWings.h"
#include "accessories/AccessoryLuckyPaw.h"
#include "accessories/AccessoryClawNecklace.h"
#include "../Tile.h"
#include <vector>
#include <memory>
#include <random>

// ******************** ITEM MANAGER CLASS ********************

class ItemManager {
public:
    // Item with position information
    struct ItemWithPosition {
        Position position;
        std::unique_ptr<ItemBase> item;
        bool is_in_treasure_chest;

        ItemWithPosition(const Position& pos, std::unique_ptr<ItemBase> item_ptr, bool in_chest = false)
                : position(pos), item(std::move(item_ptr)), is_in_treasure_chest(in_chest) {}
    };

    // Constructor
    ItemManager();

    // Destructor
    ~ItemManager();

    // Item generation
    void GenerateItemsForMap(int map_width, int map_height, int min_items = 5);
    void ClearAllItems();

    // Item access
    const std::vector<ItemWithPosition>& GetAllItems() const { return items_; }
    std::vector<ItemWithPosition*> GetItemsAtPosition(const Position& pos);
    ItemWithPosition* GetTreasureChestItem(const Position& pos);

    // Item removal (for pickup)
    bool RemoveItemAtPosition(const Position& pos, bool from_treasure_chest = false);
    std::unique_ptr<ItemBase> TakeItemAtPosition(const Position& pos, bool from_treasure_chest = false);

    // Utility
    int GetItemCountAtPosition(const Position& pos) const;
    int GetTotalItemCount() const { return items_.size(); }
    void PrintItemsInfo() const;

    // Static item creation helpers
    static std::unique_ptr<ItemBase> CreateRandomItem(ItemRarity rarity = ItemRarity::COMMON);
    static std::unique_ptr<ItemBase> CreateRandomItemByWeight();
    static std::vector<Position> GetTreasureChestPositions() { return treasure_chest_positions_; }

    std::vector<ItemWithPosition> items_;
private:
    static std::vector<Position> treasure_chest_positions_;

    // Random generation helpers
    static std::mt19937 random_generator_;
    static ItemRarity GetRandomRarity();
    static std::unique_ptr<ItemBase> CreateSpecificItem(const std::string& item_type, ItemRarity rarity);

    // Item placement helpers
    Position GetRandomTraversablePosition(int map_width, int map_height) const;
    bool IsValidItemPosition(const Position& pos, int map_width, int map_height) const;
};

// ******************** RARITY WEIGHTS ********************

struct RarityWeights {
    static constexpr float COMMON_WEIGHT = 60.0f;      // 60%
    static constexpr float UNCOMMON_WEIGHT = 25.0f;    // 25%
    static constexpr float RARE_WEIGHT = 13.0f;        // 13%
    static constexpr float LEGENDARY_WEIGHT = 2.0f;    // 2%

    static constexpr float TOTAL_WEIGHT = COMMON_WEIGHT + UNCOMMON_WEIGHT +
                                          RARE_WEIGHT + LEGENDARY_WEIGHT;
};

#endif //RAYLIBSTARTER_ITEMMANAGER_H