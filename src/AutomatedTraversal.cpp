#include "AutomatedTraversal.h"
#include "items/base-classes/WeaponItem.h"
#include "items/base-classes/ArmorItem.h"
#include "items/base-classes/AccessoryItem.h"
#include "inventory/EquipmentSlot.h"
#include <iostream>
#include <iomanip>

// ******************** CONSTRUCTOR & DESTRUCTOR ********************

AutomatedTraversal::AutomatedTraversal()
        : is_active_(false)
        , is_complete_(false)
        , is_moving_(false)
        , show_path_visualization_(true)
        , current_step_(0)
        , target_position_(0, 0)
        , player_character_(nullptr)
        , game_map_(nullptr)
        , pathfinding_system_(nullptr)
        , movement_timer_(0.0f)
        , movement_delay_(0.8f)  // 0.8 seconds between moves
        , status_message_("Ready for automated traversal")
        , items_picked_up_(0)
        , items_equipped_(0)
        , total_items_found_(0) {

    std::cout << "Automated Traversal System initialized." << std::endl;
}

AutomatedTraversal::~AutomatedTraversal() {
    Stop();
}

// ******************** MAIN TRAVERSAL METHODS ********************

bool AutomatedTraversal::StartAutomatedTraversal(PlayerChar* player, Map<>* game_map, Pathfinding* pathfinder) {
    if (!player || !game_map || !pathfinder) {
        std::cout << "Error: Invalid parameters for automated traversal!" << std::endl;
        return false;
    }

    // Store references
    player_character_ = player;
    game_map_ = game_map;
    pathfinding_system_ = pathfinder;

    // Calculate path from current player position to end
    Position start = player_character_->GetPosition();
    Position goal = game_map_->GetEndPosition();

    std::cout << "\n STARTING AUTOMATED TRAVERSAL " << std::endl;
    std::cout << "Calculating optimal path from (" << start.x << ", " << start.y
              << ") to (" << goal.x << ", " << goal.y << ")..." << std::endl;

    PathResult result = pathfinding_system_->FindPathAStar(start, goal, *game_map_);

    if (!result.path_found) {
        std::cout << "Cannot find path to destination! Automated traversal failed." << std::endl;
        status_message_ = "No path to destination";
        return false;
    }

    // Store the calculated path
    calculated_path_ = result.path;
    current_step_ = 0;

    // Initialize state
    is_active_ = true;
    is_complete_ = false;
    is_moving_ = true;
    movement_timer_ = 0.0f;
    items_picked_up_ = 0;
    items_equipped_ = 0;
    total_items_found_ = 0;

    std::cout << "Path calculated successfully!" << std::endl;
    std::cout << "Path length: " << calculated_path_.size() << " steps" << std::endl;
    std::cout << "Path cost: " << result.total_cost << std::endl;
    std::cout << "Nodes explored: " << result.nodes_explored << std::endl;
    std::cout << "⏱Movement delay: " << movement_delay_ << " seconds per step" << std::endl;
    std::cout << "Starting automated movement..." << std::endl;

    UpdateStatusMessage();
    return true;
}

void AutomatedTraversal::Update() {
    if (!is_active_ || is_complete_ || !is_moving_) {
        return;
    }

    // Update movement timer
    movement_timer_ += GetFrameTime();

    if (movement_timer_ >= movement_delay_) {
        movement_timer_ = 0.0f;
        ProcessCurrentStep();
    }
}

void AutomatedTraversal::Stop() {
    if (is_active_) {
        std::cout << "\n Automated traversal stopped by user." << std::endl;
    }

    is_active_ = false;
    is_complete_ = false;
    is_moving_ = false;
    calculated_path_.clear();
    current_step_ = 0;
    status_message_ = "Traversal stopped";
}

// ******************** PRIVATE PROCESSING METHODS ********************

