
#include "InventorySystem.h"

#include <iostream>

// ******************** CONSTRUCTOR & DESTRUCTOR ********************

InventorySystem::InventorySystem()
        : player_inventory_(std::make_unique<Inventory<std::vector>>(10)),
          is_inventory_open_(false),
          is_equip_mode_(false),
          selected_slot_(0),
          status_message_(""),
          status_message_timer_(0.0f) {

    std::cout << "InventorySystem initialized with 10 item slots." << std::endl;
    SetStatusMessage("Inventory System Ready!", 2.0f);
}

InventorySystem::~InventorySystem() {
    std::cout << "InventorySystem destroyed." << std::endl;
}

// ******************** CORE SYSTEM METHODS ********************

void InventorySystem::HandleInput() {
    if (!is_inventory_open_) {
        // Inventory closed - only check for open key
        if (IsKeyPressed(KEY_I)) {
            OpenInventory();
        }
        return;
    }

    // Inventory is open - handle inventory-specific input
    if (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_ESCAPE)) {
        CloseInventory();
        return;
    }

    // Equipment mode toggle
    if (IsKeyPressed(KEY_E)) {
        is_equip_mode_ = !is_equip_mode_;
        SetStatusMessage(is_equip_mode_ ? "EQUIP MODE: Select item slot, then equipment slot" : "Browse Mode", 2.0f);
        selected_slot_ = 0;
    }

    // Slot navigation
    HandleSlotSelection();

    // Equipment actions
    if (is_equip_mode_) {
        HandleEquipmentActions();
    }

    // Quick actions (non-equip mode)
    if (!is_equip_mode_) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            // Use/examine selected item
            const ItemBase* item = player_inventory_->GetItem(selected_slot_);
            if (item) {
                std::cout << "\n=== ITEM DETAILS ===" << std::endl;
                std::cout << "Name: " << item->GetName() << std::endl;
                std::cout << "Description: " << item->GetDescription() << std::endl;
                std::cout << "Type: " << item->GetTypeDescription() << std::endl;
                std::cout << "Rarity: " << item->GetRarityName() << std::endl;
                std::cout << "Weight: " << item->GetWeight() << " kg" << std::endl;
                std::cout << "Value: " << item->GetValue() << " kitty coins" << std::endl;
                std::cout << "===================" << std::endl;

                SetStatusMessage("Examined: " + item->GetName(), 2.0f);
            } else {
                SetStatusMessage("Empty slot", 1.0f);
            }
        }

        // Drop item
        if (IsKeyPressed(KEY_D)) {
            auto item = player_inventory_->RemoveItem(selected_slot_);
            if (item) {
                SetStatusMessage("Dropped: " + item->GetName(), 2.0f);
                // In a real game, you'd place the item on the ground
            } else {
                SetStatusMessage("No item to drop", 1.0f);
            }
        }
    }
}

void InventorySystem::Update() {
    UpdateStatusMessage();
}

// ******************** MAIN RENDER METHOD ********************

void InventorySystem::Render(int screen_width, int screen_height) {
    if (!is_inventory_open_) {
        // **IMPROVED** Use new minimal UI
        RenderMinimalUI(screen_width, screen_height);
        RenderStatusMessage(screen_width, screen_height);
        return;
    }

    // Render full inventory window (already improved above)
    RenderInventoryWindow(screen_width, screen_height);
    RenderStatusMessage(screen_width, screen_height);
}

// ******************** INVENTORY STATE MANAGEMENT ********************

void InventorySystem::OpenInventory() {
    is_inventory_open_ = true;
    is_equip_mode_ = false;
    selected_slot_ = 0;
    SetStatusMessage("Inventory opened. Press 'E' for equip mode, 'I' or 'ESC' to close", 3.0f);
    std::cout << "Inventory opened." << std::endl;
}

void InventorySystem::CloseInventory() {
    is_inventory_open_ = false;
    is_equip_mode_ = false;
    SetStatusMessage("Inventory closed", 1.0f);
    std::cout << "Inventory closed." << std::endl;
}

void InventorySystem::ToggleInventory() {
    if (is_inventory_open_) {
        CloseInventory();
    } else {
        OpenInventory();
    }
}

// ******************** ITEM MANAGEMENT ********************

bool InventorySystem::AddItemToInventory(std::unique_ptr<ItemBase> item) {
    if (!item) return false;

    bool success = player_inventory_->AddItem(std::move(item));
    if (success) {
        SetStatusMessage("Item added to inventory!", 2.0f);
    } else {
        SetStatusMessage("Inventory is full!", 3.0f);
    }
    return success;
}

bool InventorySystem::OpenTreasureChest(const Position& pos, ItemManager& item_manager) {
    auto item = item_manager.TakeItemAtPosition(pos, true);
    if (!item) {
        SetStatusMessage("No item in chest", 1.5f);
        return false;
    }

    std::string item_name = item->GetName();

    if (player_inventory_->AddItem(std::move(item))) {
        SetStatusMessage("Found: " + item_name + "!", 3.0f);
        std::cout << "Treasure chest item added to inventory: " << item_name << std::endl;
        return true;
    } else {
        // Inventory full - for now, just drop the item back
        // TODO: In Phase 2, show inventory management dialog
        std::cout << "Inventory full! Cannot pick up: " << item_name << std::endl;
        SetStatusMessage("Inventory full! Cannot pick up item.", 4.0f);

        // Put item back for now (in real game, might drop on ground)
        item_manager.items_.emplace_back(pos, std::move(item), true);
        return false;
    }
}

