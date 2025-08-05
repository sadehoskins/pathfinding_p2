#include "PlayerChar.h"
#include "TextureManager.h"
#include "inventory/EquipmentSlot.h"
#include <iostream>
#include <algorithm>

// ******************** CONSTRUCTOR & DESTRUCTOR ********************

PlayerChar::PlayerChar(const Position& start_position, int base_strength)
        : Character(start_position, CharacterType::PLAYER)  // **UPDATED** - Use Character constructor
        , current_map_(nullptr)
        , inventory_system_(std::make_unique<InventorySystem>())
        , texture_loaded_(false) {

    // Set base strength using Character's system
    base_strength_ = base_strength;

    // Set default player name
    SetName("Player");

    std::cout << "PlayerChar created at position (" << start_position.x << ", " << start_position.y
              << ") with " << base_strength << " base strength." << std::endl;

    LoadTexture();
}

PlayerChar::~PlayerChar() {
    UnloadTexture();
    std::cout << "PlayerChar destroyed." << std::endl;
}

// ******************** INTERFACE FOR TRAVERSING THE MAP  ********************

bool PlayerChar::CanMoveTo(const Position& new_position) const {
    if (!current_map_) {
        std::cout << "Error: No map set for player character!" << std::endl;
        return false;
    }

    // Check if position is valid on the map
    if (!current_map_->IsValidPosition(new_position)) {
        return false;
    }

    // Check if tile is traversable
    const Tile& target_tile = current_map_->GetTile(new_position);
    if (!target_tile.IsTraversable()) {
        return false;
    }

    // Weight restriction -> Check if player is overweight
    if (IsOverweight()) {
        std::cout << "Cannot move - you are carrying too much weight! Current: "
                  << GetCurrentWeight() << "kg, Max: " << GetMaxCarryWeight() << "kg" << std::endl;
        return false;
    }

    return true;
}

void PlayerChar::MoveTo(const Position& new_position) {
    if (CanMoveTo(new_position)) {
        position_ = new_position;
        std::cout << "Player moved to position (" << new_position.x << ", " << new_position.y << ")" << std::endl;
    } else {
        std::cout << "Cannot move to position (" << new_position.x << ", " << new_position.y << ")" << std::endl;
    }
}

// ******************** MOVEMENT METHODS ********************

bool PlayerChar::TryMoveUp() {
    Position new_pos(position_.x, position_.y - 1);
    if (CanMoveTo(new_pos)) {
        MoveTo(new_pos);
        return true;
    }
    return false;
}

bool PlayerChar::TryMoveDown() {
    Position new_pos(position_.x, position_.y + 1);
    if (CanMoveTo(new_pos)) {
        MoveTo(new_pos);
        return true;
    }
    return false;
}

bool PlayerChar::TryMoveLeft() {
    Position new_pos(position_.x - 1, position_.y);
    if (CanMoveTo(new_pos)) {
        MoveTo(new_pos);
        return true;
    }
    return false;
}

bool PlayerChar::TryMoveRight() {
    Position new_pos(position_.x + 1, position_.y);
    if (CanMoveTo(new_pos)) {
        MoveTo(new_pos);
        return true;
    }
    return false;
}

// ******************** STRENGTH SYSTEM  ********************

int PlayerChar::GetStrength() const {
    int total = base_strength_;  // Use base class member

    // Add equipment bonuses
    if (inventory_system_) {
        total += inventory_system_->GetTotalStrengthBonus();
    }

    return total;
}


float PlayerChar::GetMaxCarryWeight() const {
    // Each point of strength allows 2kg of carry weight
    return GetTotalStrength() * 2.0f;
}

float PlayerChar::GetCurrentWeight() const {
    if (!inventory_system_) {
        return 0.0f;
    }

    return inventory_system_->GetCurrentWeight();
}

bool PlayerChar::IsOverweight() const {
    return GetCurrentWeight() > GetMaxCarryWeight();
}

// ******************** INTERFACE FOR PICKING UP AND DROPPING ITEMS ********************

