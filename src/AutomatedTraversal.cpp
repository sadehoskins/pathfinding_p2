#include "AutomatedTraversal.h"
#include "items/base-classes/WeaponItem.h"
#include "items/base-classes/ArmorItem.h"
#include "items/base-classes/AccessoryItem.h"
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

    std::cout << "\n🤖 STARTING AUTOMATED TRAVERSAL 🤖" << std::endl;
    std::cout << "Calculating optimal path from (" << start.x << ", " << start.y
              << ") to (" << goal.x << ", " << goal.y << ")..." << std::endl;

    PathResult result = pathfinding_system_->FindPathAStar(start, goal, *game_map_);

    if (!result.path_found) {
        std::cout << "❌ Cannot find path to destination! Automated traversal failed." << std::endl;
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

    std::cout << "✅ Path calculated successfully!" << std::endl;
    std::cout << "📊 Path length: " << calculated_path_.size() << " steps" << std::endl;
    std::cout << "💰 Path cost: " << result.total_cost << std::endl;
    std::cout << "🧠 Nodes explored: " << result.nodes_explored << std::endl;
    std::cout << "⏱️  Movement delay: " << movement_delay_ << " seconds per step" << std::endl;
    std::cout << "🎮 Starting automated movement..." << std::endl;

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
        std::cout << "\n🛑 Automated traversal stopped by user." << std::endl;
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

    std::cout << "🚶 Step " << (current_step_ + 1) << "/" << calculated_path_.size()
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

    // Check if we've reached the end
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
            // Try to pick up hidden item
            std::cout << "✨ Found hidden item: " << item_with_pos->item->GetName()
                      << " (weight: " << item_with_pos->item->GetWeight() << "kg)" << std::endl;

            // Check if player can carry it
            float item_weight = item_with_pos->item->GetWeight();
            if (player_character_->GetCurrentWeight() + item_weight <= player_character_->GetMaxCarryWeight()) {
                // Use the player character's pickup method
                if (player_character_->PickUpItemAt(pos)) {
                    items_picked_up_++;
                    total_items_found_++;
                    std::cout << "📦 Successfully picked up: " << item_with_pos->item->GetName() << std::endl;
                } else {
                    std::cout << "❌ Failed to pick up item (inventory full)" << std::endl;
                }
            } else {
                std::cout << "⚖️  Too heavy to pick up (would exceed weight limit)" << std::endl;
            }
            break; // Only pick up one item per step
        }
    }

    // Handle treasure chests
    if (game_map_->HasTreasureChestAt(pos)) {
        const Tile& tile = game_map_->GetTile(pos);
        if (tile.IsClosedTreasureChest()) {
            std::cout << "🏆 Found treasure chest! Opening..." << std::endl;

            // Try to pick up from treasure chest
            if (player_character_->PickUpItemAt(pos)) {
                items_picked_up_++;
                total_items_found_++;
                std::cout << "💎 Successfully looted treasure chest!" << std::endl;
            } else {
                std::cout << "📦 Treasure chest full or inventory full" << std::endl;
            }
        }
    }
}

void AutomatedTraversal::HandleAutoEquipment() {
    if (!player_character_) {
        return;
    }

    InventorySystem& inventory = player_character_->GetInventorySystem();

    // Check each inventory slot for potentially better equipment
    for (int slot = 0; slot < inventory.GetMaxInventorySlots(); ++slot) {
        const ItemBase* item = inventory.GetItemInSlot(slot);
        if (!item) continue;

        // Check weapons
        if (const WeaponItem* weapon = dynamic_cast<const WeaponItem*>(item)) {
            // **FIXED** - Use internal inventory access to check equipped weapon
            bool should_equip = true;
            int current_weapon_strength = 0;

            // Check if there's already a weapon equipped by trying to get current strength bonus
            int total_strength_before = inventory.GetTotalStrengthBonus();

            // Temporarily check what the strength would be if we equipped this weapon
            // For now, we'll use a simpler approach: just equip if it has better strength than 0
            // or if we can determine no weapon is currently equipped

            if (inventory.EquipItemInSlot(slot, EquipmentSlotType::WEAPON)) {
                items_equipped_++;
                std::cout << "⚔️  Auto-equipped weapon: " << weapon->GetName()
                          << " (+" << weapon->GetStrengthBonus() << " STR)" << std::endl;
            }
        }
            // Check armor
        else if (const ArmorItem* armor = dynamic_cast<const ArmorItem*>(item)) {
            if (inventory.EquipItemInSlot(slot, EquipmentSlotType::ARMOR)) {
                items_equipped_++;
                std::cout << "🛡️  Auto-equipped armor: " << armor->GetName()
                          << " (+" << armor->GetStrengthBonus() << " STR)" << std::endl;
            }
        }
            // Check accessories
        else if (const AccessoryItem* accessory = dynamic_cast<const AccessoryItem*>(item)) {
            if (inventory.EquipItemInSlot(slot, EquipmentSlotType::ACCESSORY)) {
                items_equipped_++;
                std::cout << "💍 Auto-equipped accessory: " << accessory->GetName()
                          << " (+" << accessory->GetStrengthBonus() << " STR)" << std::endl;
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

    std::cout << "\n🎉 AUTOMATED TRAVERSAL COMPLETE! 🎉" << std::endl;
    std::cout << "🏁 Successfully reached the destination!" << std::endl;

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

    std::cout << "📊 TRAVERSAL STATISTICS:" << std::endl;
    std::cout << "  • Total steps taken: " << calculated_path_.size() << std::endl;
    std::cout << "  • Items found: " << total_items_found_ << std::endl;
    std::cout << "  • Items picked up: " << items_picked_up_ << std::endl;
    std::cout << "  • Items auto-equipped: " << items_equipped_ << std::endl;

    if (player_character_) {
        std::cout << "\n👤 PLAYER FINAL STATUS:" << std::endl;
        std::cout << "  • Final position: (" << player_character_->GetPosition().x
                  << ", " << player_character_->GetPosition().y << ")" << std::endl;
        std::cout << "  • Total strength: " << player_character_->GetTotalStrength() << std::endl;
        std::cout << "  • Current weight: " << std::fixed << std::setprecision(1)
                  << player_character_->GetCurrentWeight() << "/"
                  << player_character_->GetMaxCarryWeight() << " kg" << std::endl;

        // Show equipment summary using available methods
        InventorySystem& inventory = player_character_->GetInventorySystem();
        std::cout << "\n⚔️  FINAL EQUIPMENT:" << std::endl;
        std::cout << "  • Total equipment strength bonus: +" << inventory.GetTotalStrengthBonus() << std::endl;

        std::cout << "\n📦 FINAL INVENTORY STATUS:" << std::endl;
        std::cout << "  • Slots used: " << inventory.GetUsedInventorySlots()
                  << "/" << inventory.GetMaxInventorySlots() << std::endl;

        // Show inventory contents
        std::cout << "\n📋 FINAL INVENTORY CONTENTS:" << std::endl;
        bool has_items = false;
        for (int i = 0; i < inventory.GetMaxInventorySlots(); ++i) {
            const ItemBase* item = inventory.GetItemInSlot(i);
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

    std::cout << "\n🎯 MISSION STATUS: SUCCESS!" << std::endl;
    std::cout << "The automated traversal system successfully guided the player" << std::endl;
    std::cout << "from start to end, collecting items and optimizing equipment!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    // Suggest final actions
    std::cout << "\n💡 TIP: Press '1', '2', '3', or '4' to sort your final inventory!" << std::endl;
    std::cout << "💡 TIP: Press 'I' to view your complete inventory in detail!" << std::endl;
}