#ifndef RAYLIBSTARTER_INVENTORYSYSTEM_H
#define RAYLIBSTARTER_INVENTORYSYSTEM_H

#include "../TextureManager.h"
#include "../inventory/Inventory.h"
#include "../items/ItemManager.h"
#include "../items/armor/ArmorKittyBoots.h"
#include "../items/armor/ArmorElderWings.h"
#include "../items/accessories/AccessoryLuckyPaw.h"
#include "../items/accessories/AccessoryClawNecklace.h"
#include "../items/weapons/WeaponSword.h"
#include "../items/weapons/WeaponStaff.h"

#include "raylib.h"
#include <memory>
#include <string>

// ******************** INVENTORY SYSTEM CLASS ********************

class InventorySystem {
public:
    // Constructor
    InventorySystem();

    // Destructor
    ~InventorySystem();

    // Core system methods
    void HandleInput();
    void Update();
    void Render(int screen_width, int screen_height);

    // Inventory state
    bool IsInventoryOpen() const { return is_inventory_open_; }
    void OpenInventory();
    void CloseInventory();
    void ToggleInventory();

    // Item management
    bool AddItemToInventory(std::unique_ptr<ItemBase> item);
    bool OpenTreasureChest(const Position& pos, ItemManager& item_manager);

    // Equipment management
    void HandleEquipInput();
    void ShowEquipMenu();

    // Utility
    void PrintInventoryStatus() const;
    int GetTotalStrengthBonus() const;

    // Weight management methods
    float GetCurrentWeight() const;
    float GetMaxCarryWeight(int player_strength) const;
    bool IsOverweight(int player_strength) const;

    // Equipment management methods
    bool EquipItemInSlot(int inventory_slot, EquipmentSlotType equipment_slot);
    bool UnequipEquipmentSlot(EquipmentSlotType slot_type);

    // Item access methods
    const ItemBase* GetItemInSlot(int slot) const;
    int GetMaxInventorySlots() const;
    int GetUsedInventorySlots() const;

    // Inventory sorting methods
    void SortByWeight(bool ascending = true);
    void SortByName(bool ascending = true);
    void SortByValue(bool ascending = true);
    void SortByType(bool ascending = true);  // Bonus: sort by item type

    // Helper method for sorting display
    void PrintInventoryItems() const;

    // Sorting demonstration methods
    void GenerateTestInventory();
    void DemonstrateAllSorting();
    void RunSortingDemo();

private:
    std::unique_ptr<Inventory<std::vector>> player_inventory_;
    bool is_inventory_open_;
    bool is_equip_mode_;
    int selected_slot_;

    // UI state
    std::string status_message_;
    float status_message_timer_;

    // Private methods
    void RenderInventoryWindow(int screen_width, int screen_height);
    void RenderEquipmentSlots(int start_x, int start_y);
    void RenderInventorySlots(int start_x, int start_y);
    void RenderStatusMessage(int screen_width, int screen_height);
    void RenderMinimalUI(int screen_width, int screen_height);
    void HandleSlotSelection();
    void HandleEquipmentActions();
    void SetStatusMessage(const std::string& message, float duration = 3.0f);
    void UpdateStatusMessage();
};


#endif //RAYLIBSTARTER_INVENTORYSYSTEM_H