// ******************** EQUIPMENT MANAGEMENT ********************

void InventorySystem::HandleEquipInput() {
    if (!is_equip_mode_) return;

    HandleEquipmentActions();
}

void InventorySystem::ShowEquipMenu() {
    std::cout << "\n=== EQUIPMENT MENU ===" << std::endl;
    std::cout << "1. Weapon Slot: ";
    const ItemBase* weapon = player_inventory_->GetEquippedItem(EquipmentSlotType::WEAPON);
    if (weapon) {
        std::cout << weapon->GetName();
    } else {
        std::cout << "[Empty]";
    }
    std::cout << std::endl;

    std::cout << "2. Armor Slot: ";
    const ItemBase* armor = player_inventory_->GetEquippedItem(EquipmentSlotType::ARMOR);
    if (armor) {
        std::cout << armor->GetName();
    } else {
        std::cout << "[Empty]";
    }
    std::cout << std::endl;

    std::cout << "3. Accessory Slot: ";
    const ItemBase* accessory = player_inventory_->GetEquippedItem(EquipmentSlotType::ACCESSORY);
    if (accessory) {
        std::cout << accessory->GetName();
    } else {
        std::cout << "[Empty]";
    }
    std::cout << std::endl;

    std::cout << "Total Strength Bonus: +" << GetTotalStrengthBonus() << std::endl;
    std::cout << "======================" << std::endl;
}

// ******************** UTILITY ********************

void InventorySystem::PrintInventoryStatus() const {
    player_inventory_->PrintInventory();
    player_inventory_->PrintEquipment();
}

int InventorySystem::GetTotalStrengthBonus() const {
    return player_inventory_->GetTotalStrengthBonus();
}

// ******************** PRIVATE METHODS ********************

void InventorySystem::RenderInventoryWindow(int screen_width, int screen_height) {
    // Calculate window dimensions
    int window_width = 750;
    int window_height = 580;
    int window_x = (screen_width - window_width) / 2;
    int window_y = (screen_height - window_height) / 2;

    // Draw window background
    DrawRectangle(window_x, window_y, window_width, window_height, ColorAlpha(BLACK, 0.9f));
    DrawRectangleLines(window_x, window_y, window_width, window_height, WHITE);

    // Title
    const char* title = "INVENTORY";
    int title_width = MeasureText(title, 28);
    DrawText(title, window_x + (window_width - title_width) / 2, window_y + 15, 28, WHITE);

    // Mode indicator
    const char* mode_text = is_equip_mode_ ? "EQUIP MODE" : "BROWSE MODE";
    DrawText(mode_text, window_x + 15, window_y + 55, 18, is_equip_mode_ ? YELLOW : LIGHTGRAY);

    // Render equipment slots
    RenderEquipmentSlots(window_x + 15, window_y + 85);

    // Render inventory slots
    RenderInventorySlots(window_x + 15, window_y + 230);

    // Controls help
    int help_y = window_y + window_height - 140;

    // **IMPROVED** Better contrast for control text
    DrawRectangle(window_x + 10, help_y - 5, window_width - 20, 130, ColorAlpha(BLACK, 0.8f));
    DrawRectangleLines(window_x + 10, help_y - 5, window_width - 20, 130, GRAY);

    DrawText("CONTROLS:", window_x + 20, help_y + 5, 16, WHITE);

    // **IMPROVED** Better organized and more readable control text
    DrawText("Navigate: Arrow Keys  |  Examine: ENTER  |  Drop: D",
             window_x + 20, help_y + 30, 14, LIGHTGRAY);
    DrawText("Toggle Mode: E  |  Close: I or ESC",
             window_x + 20, help_y + 50, 14, LIGHTGRAY);

    if (is_equip_mode_) {
        // **IMPROVED** Better contrast for equip mode text
        DrawText("EQUIP TO SLOTS:", window_x + 20, help_y + 75, 14, YELLOW);
        DrawText("1 = Weapon  |  2 = Armor  |  3 = Accessory",
                 window_x + 20, help_y + 95, 13, ColorAlpha(YELLOW, 0.9f));
        DrawText("UNEQUIP: U+Q = Weapon  |  U+W = Armor  |  U+E = Accessory",
                 window_x + 20, help_y + 115, 13, ColorAlpha(YELLOW, 0.9f));
    }
}

