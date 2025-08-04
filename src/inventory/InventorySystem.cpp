
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
    int window_width = 700;
    int window_height = 550;
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
    RenderInventorySlots(window_x + 15, window_y + 220);

    // Controls help
    int help_y = window_y + window_height - 120;
    DrawText("Controls:", window_x + 15, help_y, 16, LIGHTGRAY);
    DrawText("Arrow Keys: Navigate | E: Toggle Equip Mode", window_x + 15, help_y + 25, 14, GRAY);
    DrawText("Enter: Examine Item | D: Drop Item | I/ESC: Close", window_x + 15, help_y + 45, 14, GRAY);

    if (is_equip_mode_) {
        DrawText("EQUIP: 1=Weapon | 2=Armor | 3=Accessory",
                 window_x + 15, help_y + 65, 14, YELLOW);
        DrawText("UNEQUIP: U+Q=Weapon | U+W=Armor | U+E=Accessory",
                 window_x + 15, help_y + 85, 14, YELLOW);
    }
}

void InventorySystem::RenderEquipmentSlots(int start_x, int start_y) {
    DrawText("EQUIPMENT:", start_x, start_y, 18, WHITE);

    int slot_size = 80;
    int slot_spacing = 110;

    // Equipment slot names
    const char* slot_names[] = {"WEAPON", "ARMOR", "ACCESSORY"};
    EquipmentSlotType slot_types[] = {EquipmentSlotType::WEAPON, EquipmentSlotType::ARMOR, EquipmentSlotType::ACCESSORY};

    for (int i = 0; i < 3; i++) {
        int slot_x = start_x + (i * (slot_size + slot_spacing));
        int slot_y = start_y + 30;  // More space from label

        // Draw slot background
        Color slot_color = player_inventory_->IsEquipmentSlotOccupied(slot_types[i]) ?
                           ColorAlpha(DARKGREEN, 0.8f) : ColorAlpha(DARKGRAY, 0.6f);
        DrawRectangle(slot_x, slot_y, slot_size, slot_size, slot_color);
        DrawRectangleLines(slot_x, slot_y, slot_size, slot_size, WHITE);

        // Draw slot label
        int label_width = MeasureText(slot_names[i], 14);
        DrawText(slot_names[i], slot_x + (slot_size - label_width) / 2, slot_y - 22, 14, WHITE);


        // Draw equipped item name (truncated)
        // **IMPROVED** Draw equipped item info
        const ItemBase* equipped = player_inventory_->GetEquippedItem(slot_types[i]);
        if (equipped) {
            // Item name with better wrapping
            std::string item_name = equipped->GetName();

            // **IMPROVED** Better text wrapping for item names
            if (item_name.length() > 12) {
                // Split long names into two lines
                std::string line1 = item_name.substr(0, 12);
                std::string line2 = item_name.substr(12);
                if (line2.length() > 8) {
                    line2 = line2.substr(0, 6) + "..";
                }

                DrawText(line1.c_str(), slot_x + 4, slot_y + 8, 12, WHITE);
                DrawText(line2.c_str(), slot_x + 4, slot_y + 24, 12, WHITE);
            } else {
                DrawText(item_name.c_str(), slot_x + 4, slot_y + 8, 12, WHITE);
            }

            // **IMPROVED** Show strength bonus with better positioning
            int str_bonus = 0;
            if (const WeaponItem* weapon = dynamic_cast<const WeaponItem*>(equipped)) {
                str_bonus = weapon->GetStrengthBonus();
            } else if (const ArmorItem* armor = dynamic_cast<const ArmorItem*>(equipped)) {
                str_bonus = armor->GetStrengthBonus();
            } else if (const AccessoryItem* accessory = dynamic_cast<const AccessoryItem*>(equipped)) {
                str_bonus = accessory->GetStrengthBonus();
            }

            if (str_bonus > 0) {
                DrawText(TextFormat("+%d STR", str_bonus),
                         slot_x + 4, slot_y + slot_size - 18, 12, GREEN);
            }

            // **NEW** Show rarity indicator
            Color rarity_color = WHITE;
            switch (equipped->GetRarity()) {
                case ItemRarity::COMMON: rarity_color = LIGHTGRAY; break;
                case ItemRarity::UNCOMMON: rarity_color = GREEN; break;
                case ItemRarity::RARE: rarity_color = BLUE; break;
                case ItemRarity::LEGENDARY: rarity_color = GOLD; break;
            }
            DrawRectangle(slot_x + slot_size - 12, slot_y + 4, 8, 8, rarity_color);
        } else {
            // **IMPROVED** Show empty slot indicator
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
        DrawText(TextFormat("%d", i), slot_x + 4, slot_y + 4, 14, WHITE);

        // Draw item info if present
        if (item) {
            std::string item_name = item->GetName();
            if (item_name.length() > 10) {
                item_name = item_name.substr(0, 8) + "..";
            }
            DrawText(item_name.c_str(), slot_x + 4, slot_y + 22, 11, WHITE);

            // Draw rarity indicator
            Color rarity_color = WHITE;
            switch (item->GetRarity()) {
                case ItemRarity::COMMON: rarity_color = LIGHTGRAY; break;
                case ItemRarity::UNCOMMON: rarity_color = GREEN; break;
                case ItemRarity::RARE: rarity_color = BLUE; break;
                case ItemRarity::LEGENDARY: rarity_color = GOLD; break;
            }
            DrawRectangle(slot_x + slot_size - 12, slot_y + 4, 10, 10, rarity_color);

            // **NEW** Show item type indicator
            char type_char = '?';
            Color type_color = WHITE;
            if (dynamic_cast<const WeaponItem*>(item)) {
                type_char = 'W';
                type_color = RED;
            } else if (dynamic_cast<const ArmorItem*>(item)) {
                type_char = 'A';
                type_color = BLUE;
            } else if (dynamic_cast<const AccessoryItem*>(item)) {
                type_char = 'C'; // aCcessory
                type_color = PURPLE;
            } else if (dynamic_cast<const ConsumableItem*>(item)) {
                type_char = 'U'; // consUmable
                type_color = ORANGE;
            } else if (dynamic_cast<const CurrencyItem*>(item)) {
                type_char = '$';
                type_color = YELLOW;
            }

            DrawText(TextFormat("%c", type_char), slot_x + 4, slot_y + slot_size - 18, 12, type_color);
        }
    }
    // **NEW** Show inventory usage stats
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

