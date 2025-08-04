#include "ItemManager.h"
#include "raylib.h"

#include <iostream>
#include <algorithm>
#include <chrono>

// ******************** STATIC MEMBER DEFINITIONS ********************

std::mt19937 ItemManager::random_generator_(std::chrono::steady_clock::now().time_since_epoch().count());
std::vector<Position> ItemManager::treasure_chest_positions_;

// ******************** CONSTRUCTOR & DESTRUCTOR ********************

ItemManager::ItemManager() {
    // Initialize random generator with current time
    random_generator_.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

ItemManager::~ItemManager() {
    ClearAllItems();
}

// ******************** ITEM GENERATION ********************

void ItemManager::GenerateItemsForMap(int map_width, int map_height, int min_items) {
    ClearAllItems();
    treasure_chest_positions_.clear();

    // Calculate total items to generate (5-8 items for 15x15 map)
    int total_items = std::max(min_items, GetRandomValue(min_items, min_items + 3));

    // Determine how many items go in treasure chests (30-40% of total)
    int chest_items = std::max(1, static_cast<int>(total_items * 0.35f));
    int hidden_items = total_items - chest_items;

    std::cout << "Generating " << total_items << " items (" << chest_items
              << " in chests, " << hidden_items << " hidden)" << std::endl;

    // Generate treasure chest items (visible)
    for (int i = 0; i < chest_items; ++i) {
        Position chest_pos = GetRandomTraversablePosition(map_width, map_height);

        // Avoid first and last rows (start/end positions)
        while (chest_pos.y <= 0 || chest_pos.y >= map_height - 1) {
            chest_pos = GetRandomTraversablePosition(map_width, map_height);
        }

        // Create item with higher chance for rare items in chests
        std::unique_ptr<ItemBase> item;
        float rare_chance = GetRandomValue(0, 100);
        if (rare_chance < 30.0f) {
            item = CreateRandomItem(ItemRarity::RARE);
        } else if (rare_chance < 60.0f) {
            item = CreateRandomItem(ItemRarity::UNCOMMON);
        } else {
            item = CreateRandomItemByWeight();
        }

        items_.emplace_back(chest_pos, std::move(item), true);
        treasure_chest_positions_.push_back(chest_pos);
    }

    // Generate hidden items (scattered on traversable tiles)
    for (int i = 0; i < hidden_items; ++i) {
        Position item_pos = GetRandomTraversablePosition(map_width, map_height);

        // Ensure not on treasure chest positions
        while (std::find(treasure_chest_positions_.begin(), treasure_chest_positions_.end(), item_pos)
               != treasure_chest_positions_.end()) {
            item_pos = GetRandomTraversablePosition(map_width, map_height);
        }

        // Create item using weighted random generation
        std::unique_ptr<ItemBase> item = CreateRandomItemByWeight();
        items_.emplace_back(item_pos, std::move(item), false);
    }

    // Ensure at least one kitty coin per map
    bool has_currency = false;
    for (const auto& item_with_pos : items_) {
        if (dynamic_cast<CurrencyKittyCoin*>(item_with_pos.item.get()) != nullptr) {
            has_currency = true;
            break;
        }
    }

    if (!has_currency) {
        Position currency_pos = GetRandomTraversablePosition(map_width, map_height);
        auto currency = std::make_unique<CurrencyKittyCoin>(GetRandomValue(5, 15));
        items_.emplace_back(currency_pos, std::move(currency), false);
        std::cout << "Added guaranteed kitty coins to map!" << std::endl;
    }

    PrintItemsInfo();
}

void ItemManager::ClearAllItems() {
    items_.clear();
    treasure_chest_positions_.clear();
}

// ******************** ITEM ACCESS ********************

std::vector<ItemManager::ItemWithPosition*> ItemManager::GetItemsAtPosition(const Position& pos) {
    std::vector<ItemWithPosition*> result;
    for (auto& item_with_pos : items_) {
        if (item_with_pos.position == pos) {
            result.push_back(&item_with_pos);
        }
    }
    return result;
}

ItemManager::ItemWithPosition* ItemManager::GetTreasureChestItem(const Position& pos) {
    for (auto& item_with_pos : items_) {
        if (item_with_pos.position == pos && item_with_pos.is_in_treasure_chest) {
            return &item_with_pos;
        }
    }
    return nullptr;
}

// ******************** ITEM REMOVAL ********************

bool ItemManager::RemoveItemAtPosition(const Position& pos, bool from_treasure_chest) {
    auto it = std::find_if(items_.begin(), items_.end(),
                           [&pos, from_treasure_chest](const ItemWithPosition& item_with_pos) {
                               return item_with_pos.position == pos &&
                                      item_with_pos.is_in_treasure_chest == from_treasure_chest;
                           });

    if (it != items_.end()) {
        items_.erase(it);
        return true;
    }
    return false;
}

std::unique_ptr<ItemBase> ItemManager::TakeItemAtPosition(const Position& pos, bool from_treasure_chest) {
    auto it = std::find_if(items_.begin(), items_.end(),
                           [&pos, from_treasure_chest](const ItemWithPosition& item_with_pos) {
                               return item_with_pos.position == pos &&
                                      item_with_pos.is_in_treasure_chest == from_treasure_chest;
                           });

    if (it != items_.end()) {
        std::unique_ptr<ItemBase> item = std::move(it->item);
        items_.erase(it);
        return item;
    }
    return nullptr;
}

// ******************** UTILITY ********************

int ItemManager::GetItemCountAtPosition(const Position& pos) const {
    int count = 0;
    for (const auto& item_with_pos : items_) {
        if (item_with_pos.position == pos) {
            count++;
        }
    }
    return count;
}

void ItemManager::PrintItemsInfo() const {
    std::cout << "\n=== GENERATED ITEMS ===" << std::endl;
    std::cout << "Total items: " << items_.size() << std::endl;

    int chest_count = 0, hidden_count = 0;
    for (const auto& item_with_pos : items_) {
        if (item_with_pos.is_in_treasure_chest) {
            chest_count++;
        } else {
            hidden_count++;
        }

        std::cout << "- " << item_with_pos.item->GetName()
                  << " (" << item_with_pos.item->GetRarityName() << ") "
                  << "at (" << item_with_pos.position.x << "," << item_with_pos.position.y << ") "
                  << (item_with_pos.is_in_treasure_chest ? "[CHEST]" : "[HIDDEN]")
                  << std::endl;
    }

    std::cout << "Chest items: " << chest_count << ", Hidden items: " << hidden_count << std::endl;
    std::cout << "=======================" << std::endl;
}

// ******************** STATIC ITEM CREATION ********************

std::unique_ptr<ItemBase> ItemManager::CreateRandomItem(ItemRarity rarity) {
    std::vector<std::string> item_types;

    switch (rarity) {
        case ItemRarity::COMMON:
            item_types = {"KittyCoin"};
            break;
        case ItemRarity::UNCOMMON:
            item_types = {"HealthPotion", "Bomb", "KittyBoots", "LuckyPaw"};
            break;
        case ItemRarity::RARE:
            item_types = {"Sword", "Staff", "BlueGemstone", "ClawNecklace"};
            break;
        case ItemRarity::LEGENDARY:
            item_types = {"ElderWings", "Staff"}; // For now, weapons can be legendary
            break;
    }

    if (item_types.empty()) {
        return std::make_unique<CurrencyKittyCoin>(1);
    }

    std::string chosen_type = item_types[GetRandomValue(0, item_types.size() - 1)];
    return CreateSpecificItem(chosen_type, rarity);
}

std::unique_ptr<ItemBase> ItemManager::CreateRandomItemByWeight() {
    ItemRarity rarity = GetRandomRarity();
    return CreateRandomItem(rarity);
}

// ******************** PRIVATE HELPERS ********************

ItemRarity ItemManager::GetRandomRarity() {
    float random_weight = static_cast<float>(GetRandomValue(0, 10000)) / 100.0f; // 0-100 range

    if (random_weight < RarityWeights::COMMON_WEIGHT) {
        return ItemRarity::COMMON;
    } else if (random_weight < RarityWeights::COMMON_WEIGHT + RarityWeights::UNCOMMON_WEIGHT) {
        return ItemRarity::UNCOMMON;
    } else if (random_weight < RarityWeights::COMMON_WEIGHT + RarityWeights::UNCOMMON_WEIGHT + RarityWeights::RARE_WEIGHT) {
        return ItemRarity::RARE;
    } else {
        return ItemRarity::LEGENDARY;
    }
}

std::unique_ptr<ItemBase> ItemManager::CreateSpecificItem(const std::string& item_type, ItemRarity rarity) {
    if (item_type == "KittyCoin") {
        int amount = GetRandomValue(3, 12);
        return std::make_unique<CurrencyKittyCoin>(amount);
    } else if (item_type == "HealthPotion") {
        return std::make_unique<ConsumablesHealthPotion>();
    } else if (item_type == "Bomb") {
        return std::make_unique<ConsumablesBomb>();
    } else if (item_type == "Sword") {
        return std::make_unique<WeaponSword>();
    } else if (item_type == "Staff") {
        return std::make_unique<WeaponStaff>();
    } else if (item_type == "BlueGemstone") {
        return std::make_unique<GemstoneBlue>();
    } else if (item_type == "KittyBoots") {
        return std::make_unique<ArmorKittyBoots>();
    } else if (item_type == "ElderWings") {
        return std::make_unique<ArmorElderWings>();
    } else if (item_type == "LuckyPaw") {
        return std::make_unique<AccessoryLuckyPaw>();
    } else if (item_type == "ClawNecklace") {
        return std::make_unique<AccessoryClawNecklace>();
    }

    // Fallback
    return std::make_unique<CurrencyKittyCoin>(1);
}

Position ItemManager::GetRandomTraversablePosition(int map_width, int map_height) const {
    Position pos;
    pos.x = GetRandomValue(0, map_width - 1);
    pos.y = GetRandomValue(1, map_height - 2); // Avoid first and last rows
    return pos;
}

bool ItemManager::IsValidItemPosition(const Position& pos, int map_width, int map_height) const {
    return pos.x >= 0 && pos.x < map_width &&
           pos.y > 0 && pos.y < map_height - 1; // Avoid start/end rows
}