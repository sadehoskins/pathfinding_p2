
#ifndef RAYLIBSTARTER_PLAYERCHAR_H
#define RAYLIBSTARTER_PLAYERCHAR_H

#include "Character.h"
#include "inventory/Inventory.h"
#include "inventory/EquipmentSlot.h"
//#include "inventory/InventorySystem.h"
#include "Map.h"
#include <memory>

// Forward declaration
class InventorySystem;

class PlayerChar : public Character {
public:
    // Constructor
    PlayerChar(const Position& start_position, int base_strength = 10);
    ~PlayerChar();

    // Interface for traversing the map
    bool CanMoveTo(const Position& new_position) const override;
    void MoveTo(const Position& new_position) override;

    // Movement methods
    bool TryMoveUp();
    bool TryMoveDown();
    bool TryMoveLeft();
    bool TryMoveRight();
    void SetMap(Map<>* map) { current_map_ = map; }

    // FIXED: Inventory access - return pointer to actual inventory
    Inventory<std::vector>* GetInventory() { return inventory_.get(); }
    const Inventory<std::vector>* GetInventory() const { return inventory_.get(); }

    // Strength system
    int GetStrength() const override;
    int GetTotalStrength() const { return GetStrength(); }
    float GetMaxCarryWeight() const;
    float GetCurrentWeight() const;
    bool IsOverweight() const;

    // Interface for picking up and dropping items
    bool PickUpItemAt(const Position& pos);
    bool DropSelectedItem();

    // Interface for equipping items
    bool EquipSelectedItem(EquipmentSlotType slot_type);
    bool UnequipItem(EquipmentSlotType slot_type);

    // Player methods
    void SetName(const std::string& name) override { Character::SetName(name); }
    void PrintStatus() const override;
    void Update() override;

    // Rendering
    void Render(int screen_x, int screen_y, int tile_size) const override;
    void LoadTexture();
    void UnloadTexture();

    // Utility
    void CheckItemsAtCurrentPosition() const;

private:
    // SINGLE inventory - owned by player
    std::unique_ptr<Inventory<std::vector>> inventory_;

    // Map reference
    Map<>* current_map_;

    // Rendering
    Texture2D character_texture_;
    bool texture_loaded_;

    // Helper methods
    bool IsValidPosition(const Position& pos) const;
    void UpdateStrengthFromEquipment();
};

#endif //RAYLIBSTARTER_PLAYERCHAR_H