void InventorySystem::RenderEquipmentSlots(int start_x, int start_y) {
    DrawText("EQUIPMENT:", start_x, start_y, 18, WHITE);

    int slot_size = 80;
    int slot_spacing = 120; // **INCREASED** spacing to prevent overlap

    // Equipment slot names
    const char* slot_names[] = {"WEAPON", "ARMOR", "ACCESSORY"};
    EquipmentSlotType slot_types[] = {EquipmentSlotType::WEAPON, EquipmentSlotType::ARMOR, EquipmentSlotType::ACCESSORY};

    for (int i = 0; i < 3; i++) {
        int slot_x = start_x + (i * slot_spacing);
        int slot_y = start_y + 30;

        // Draw slot background
        Color slot_color = player_inventory_->IsEquipmentSlotOccupied(slot_types[i]) ?
                           ColorAlpha(DARKGREEN, 0.8f) : ColorAlpha(DARKGRAY, 0.6f);
        DrawRectangle(slot_x, slot_y, slot_size, slot_size, slot_color);
        DrawRectangleLines(slot_x, slot_y, slot_size, slot_size, WHITE);

        // **IMPROVED** Draw slot label with better positioning
        int label_width = MeasureText(slot_names[i], 12); // **SMALLER** font to prevent overlap
        DrawText(slot_names[i], slot_x + (slot_size - label_width) / 2, slot_y - 20, 12, WHITE);

        // Draw equipped item
        const ItemBase* equipped = player_inventory_->GetEquippedItem(slot_types[i]);
        if (equipped) {
            // **UPDATED** Use TextureManager directly
            if (TextureManager::AreTexturesLoaded()) {
                Texture2D item_texture = TextureManager::GetItemTexture(equipped->GetName());
                if (item_texture.id != 0) {
                    // Draw texture scaled to fit slot (with small margin)
                    int texture_size = slot_size - 10;
                    int texture_x = slot_x + 5;
                    int texture_y = slot_y + 5;

                    Rectangle source = {0, 0, (float)item_texture.width, (float)item_texture.height};
                    Rectangle dest = {(float)texture_x, (float)texture_y, (float)texture_size, (float)texture_size};
                    DrawTexturePro(item_texture, source, dest, {0, 0}, 0.0f, WHITE);

                    // Draw rarity border around texture
                    Color rarity_color = WHITE;
                    switch (equipped->GetRarity()) {
                        case ItemRarity::COMMON: rarity_color = LIGHTGRAY; break;
                        case ItemRarity::UNCOMMON: rarity_color = GREEN; break;
                        case ItemRarity::RARE: rarity_color = BLUE; break;
                        case ItemRarity::LEGENDARY: rarity_color = GOLD; break;
                    }
                    DrawRectangleLines(texture_x - 1, texture_y - 1, texture_size + 2, texture_size + 2, rarity_color);
                } else {
                    // Fallback to text if texture not found
                    std::string item_name = equipped->GetName();
                    if (item_name.length() > 10) {
                        item_name = item_name.substr(0, 8) + "..";
                    }
                    DrawText(item_name.c_str(), slot_x + 4, slot_y + 8, 10, WHITE);
                }
            } else {
                // Fallback to text if textures not loaded
                std::string item_name = equipped->GetName();
                if (item_name.length() > 10) {
                    item_name = item_name.substr(0, 8) + "..";
                }
                DrawText(item_name.c_str(), slot_x + 4, slot_y + 8, 10, WHITE);
            }

            // Show strength bonus (always show regardless of texture)
            int str_bonus = 0;
            if (const WeaponItem* weapon = dynamic_cast<const WeaponItem*>(equipped)) {
                str_bonus = weapon->GetStrengthBonus();
            } else if (const ArmorItem* armor = dynamic_cast<const ArmorItem*>(equipped)) {
                str_bonus = armor->GetStrengthBonus();
            } else if (const AccessoryItem* accessory = dynamic_cast<const AccessoryItem*>(equipped)) {
                str_bonus = accessory->GetStrengthBonus();
            }

            if (str_bonus > 0) {
                // **IMPROVED** Better positioning for strength bonus
                DrawRectangle(slot_x + 2, slot_y + slot_size - 16, 40, 14, ColorAlpha(BLACK, 0.7f));
                DrawText(TextFormat("+%d STR", str_bonus), slot_x + 4, slot_y + slot_size - 15, 10, GREEN);
            }
        } else {
            // Show empty slot indicator
            DrawText("EMPTY", slot_x + (slot_size - MeasureText("EMPTY", 12)) / 2,
                     slot_y + (slot_size - 12) / 2, 12, GRAY);
        }
    }
}