bool PlayerChar::PickUpItemAt(const Position& pos) {
    if (!current_map_) {
        std::cout << "Error: No map set for player character!" << std::endl;
        return false;
    }

    // Check if player is at the specified position
    if (position_ != pos) {
        std::cout << "Cannot pick up item - not at that position!" << std::endl;
        return false;
    }

    // Try to get item from treasure chest first
    if (current_map_->HasTreasureChestAt(pos)) {
        const Tile& tile = current_map_->GetTile(pos);
        if (tile.IsClosedTreasureChest()) {
            // Use inventory system to open treasure chest
            bool success = inventory_system_->OpenTreasureChest(pos, current_map_->GetItemManager());
            if (success) {
                current_map_->OpenTreasureChestAt(pos);
                std::cout << "Opened treasure chest and picked up item!" << std::endl;
                return true;
            } else {
                std::cout << "Could not pick up item from treasure chest." << std::endl;
                return false;
            }
        }
    }

    // Try to pick up hidden items at this position
    auto item = current_map_->GetItemManager().TakeItemAtPosition(pos, false);
    if (item) {
        // Check weight before picking up
        float item_weight = item->GetWeight();
        if (GetCurrentWeight() + item_weight > GetMaxCarryWeight()) {
            std::cout << "Cannot pick up " << item->GetName()
                      << " - would exceed weight limit! (" << item_weight << "kg)" << std::endl;
            // Put item back
            current_map_->GetItemManager().items_.emplace_back(pos, std::move(item), false);
            return false;
        }

        std::string item_name = item->GetName();
        if (inventory_system_->AddItemToInventory(std::move(item))) {
            std::cout << "Picked up " << item_name << "!" << std::endl;
            return true;
        } else {
            std::cout << "Inventory full! Cannot pick up " << item_name << std::endl;
            return false;
        }
    }

    std::cout << "No items to pick up at this location." << std::endl;
    return false;
}

bool PlayerChar::DropSelectedItem() {
    if (!inventory_system_) {
        return false;
    }

    // Find first item to drop
    for (int i = 0; i < inventory_system_->GetMaxInventorySlots(); ++i) {
        const ItemBase* item = inventory_system_->GetItemInSlot(i);
        if (item) {
            std::string item_name = item->GetName();
            // In a real implementation, you'd place the item on the map at player position
            std::cout << "Dropped " << item_name << " at position ("
                      << position_.x << ", " << position_.y << ")" << std::endl;
            return true;
        }
    }

    std::cout << "No items to drop!" << std::endl;
    return false;
}

// ******************** INTERFACE FOR EQUIPPING ITEMS ********************

bool PlayerChar::EquipSelectedItem(EquipmentSlotType slot_type) {
    if (!inventory_system_) {
        return false;
    }

    // Find first compatible item in inventory
    for (int i = 0; i < inventory_system_->GetMaxInventorySlots(); ++i) {
        const ItemBase* item = inventory_system_->GetItemInSlot(i);
        if (item) {
            // Check if item can be equipped in this slot
            bool can_equip = false;
            switch (slot_type) {
                case EquipmentSlotType::WEAPON:
                    can_equip = dynamic_cast<const WeaponItem*>(item) != nullptr;
                    break;
                case EquipmentSlotType::ARMOR:
                    can_equip = dynamic_cast<const ArmorItem*>(item) != nullptr;
                    break;
                case EquipmentSlotType::ACCESSORY:
                    can_equip = dynamic_cast<const AccessoryItem*>(item) != nullptr;
                    break;
            }

            if (can_equip) {
                if (inventory_system_->EquipItemInSlot(i, slot_type)) {
                    std::cout << "Equipped " << item->GetName() << "!" << std::endl;
                    UpdateStrengthFromEquipment();
                    return true;
                }
            }
        }
    }

    std::cout << "No compatible items found to equip!" << std::endl;
    return false;
}

bool PlayerChar::UnequipItem(EquipmentSlotType slot_type) {
    if (!inventory_system_) {
        return false;
    }

    if (inventory_system_->UnequipEquipmentSlot(slot_type)) {
        std::cout << "Unequipped item from " << GetEquipmentSlotName(slot_type) << " slot!" << std::endl;
        UpdateStrengthFromEquipment();
        return true;
    }

    std::cout << "No item equipped in " << GetEquipmentSlotName(slot_type) << " slot!" << std::endl;
    return false;
}

// ******************** RENDERING ********************

void PlayerChar::Render(int screen_x, int screen_y, int tile_size) const {
    if (texture_loaded_ && character_texture_.id != 0) {
        // Draw character texture
        Rectangle source = {0, 0, (float)character_texture_.width, (float)character_texture_.height};
        Rectangle dest = {(float)screen_x, (float)screen_y, (float)tile_size, (float)tile_size};
        DrawTexturePro(character_texture_, source, dest, {0, 0}, 0.0f, WHITE);

        // Draw border to make character stand out
        DrawRectangleLines(screen_x, screen_y, tile_size, tile_size, BLACK);
    } else {
        // Fallback rendering -> draw colored rectangle with '@' symbol
        DrawRectangle(screen_x, screen_y, tile_size, tile_size, YELLOW);
        DrawRectangleLines(screen_x, screen_y, tile_size, tile_size, BLACK);

        // Draw '@' character in center
        const char* player_char = "@";
        int char_width = MeasureText(player_char, tile_size / 2);
        DrawText(player_char,
                 screen_x + (tile_size - char_width) / 2,
                 screen_y + tile_size / 4,
                 tile_size / 2,
                 BLACK);
    }

    // Show overweight indicator if carrying too much
    if (IsOverweight()) {
        DrawRectangle(screen_x + tile_size - 8, screen_y, 8, 8, RED);
        DrawText("!", screen_x + tile_size - 6, screen_y + 1, 6, WHITE);
    }
}