void AutomatedTraversal::ProcessCurrentStep() {
    if (current_step_ >= static_cast<int>(calculated_path_.size())) {
        CompleteTraversal();
        return;
    }

    Position next_position = calculated_path_[current_step_];

    std::cout << "Step " << (current_step_ + 1) << "/" << calculated_path_.size()
              << ": Moving to (" << next_position.x << ", " << next_position.y << ")" << std::endl;

    // Move player to next position
    player_character_->SetPosition(next_position);

    // Check for items at this position
    HandleItemPickup(next_position);

    // Check if we should auto-equip any items
    HandleAutoEquipment();

    // Move to next step
    current_step_++;
    UpdateStatusMessage();

    // Check if end reached is reached
    if (current_step_ >= static_cast<int>(calculated_path_.size())) {
        CompleteTraversal();
    }
}

void AutomatedTraversal::HandleItemPickup(const Position& pos) {
    if (!game_map_->HasItemsAt(pos)) {
        return;
    }

    // Get all items at this position
    auto items = game_map_->GetItemManager().GetItemsAtPosition(pos);

    for (const auto* item_with_pos : items) {
        if (!item_with_pos->is_in_treasure_chest) {
            std::cout << "[PICKUP] Found hidden item: " << item_with_pos->item->GetName()
                      << " (weight: " << item_with_pos->item->GetWeight() << "kg)" << std::endl;

            // Use PlayerChar's pickup method to avoid duplicates
            bool picked_up = player_character_->PickUpItemAt(pos);

            if (picked_up) {
                items_picked_up_++;
                total_items_found_++;
                std::cout << "[SUCCESS] Successfully picked up item!" << std::endl;
            } else {
                std::cout << "[WARNING] Could not pick up item (inventory full or insufficient strength)" << std::endl;
            }

            break; // Only pick up one item per step to avoid duplicates
        }
    }

    // Handle treasure chests
    if (game_map_->HasTreasureChestAt(pos)) {
        const Tile& tile = game_map_->GetTile(pos);
        if (tile.IsClosedTreasureChest()) {
            std::cout << "[TREASURE] Found treasure chest at (" << pos.x << ", " << pos.y << ")!" << std::endl;

            // Use PlayerChar's pickup method for consistency
            bool opened = player_character_->PickUpItemAt(pos);
            if (opened) {
                items_picked_up_++;
                total_items_found_++;
                std::cout << "[SUCCESS] Opened treasure chest!" << std::endl;
            }
        }
    }
}

void AutomatedTraversal::HandleAutoEquipment() {
    if (!player_character_) {
        return;
    }

    auto* inventory = player_character_->GetInventory();
    if (!inventory) {
        std::cout << "[ERROR] Player inventory is null!" << std::endl;
        return;
    }

    // Check all inventory slots for equipable items
    for (int slot = 0; slot < inventory->GetMaxSlots(); ++slot) {
        const ItemBase* item = inventory->GetItem(slot);
        if (!item) continue;

        // Determine what type of item this is
        EquipmentSlotType target_slot;
        bool is_equipable = false;

        if (dynamic_cast<const WeaponItem*>(item)) {
            target_slot = EquipmentSlotType::WEAPON;
            is_equipable = true;
        } else if (dynamic_cast<const ArmorItem*>(item)) {
            target_slot = EquipmentSlotType::ARMOR;
            is_equipable = true;
        } else if (dynamic_cast<const AccessoryItem*>(item)) {
            target_slot = EquipmentSlotType::ACCESSORY;
            is_equipable = true;
        }

        if (!is_equipable) continue;

        // Check if this item is better than currently equipped
        const ItemBase* current_equipped = inventory->GetEquippedItem(target_slot);

        if (ShouldAutoEquipItem(item, current_equipped)) {
            if (inventory->EquipItem(slot, target_slot)) {
                items_equipped_++;

                int str_bonus = 0;
                if (const WeaponItem* weapon = dynamic_cast<const WeaponItem*>(item)) {
                    str_bonus = weapon->GetStrengthBonus();
                } else if (const ArmorItem* armor = dynamic_cast<const ArmorItem*>(item)) {
                    str_bonus = armor->GetStrengthBonus();
                } else if (const AccessoryItem* accessory = dynamic_cast<const AccessoryItem*>(item)) {
                    str_bonus = accessory->GetStrengthBonus();
                }

                std::cout << "[EQUIP] Auto-equipped " << GetEquipmentSlotName(target_slot)
                          << ": " << item->GetName() << " (+" << str_bonus << " STR)" << std::endl;

                break; // Only equip one item per step to avoid chaos
            }
        }
    }
}