void InventorySystem::RenderInventorySlots(int start_x, int start_y) {
    DrawText("INVENTORY:", start_x, start_y, 18, WHITE);

    int slot_size = 65;
    int slot_spacing = 75;
    int slots_per_row = 5;

    for (int i = 0; i < 10; i++) {
        int row = i / slots_per_row;
        int col = i % slots_per_row;

        int slot_x = start_x + (col * slot_spacing);
        int slot_y = start_y + 30 + (row * slot_spacing);

        // Highlight selected slot
        Color slot_color = ColorAlpha(DARKGRAY, 0.6f);
        if (i == selected_slot_) {
            slot_color = is_equip_mode_ ? ColorAlpha(GOLD, 0.8f) : ColorAlpha(SKYBLUE, 0.8f);
        }

        const ItemBase* item = player_inventory_->GetItem(i);
        if (item) {
            slot_color = ColorAlpha(GREEN, 0.3f);
            if (i == selected_slot_) {
                slot_color = is_equip_mode_ ? ColorAlpha(GOLD, 0.9f) : ColorAlpha(SKYBLUE, 0.9f);
            }
        }

        // Draw slot
        DrawRectangle(slot_x, slot_y, slot_size, slot_size, slot_color);
        DrawRectangleLines(slot_x, slot_y, slot_size, slot_size, WHITE);

        // Draw slot number
        DrawText(TextFormat("%d", i), slot_x + 4, slot_y + 4, 12, WHITE);

        // Draw item if present
        if (item) {
            // **NEW** Try to render item texture first
            if (TextureManager::AreTexturesLoaded()) {
                Texture2D item_texture = TextureManager::GetItemTexture(item->GetName());
                if (item_texture.id != 0) {
                    // Draw texture scaled to fit slot (with margin for slot number)
                    int texture_size = slot_size - 20;
                    int texture_x = slot_x + 10;
                    int texture_y = slot_y + 18;

                    Rectangle source = {0, 0, (float)item_texture.width, (float)item_texture.height};
                    Rectangle dest = {(float)texture_x, (float)texture_y, (float)texture_size, (float)texture_size};
                    DrawTexturePro(item_texture, source, dest, {0, 0}, 0.0f, WHITE);
                } else {
                    // Fallback to text if texture not found
                    std::string item_name = item->GetName();
                    if (item_name.length() > 8) {
                        item_name = item_name.substr(0, 6) + "..";
                    }
                    DrawText(item_name.c_str(), slot_x + 4, slot_y + 20, 9, WHITE);
                }
            } else {
                // Fallback to text if textures not loaded
                std::string item_name = item->GetName();
                if (item_name.length() > 8) {
                    item_name = item_name.substr(0, 6) + "..";
                }
                DrawText(item_name.c_str(), slot_x + 4, slot_y + 20, 9, WHITE);
            }

            // Draw rarity indicator (always show)
            Color rarity_color = WHITE;
            switch (item->GetRarity()) {
                case ItemRarity::COMMON: rarity_color = LIGHTGRAY; break;
                case ItemRarity::UNCOMMON: rarity_color = GREEN; break;
                case ItemRarity::RARE: rarity_color = BLUE; break;
                case ItemRarity::LEGENDARY: rarity_color = GOLD; break;
            }
            DrawRectangle(slot_x + slot_size - 12, slot_y + 4, 10, 10, rarity_color);

            // Show item type indicator
            char type_char = '?';
            Color type_color = WHITE;
            if (dynamic_cast<const WeaponItem*>(item)) {
                type_char = 'W';
                type_color = RED;
            } else if (dynamic_cast<const ArmorItem*>(item)) {
                type_char = 'A';
                type_color = BLUE;
            } else if (dynamic_cast<const AccessoryItem*>(item)) {
                type_char = 'C';
                type_color = PURPLE;
            } else if (dynamic_cast<const ConsumableItem*>(item)) {
                type_char = 'U';
                type_color = ORANGE;
            } else if (dynamic_cast<const CurrencyItem*>(item)) {
                type_char = '$';
                type_color = YELLOW;
            }

            // **IMPROVED** Better positioning for type indicator
            DrawRectangle(slot_x + 2, slot_y + slot_size - 16, 12, 12, ColorAlpha(BLACK, 0.7f));
            DrawText(TextFormat("%c", type_char), slot_x + 4, slot_y + slot_size - 14, 10, type_color);
        }
    }

    // Show inventory usage stats
    int used_slots = player_inventory_->GetUsedSlots();
    int max_slots = player_inventory_->GetMaxSlots();
    DrawText(TextFormat("SLOTS: %d/%d", used_slots, max_slots),
             start_x + (5 * slot_spacing) + 20, start_y + 30, 16,
             used_slots >= max_slots ? RED : WHITE);
}

void InventorySystem::RenderStatusMessage(int screen_width, int screen_height) {
    if (status_message_timer_ > 0.0f && !status_message_.empty()) {
        int text_width = MeasureText(status_message_.c_str(), 20);
        int text_x = (screen_width - text_width) / 2;
        int text_y = screen_height - 100;

        // Background
        DrawRectangle(text_x - 15, text_y - 8, text_width + 30, 35, ColorAlpha(BLACK, 0.8f));

        // Text with fade effect
        float alpha = status_message_timer_ > 1.0f ? 1.0f : status_message_timer_;
        Color text_color = ColorAlpha(WHITE, alpha);
        DrawText(status_message_.c_str(), text_x, text_y, 20, text_color);
    }
}

void InventorySystem::HandleSlotSelection() {
    int old_slot = selected_slot_;

    if (IsKeyPressed(KEY_RIGHT)) {
        selected_slot_ = (selected_slot_ + 1) % 10;
    } else if (IsKeyPressed(KEY_LEFT)) {
        selected_slot_ = (selected_slot_ - 1 + 10) % 10;
    } else if (IsKeyPressed(KEY_DOWN)) {
        selected_slot_ = (selected_slot_ + 5) % 10;
    } else if (IsKeyPressed(KEY_UP)) {
        selected_slot_ = (selected_slot_ - 5 + 10) % 10;
    }

    if (old_slot != selected_slot_) {
        const ItemBase* item = player_inventory_->GetItem(selected_slot_);
        if (item) {
            SetStatusMessage("Selected: " + item->GetName(), 2.0f);
        } else {
            SetStatusMessage("Slot " + std::to_string(selected_slot_) + " [Empty]", 1.0f);
        }
    }
}