void PlayerChar::LoadTexture() {
    if (TextureManager::AreTexturesLoaded()) {
        character_texture_ = TextureManager::GetCharacterTexture("player");
        if (character_texture_.id != 0) {
            texture_loaded_ = true;
            std::cout << "Player character texture loaded successfully." << std::endl;
        } else {
            std::cout << "Warning: Player character texture not found, using fallback rendering." << std::endl;
        }
    } else {
        std::cout << "Textures not loaded yet, player will use fallback rendering." << std::endl;
    }
}

void PlayerChar::UnloadTexture() {
    // TextureManager handles all texture unloading
    texture_loaded_ = false;
}

// ******************** UTILITY ********************
// Enhanced PrintStatus method that combines character info and player-specific info

void PlayerChar::PrintStatus() const {
    // Call base class status first
    Character::PrintStatus();

    // Add player-specific information
    std::cout << "\n=== PLAYER-SPECIFIC INFO ===" << std::endl;
    std::cout << "Total Strength: " << GetStrength() << " (Base: " << base_strength_
              << " + Equipment: " << (GetStrength() - base_strength_) << ")" << std::endl;
    std::cout << "Weight: " << GetCurrentWeight() << "/" << GetMaxCarryWeight() << " kg";
    if (IsOverweight()) {
        std::cout << " **OVERWEIGHT!**";
    }
    std::cout << std::endl;

    if (inventory_system_) {
        std::cout << "Inventory: " << inventory_system_->GetUsedInventorySlots()
                  << "/" << inventory_system_->GetMaxInventorySlots() << " slots used" << std::endl;
        int strength_bonus = inventory_system_->GetTotalStrengthBonus();
        if (strength_bonus > 0) {
            std::cout << "Equipment Strength Bonus: +" << strength_bonus << std::endl;
        }
    }
    std::cout << "===============================" << std::endl;
}

void PlayerChar::Update() {
    // Player specific update logic
    // Update inventory system
    if (inventory_system_) {
        inventory_system_->Update();
    }

    // Check if player is overweight and update status
    if (IsOverweight()) {
        // Add visual indicators, sound effects, etc. here in the future
    }

    // Future player-specific updates go here:
    // - Health regeneration over time
    // - Status effect updates (poison, buffs, etc.)
    // - Experience/leveling systems
    // - Achievement tracking
    // - Auto-save triggers

    // Call base class update for any shared character logic
    Character::Update();
}

// ******************** PRIVATE HELPER METHODS ********************

bool PlayerChar::IsValidPosition(const Position& pos) const {
    if (!current_map_) {
        return false;
    }
    return current_map_->IsValidPosition(pos);
}

void PlayerChar::UpdateStrengthFromEquipment() {
    // Strength is automatically calculated in GetTotalStrength()
    // This method is called after equipment changes to trigger any necessary updates

    std::cout << "Strength updated! Total strength: " << GetTotalStrength()
              << " (Base: " << base_strength_ << " + Equipment: "
              << (GetTotalStrength() - base_strength_) << ")" << std::endl;

    // Check if player is now overweight due to strength changes
    if (IsOverweight()) {
        std::cout << "Warning: You are now overweight! Drop some items or increase strength." << std::endl;
    }
}

// ******************** ITEM DETECTION METHOD ********************

void PlayerChar::CheckItemsAtCurrentPosition() const {
    if (!current_map_) {
        return;
    }

    // Check for treasure chests
    if (current_map_->HasTreasureChestAt(position_)) {
        const Tile& tile = current_map_->GetTile(position_);
        if (tile.IsClosedTreasureChest()) {
            std::cout << "There is a closed treasure chest here! Press SPACE to open it." << std::endl;
        } else if (tile.IsOpenTreasureChest()) {
            std::cout << "There is an empty opened treasure chest here." << std::endl;
        }
    }

    // Check for hidden items
    auto items = current_map_->GetItemManager().GetItemsAtPosition(position_);
    if (!items.empty()) {
        std::cout << "Items found at this location:" << std::endl;
        for (const auto* item_with_pos : items) {
            if (!item_with_pos->is_in_treasure_chest) {
                std::cout << "- " << item_with_pos->item->GetName()
                          << " (" << item_with_pos->item->GetRarityName()
                          << ") - Press F to pick up" << std::endl;
            }
        }
    }
}