bool AutomatedTraversal::ShouldAutoEquipItem(const ItemBase* new_item, const ItemBase* current_item) const {
    if (!new_item) return false;

    // If no current item, always equip
    if (!current_item) return true;

    // Compare strength bonuses
    int new_strength = 0;
    int current_strength = 0;

    if (const WeaponItem* weapon = dynamic_cast<const WeaponItem*>(new_item)) {
        new_strength = weapon->GetStrengthBonus();
    } else if (const ArmorItem* armor = dynamic_cast<const ArmorItem*>(new_item)) {
        new_strength = armor->GetStrengthBonus();
    } else if (const AccessoryItem* accessory = dynamic_cast<const AccessoryItem*>(new_item)) {
        new_strength = accessory->GetStrengthBonus();
    }

    if (const WeaponItem* weapon = dynamic_cast<const WeaponItem*>(current_item)) {
        current_strength = weapon->GetStrengthBonus();
    } else if (const ArmorItem* armor = dynamic_cast<const ArmorItem*>(current_item)) {
        current_strength = armor->GetStrengthBonus();
    } else if (const AccessoryItem* accessory = dynamic_cast<const AccessoryItem*>(current_item)) {
        current_strength = accessory->GetStrengthBonus();
    }

    // Auto-equip if new item has better strength bonus
    return new_strength > current_strength;
}

void AutomatedTraversal::UpdateStatusMessage() {
    if (is_complete_) {
        status_message_ = "Journey Complete!";
    } else if (is_active_) {
        float progress = GetProgress();
        status_message_ = "Automated Travel: " + std::to_string(static_cast<int>(progress * 100)) + "% complete";
    } else {
        status_message_ = "Ready for automated traversal";
    }
}

void AutomatedTraversal::CompleteTraversal() {
    is_complete_ = true;
    is_moving_ = false;

    std::cout << "\n AUTOMATED TRAVERSAL COMPLETE!" << std::endl;
    std::cout << "Successfully reached the destination!" << std::endl;

    UpdateStatusMessage();
    ShowFinalSummary();
}

// ******************** PATH VISUALIZATION ********************

void AutomatedTraversal::RenderPathVisualization(int offset_x, int offset_y, int tile_size) const {
    if (!show_path_visualization_ || calculated_path_.empty()) {
        return;
    }

    // Render path steps
    for (int i = 0; i < static_cast<int>(calculated_path_.size()); ++i) {
        const Position& pos = calculated_path_[i];
        int x = offset_x + (pos.x * tile_size);
        int y = offset_y + (pos.y * tile_size);

        bool is_current = (i == current_step_);
        RenderPathStep(x, y, tile_size, i, is_current);
    }

    // Draw progress indicator
    if (is_active_ && !calculated_path_.empty()) {
        std::string progress_text = "Path Progress: " + std::to_string(current_step_) + "/" + std::to_string(calculated_path_.size());
        DrawText(progress_text.c_str(), offset_x, offset_y - 25, 16, WHITE);

        // Draw progress bar
        int bar_width = 200;
        int bar_height = 8;
        float progress = GetProgress();

        DrawRectangle(offset_x, offset_y - 40, bar_width, bar_height, DARKGRAY);
        DrawRectangle(offset_x, offset_y - 40, static_cast<int>(bar_width * progress), bar_height, GREEN);
        DrawRectangleLines(offset_x, offset_y - 40, bar_width, bar_height, WHITE);
    }
}