void InventorySystem::HandleEquipmentActions() {
    if (IsKeyPressed(KEY_ONE)) {
        // Equip to weapon slot
        if (player_inventory_->EquipItem(selected_slot_, EquipmentSlotType::WEAPON)) {
            SetStatusMessage("Equipped weapon!", 2.0f);
        } else {
            SetStatusMessage("Cannot equip as weapon", 2.0f);
        }
    } else if (IsKeyPressed(KEY_TWO)) {
        // Equip to armor slot
        if (player_inventory_->EquipItem(selected_slot_, EquipmentSlotType::ARMOR)) {
            SetStatusMessage("Equipped armor!", 2.0f);
        } else {
            SetStatusMessage("Cannot equip as armor", 2.0f);
        }
    } else if (IsKeyPressed(KEY_THREE)) {
        // Equip to accessory slot
        if (player_inventory_->EquipItem(selected_slot_, EquipmentSlotType::ACCESSORY)) {
            SetStatusMessage("Equipped accessory!", 2.0f);
        } else {
            SetStatusMessage("Cannot equip as accessory", 2.0f);
        }
    } else if (IsKeyPressed(KEY_U)) {
        // Unequip items menu
        if (IsKeyPressed(KEY_Q)) {
            player_inventory_->UnequipItem(EquipmentSlotType::WEAPON);
            SetStatusMessage("Unequipped weapon", 2.0f);
        } else if (IsKeyPressed(KEY_W)) {
            player_inventory_->UnequipItem(EquipmentSlotType::ARMOR);
            SetStatusMessage("Unequipped armor", 2.0f);
        } else if (IsKeyPressed(KEY_E)) {
            player_inventory_->UnequipItem(EquipmentSlotType::ACCESSORY);
            SetStatusMessage("Unequipped accessory", 2.0f);
        } else {
            SetStatusMessage("Press Q/W/E after U to unequip Weapon/Armor/Accessory", 3.0f);
        }
    }
}

void InventorySystem::SetStatusMessage(const std::string& message, float duration) {
    status_message_ = message;
    status_message_timer_ = duration;
}

void InventorySystem::UpdateStatusMessage() {
    if (status_message_timer_ > 0.0f) {
        status_message_timer_ -= GetFrameTime();
        if (status_message_timer_ <= 0.0f) {
            status_message_.clear();
        }
    }
}
// ******************** ADDITIONAL UI IMPROVEMENTS ********************

// Add this method to InventorySystem class (in both .h and .cpp):

void InventorySystem::RenderMinimalUI(int screen_width, int screen_height) {
    // **IMPROVED** When inventory is closed, show better UI

    // Equipment strength bonus with better styling
    int strength_bonus = GetTotalStrengthBonus();
    if (strength_bonus > 0) {
        const char* strength_text = TextFormat("Equipment Strength: +%d", strength_bonus);
        int text_width = MeasureText(strength_text, 18);

        // Background for strength display
        DrawRectangle(10, screen_height - 80, text_width + 20, 30, ColorAlpha(BLACK, 0.6f));
        DrawRectangleLines(10, screen_height - 80, text_width + 20, 30, GREEN);
        DrawText(strength_text, 20, screen_height - 70, 18, GREEN);
    }

    // Inventory prompt with better styling
    const char* prompt_text = "Press 'I' to open inventory";
    int prompt_width = MeasureText(prompt_text, 16);
    DrawRectangle(10, screen_height - 40, prompt_width + 20, 25, ColorAlpha(BLACK, 0.5f));
    DrawText(prompt_text, 20, screen_height - 35, 16, LIGHTGRAY);

    // Show inventory usage
    int used_slots = player_inventory_->GetUsedSlots();
    int max_slots = player_inventory_->GetMaxSlots();
    if (used_slots > 0) {
        const char* usage_text = TextFormat("Inventory: %d/%d", used_slots, max_slots);
        int usage_width = MeasureText(usage_text, 14);
        DrawText(usage_text, screen_width - usage_width - 20, screen_height - 35, 14,
                 used_slots >= max_slots ? RED : LIGHTGRAY);
    }
}

// ******************** WEIGHT MANAGEMENT METHODS ********************

float InventorySystem::GetCurrentWeight() const {
    if (!player_inventory_) {
        return 0.0f;
    }

    float total_weight = 0.0f;

    // Calculate weight from regular inventory slots
    for (int i = 0; i < player_inventory_->GetMaxSlots(); ++i) {
        const ItemBase* item = player_inventory_->GetItem(i);
        if (item) {
            total_weight += item->GetWeight();
        }
    }

    // Add weight from equipped items
    const ItemBase* weapon = player_inventory_->GetEquippedItem(EquipmentSlotType::WEAPON);
    if (weapon) total_weight += weapon->GetWeight();

    const ItemBase* armor = player_inventory_->GetEquippedItem(EquipmentSlotType::ARMOR);
    if (armor) total_weight += armor->GetWeight();

    const ItemBase* accessory = player_inventory_->GetEquippedItem(EquipmentSlotType::ACCESSORY);
    if (accessory) total_weight += accessory->GetWeight();

    return total_weight;
}

float InventorySystem::GetMaxCarryWeight(int player_strength) const {
    // Each point of strength allows 2kg of carry weight
    return player_strength * 2.0f;
}

bool InventorySystem::IsOverweight(int player_strength) const {
    return GetCurrentWeight() > GetMaxCarryWeight(player_strength);
}

// ******************** EQUIPMENT MANAGEMENT METHODS ********************