void AutomatedTraversal::RenderPathStep(int x, int y, int tile_size, int step_index, bool is_current) const {
    Color path_color = GetPathColor(step_index);

    if (is_current) {
        // Highlight current step with pulsing effect
        float pulse = (sin(GetTime() * 4.0f) + 1.0f) * 0.5f;
        Color pulse_color = ColorAlpha(YELLOW, 0.5f + pulse * 0.5f);
        DrawRectangle(x + 2, y + 2, tile_size - 4, tile_size - 4, pulse_color);
        DrawRectangleLines(x + 1, y + 1, tile_size - 2, tile_size - 2, YELLOW);
    } else if (step_index < current_step_) {
        // Completed steps - faded
        DrawRectangle(x + 4, y + 4, tile_size - 8, tile_size - 8, ColorAlpha(GREEN, 0.3f));
    } else {
        // Future steps - dotted line effect
        DrawRectangle(x + 6, y + 6, tile_size - 12, tile_size - 12, ColorAlpha(path_color, 0.4f));
    }
}

Color AutomatedTraversal::GetPathColor(int step_index) const {
    if (step_index == 0) return GREEN;  // Start
    if (step_index == static_cast<int>(calculated_path_.size()) - 1) return RED;  // End
    return SKYBLUE;  // Regular path
}

// ******************** UTILITY METHODS ********************

float AutomatedTraversal::GetProgress() const {
    if (calculated_path_.empty()) return 0.0f;
    return static_cast<float>(current_step_) / static_cast<float>(calculated_path_.size());
}

void AutomatedTraversal::ShowFinalSummary() const {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "                    JOURNEY COMPLETE!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    std::cout << "RAVERSAL STATISTICS:" << std::endl;
    std::cout << "  • Total steps taken: " << calculated_path_.size() << std::endl;
    std::cout << "  • Items found: " << total_items_found_ << std::endl;
    std::cout << "  • Items picked up: " << items_picked_up_ << std::endl;
    std::cout << "  • Items auto-equipped: " << items_equipped_ << std::endl;

    if (player_character_) {
        std::cout << "\nPLAYER FINAL STATUS:" << std::endl;
        std::cout << "  • Final position: (" << player_character_->GetPosition().x
                  << ", " << player_character_->GetPosition().y << ")" << std::endl;
        std::cout << "  • Total strength: " << player_character_->GetTotalStrength() << std::endl;
        std::cout << "  • Current weight: " << std::fixed << std::setprecision(1)
                  << player_character_->GetCurrentWeight() << "/"
                  << player_character_->GetMaxCarryWeight() << " kg" << std::endl;

        // Show equipment summary using available methods
        auto* inventory = player_character_->GetInventory();
        std::cout << "\n⚔FINAL EQUIPMENT:" << std::endl;
        std::cout << "  • Total equipment strength bonus: +" << inventory->GetTotalStrengthBonus() << std::endl;

        std::cout << "\nFINAL INVENTORY STATUS:" << std::endl;
        std::cout << "  • Slots used: " << inventory->GetUsedSlots()
                  << "/" << inventory->GetMaxSlots() << std::endl;

        // Show inventory contents
        std::cout << "\nFINAL INVENTORY CONTENTS:" << std::endl;
        bool has_items = false;
        for (int i = 0; i < inventory->GetMaxSlots(); ++i) {
            const ItemBase* item = inventory->GetItem(i);
            if (item) {
                has_items = true;
                std::cout << "  • Slot " << i << ": " << item->GetName()
                          << " (" << item->GetTypeDescription()
                          << ", " << item->GetRarityName() << ")" << std::endl;
            }
        }
        if (!has_items) {
            std::cout << "  • No items in inventory" << std::endl;
        }
    }

    std::cout << "\nMISSION STATUS: SUCCESS!" << std::endl;
    std::cout << "The automated traversal system successfully guided the player" << std::endl;
    std::cout << "from start to end, collecting items and optimizing equipment!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    // Suggest final actions
    std::cout << "\nTIP: Press '1', '2', '3', or '4' to sort your final inventory!" << std::endl;
    std::cout << "TIP: Press 'I' to view your complete inventory in detail!" << std::endl;
}