bool InventorySystem::EquipItemInSlot(int inventory_slot, EquipmentSlotType equipment_slot) {
    if (!player_inventory_) {
        return false;
    }

    bool success = player_inventory_->EquipItem(inventory_slot, equipment_slot);
    if (success) {
        SetStatusMessage("Item equipped successfully!", 2.0f);
    } else {
        SetStatusMessage("Failed to equip item", 2.0f);
    }

    return success;
}

bool InventorySystem::UnequipEquipmentSlot(EquipmentSlotType slot_type) {
    if (!player_inventory_) {
        return false;
    }

    bool success = player_inventory_->UnequipItem(slot_type);
    if (success) {
        SetStatusMessage("Item unequipped successfully!", 2.0f);
    } else {
        SetStatusMessage("No item equipped in that slot", 2.0f);
    }

    return success;
}

// ******************** ITEM ACCESS METHODS ********************

const ItemBase* InventorySystem::GetItemInSlot(int slot) const {
    if (!player_inventory_) {
        return nullptr;
    }

    return player_inventory_->GetItem(slot);
}

int InventorySystem::GetMaxInventorySlots() const {
    if (!player_inventory_) {
        return 0;
    }

    return player_inventory_->GetMaxSlots();
}

int InventorySystem::GetUsedInventorySlots() const {
    if (!player_inventory_) {
        return 0;
    }

    return player_inventory_->GetUsedSlots();
}
// ******************** TASK 3A - INVENTORY SORTING ALGORITHMS ********************

void InventorySystem::SortByWeight(bool ascending) {
    if (!player_inventory_) {
        std::cout << "No inventory to sort!" << std::endl;
        return;
    }

    std::cout << "\n=== SORTING BY WEIGHT (" << (ascending ? "ASCENDING" : "DESCENDING") << ") ===" << std::endl;

    // Print inventory before sorting
    std::cout << "BEFORE SORTING:" << std::endl;
    PrintInventoryItems();

    // Get all non-null items with their slot positions
    std::vector<std::pair<int, std::unique_ptr<ItemBase>>> items_to_sort;

    // Extract items from inventory
    for (int i = 0; i < player_inventory_->GetMaxSlots(); ++i) {
        auto item = player_inventory_->RemoveItem(i);
        if (item) {
            items_to_sort.emplace_back(i, std::move(item));
        }
    }

    // **BUBBLE SORT** by weight (following project rules: reusable, decoupled)
    for (size_t i = 0; i < items_to_sort.size(); ++i) {
        for (size_t j = 0; j < items_to_sort.size() - 1 - i; ++j) {
            float weight1 = items_to_sort[j].second->GetWeight();
            float weight2 = items_to_sort[j + 1].second->GetWeight();

            bool should_swap = ascending ? (weight1 > weight2) : (weight1 < weight2);

            if (should_swap) {
                std::swap(items_to_sort[j], items_to_sort[j + 1]);
            }
        }
    }

    // Put sorted items back into inventory
    for (auto& item_pair : items_to_sort) {
        player_inventory_->AddItem(std::move(item_pair.second));
    }

    // Print inventory after sorting
    std::cout << "AFTER SORTING:" << std::endl;
    PrintInventoryItems();
    std::cout << "=================================" << std::endl;

    SetStatusMessage("Inventory sorted by weight!", 3.0f);
}

void InventorySystem::SortByName(bool ascending) {
    if (!player_inventory_) {
        std::cout << "No inventory to sort!" << std::endl;
        return;
    }

    std::cout << "\n=== SORTING BY NAME (" << (ascending ? "A-Z" : "Z-A") << ") ===" << std::endl;

    // Print inventory before sorting
    std::cout << "BEFORE SORTING:" << std::endl;
    PrintInventoryItems();

    // Get all non-null items
    std::vector<std::pair<int, std::unique_ptr<ItemBase>>> items_to_sort;

    // Extract items from inventory
    for (int i = 0; i < player_inventory_->GetMaxSlots(); ++i) {
        auto item = player_inventory_->RemoveItem(i);
        if (item) {
            items_to_sort.emplace_back(i, std::move(item));
        }
    }

    // **SELECTION SORT** by name (different algorithm for variety)
    for (size_t i = 0; i < items_to_sort.size(); ++i) {
        size_t target_index = i;

        for (size_t j = i + 1; j < items_to_sort.size(); ++j) {
            const std::string& name1 = items_to_sort[target_index].second->GetName();
            const std::string& name2 = items_to_sort[j].second->GetName();

            // Compare strings alphabetically
            bool should_select = ascending ? (name2 < name1) : (name2 > name1);

            if (should_select) {
                target_index = j;
            }
        }

        if (target_index != i) {
            std::swap(items_to_sort[i], items_to_sort[target_index]);
        }
    }

    // Put sorted items back into inventory
    for (auto& item_pair : items_to_sort) {
        player_inventory_->AddItem(std::move(item_pair.second));
    }

    // Print inventory after sorting
    std::cout << "AFTER SORTING:" << std::endl;
    PrintInventoryItems();
    std::cout << "===============================" << std::endl;

    SetStatusMessage("Inventory sorted by name!", 3.0f);
}

void InventorySystem::SortByValue(bool ascending) {
    if (!player_inventory_) {
        std::cout << "No inventory to sort!" << std::endl;
        return;
    }

    std::cout << "\n=== SORTING BY VALUE (" << (ascending ? "LOW-HIGH" : "HIGH-LOW") << ") ===" << std::endl;

    // Print inventory before sorting
    std::cout << "BEFORE SORTING:" << std::endl;
    PrintInventoryItems();

    // Get all non-null items
    std::vector<std::pair<int, std::unique_ptr<ItemBase>>> items_to_sort;

    // Extract items from inventory
    for (int i = 0; i < player_inventory_->GetMaxSlots(); ++i) {
        auto item = player_inventory_->RemoveItem(i);
        if (item) {
            items_to_sort.emplace_back(i, std::move(item));
        }
    }

    // **INSERTION SORT** by value (third different algorithm)
    for (size_t i = 1; i < items_to_sort.size(); ++i) {
        auto current_item = std::move(items_to_sort[i]);
        int current_value = current_item.second->GetValue();

        size_t j = i;
        while (j > 0) {
            int compare_value = items_to_sort[j - 1].second->GetValue();
            bool should_move = ascending ? (compare_value > current_value) : (compare_value < current_value);

            if (!should_move) break;

            items_to_sort[j] = std::move(items_to_sort[j - 1]);
            --j;
        }

        items_to_sort[j] = std::move(current_item);
    }

    // Put sorted items back into inventory
    for (auto& item_pair : items_to_sort) {
        player_inventory_->AddItem(std::move(item_pair.second));
    }

    // Print inventory after sorting
    std::cout << "AFTER SORTING:" << std::endl;
    PrintInventoryItems();
    std::cout << "===============================" << std::endl;

    SetStatusMessage("Inventory sorted by value!", 3.0f);
}

void InventorySystem::SortByType(bool ascending) {
    if (!player_inventory_) {
        std::cout << "No inventory to sort!" << std::endl;
        return;
    }

    std::cout << "\n=== SORTING BY TYPE (" << (ascending ? "A-Z" : "Z-A") << ") ===" << std::endl;

    // Print inventory before sorting
    std::cout << "BEFORE SORTING:" << std::endl;
    PrintInventoryItems();

    // Get all non-null items
    std::vector<std::pair<int, std::unique_ptr<ItemBase>>> items_to_sort;

    // Extract items from inventory
    for (int i = 0; i < player_inventory_->GetMaxSlots(); ++i) {
        auto item = player_inventory_->RemoveItem(i);
        if (item) {
            items_to_sort.emplace_back(i, std::move(item));
        }
    }

    // **BUBBLE SORT** by type description (reusing algorithm but different comparison)
    for (size_t i = 0; i < items_to_sort.size(); ++i) {
        for (size_t j = 0; j < items_to_sort.size() - 1 - i; ++j) {
            const std::string& type1 = items_to_sort[j].second->GetTypeDescription();
            const std::string& type2 = items_to_sort[j + 1].second->GetTypeDescription();

            bool should_swap = ascending ? (type1 > type2) : (type1 < type2);

            if (should_swap) {
                std::swap(items_to_sort[j], items_to_sort[j + 1]);
            }
        }
    }

    // Put sorted items back into inventory
    for (auto& item_pair : items_to_sort) {
        player_inventory_->AddItem(std::move(item_pair.second));
    }

    // Print inventory after sorting
    std::cout << "AFTER SORTING:" << std::endl;
    PrintInventoryItems();
    std::cout << "==============================" << std::endl;

    SetStatusMessage("Inventory sorted by type!", 3.0f);
}

// ******************** HELPER METHOD FOR SORTING DISPLAY ********************

void InventorySystem::PrintInventoryItems() const {
    if (!player_inventory_) {
        return;
    }

    for (int i = 0; i < player_inventory_->GetMaxSlots(); ++i) {
        const ItemBase* item = player_inventory_->GetItem(i);
        if (item) {
            std::cout << "Slot " << i << ": " << item->GetName()
                      << " | Weight: " << item->GetWeight() << "kg"
                      << " | Value: " << item->GetValue() << " coins"
                      << " | Type: " << item->GetTypeDescription()
                      << " | Rarity: " << item->GetRarityName()
                      << std::endl;
        } else {
            std::cout << "Slot " << i << ": [Empty]" << std::endl;
        }
    }
}
void InventorySystem::GenerateTestInventory() {
    if (!player_inventory_) {
        return;
    }

    std::cout << "\n=== GENERATING TEST INVENTORY FOR SORTING DEMO ===" << std::endl;

    // Clear current inventory (keep equipment)
    for (int i = 0; i < player_inventory_->GetMaxSlots(); ++i) {
        player_inventory_->RemoveItem(i);
    }

    // Create varied items to demonstrate sorting (15 items total)
    std::vector<std::unique_ptr<ItemBase>> test_items;

    // **VARIED WEIGHTS** - Different items with different weights
    test_items.push_back(std::make_unique<CurrencyKittyCoin>(10));        // 0.1kg, 1 coin
    test_items.push_back(std::make_unique<ConsumablesHealthPotion>());     // 0.5kg, 50 coins
    test_items.push_back(std::make_unique<AccessoryLuckyPaw>());           // 0.3kg, 90 coins
    test_items.push_back(std::make_unique<ConsumablesBomb>());             // 1.2kg, 75 coins
    test_items.push_back(std::make_unique<ArmorKittyBoots>());             // 1.2kg, 80 coins
    test_items.push_back(std::make_unique<WeaponStaff>());                 // 1.8kg, 120 coins
    test_items.push_back(std::make_unique<AccessoryClawNecklace>());       // 0.5kg, 120 coins
    test_items.push_back(std::make_unique<WeaponSword>());                 // 2.5kg, 150 coins
    test_items.push_back(std::make_unique<ArmorElderWings>());             // 2.8kg, 250 coins
    test_items.push_back(std::make_unique<GemstoneBlue>());                // 0.3kg, 200 coins

    // **MORE VARIED ITEMS** - Add duplicates with different amounts for variety
    test_items.push_back(std::make_unique<CurrencyKittyCoin>(5));          // 0.1kg, 1 coin
    test_items.push_back(std::make_unique<CurrencyKittyCoin>(25));         // 0.1kg, 1 coin
    test_items.push_back(std::make_unique<ConsumablesHealthPotion>());     // 0.5kg, 50 coins
    test_items.push_back(std::make_unique<ConsumablesBomb>());             // 1.2kg, 75 coins
    test_items.push_back(std::make_unique<GemstoneBlue>());                // 0.3kg, 200 coins

    std::cout << "Generated " << test_items.size() << " varied test items:" << std::endl;
    std::cout << "- Weight range: 0.1kg to 2.8kg" << std::endl;
    std::cout << "- Value range: 1 to 250 kitty coins" << std::endl;
    std::cout << "- Name variety: A-Z range (Accessory to Wooden)" << std::endl;
    std::cout << "- Type variety: Weapon, Armor, Accessory, Consumable, Currency, Collectible" << std::endl;

    // Add all items to inventory
    for (auto& item : test_items) {
        if (!AddItemToInventory(std::move(item))) {
            std::cout << "Warning: Inventory full, couldn't add all test items!" << std::endl;
            break;
        }
    }

    SetStatusMessage("Test inventory generated for sorting demo!", 3.0f);
    std::cout << "========================================================" << std::endl;
}

void InventorySystem::DemonstrateAllSorting() {
    if (!player_inventory_) {
        std::cout << "No inventory available for demonstration!" << std::endl;
        return;
    }

    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           TASK 3B - SORTING ALGORITHMS DEMONSTRATION" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    std::cout << "\nDemonstrating ALL sorting functions with varied inventory..." << std::endl;
    std::cout << "Press ENTER to continue between each demonstration..." << std::endl;

    // Show initial unsorted inventory
    std::cout << "\n>>> INITIAL UNSORTED INVENTORY <<<" << std::endl;
    PrintInventoryItems();
    std::cout << "\nPress ENTER to start sorting demonstrations...";
    std::cin.get(); // Wait for user input

    // **1. DEMONSTRATE WEIGHT SORTING**
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "DEMONSTRATION 1/4: SORTING BY WEIGHT" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    SortByWeight(true); // Ascending
    std::cout << "\nPress ENTER to continue to next sorting demo...";
    std::cin.get();

    // **2. DEMONSTRATE NAME SORTING**
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "DEMONSTRATION 2/4: SORTING BY NAME (ALPHABETICAL)" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    SortByName(true); // A-Z
    std::cout << "\nPress ENTER to continue to next sorting demo...";
    std::cin.get();

    // **3. DEMONSTRATE VALUE SORTING**
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "DEMONSTRATION 3/4: SORTING BY VALUE (PRICE)" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    SortByValue(false); // High to Low (more interesting)
    std::cout << "\nPress ENTER to continue to final sorting demo...";
    std::cin.get();

    // **4. DEMONSTRATE TYPE SORTING**
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "DEMONSTRATION 4/4: SORTING BY TYPE" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    SortByType(true); // A-Z by type
    std::cout << "\nPress ENTER to finish demonstration...";
    std::cin.get();

    // **FINAL SUMMARY**
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           SORTING DEMONSTRATION COMPLETE!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "✅ Demonstrated Weight Sorting (Bubble Sort Algorithm)" << std::endl;
    std::cout << "✅ Demonstrated Name Sorting (Selection Sort Algorithm)" << std::endl;
    std::cout << "✅ Demonstrated Value Sorting (Insertion Sort Algorithm)" << std::endl;
    std::cout << "✅ Demonstrated Type Sorting (Bubble Sort Algorithm)" << std::endl;
    std::cout << "\nAll sorting algorithms successfully demonstrated with varied inventory!" << std::endl;
    std::cout << "Items ranged from 0.1kg to 2.8kg weight, 1 to 250 coin values." << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    SetStatusMessage("All sorting algorithms demonstrated successfully!", 5.0f);
}

void InventorySystem::RunSortingDemo() {
    std::cout << "\n🎯 STARTING TASK 3B - SORTING DEMONSTRATION 🎯" << std::endl;

    // Step 1: Generate test inventory with 15 varied items
    GenerateTestInventory();

    // Wait a moment for user to see generation
    std::cout << "\nTest inventory generated! Press ENTER to start sorting demonstrations...";
    std::cin.get();

    // Step 2: Demonstrate all sorting algorithms
    DemonstrateAllSorting();

    std::cout << "\n🎉 TASK 3B DEMONSTRATION COMPLETE! 🎉" << std::endl;
}