
#ifndef RAYLIBSTARTER_PLAYERCHAR_H
#define RAYLIBSTARTER_PLAYERCHAR_H

#include "Character.h"
#include "inventory/Inventory.h"
#include "inventory/InventorySystem.h"
#include "Map.h"
#include <memory>

// ******************** PLAYER CHARACTER CLASS ********************

class PlayerChar : public Character {
public:
    // Constructor
    PlayerChar(const Position& start_position, int base_strength = 10);

    // Destructor
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

    // Inventory integration
    InventorySystem& GetInventorySystem() { return *inventory_system_; }
    const InventorySystem& GetInventorySystem() const { return *inventory_system_; }

    // Strength system
    int GetStrength() const override; // Base + equipment bonuses
    int GetTotalStrength() const { return GetStrength(); } // Alias for compatibility
    float GetMaxCarryWeight() const; // Based on total strength
    float GetCurrentWeight() const;
    bool IsOverweight() const;

    // Interface for picking up and dropping items
    bool PickUpItemAt(const Position& pos);
    bool DropSelectedItem();

    // Interface for equipping items
    bool EquipSelectedItem(EquipmentSlotType slot_type);
    bool UnequipItem(EquipmentSlotType slot_type);

    // Player methods -> use base class health system
    void SetName(const std::string& name) override { Character::SetName(name); }
    void PrintStatus() const override; // Enhanced version showing inventory info
    void Update() override; // Player-specific update logic

    // Rendering
    void Render(int screen_x, int screen_y, int tile_size) const override;
    void LoadTexture();
    void UnloadTexture();

    // Utility
    void CheckItemsAtCurrentPosition() const;

private:
    // Inventory
    std::unique_ptr<InventorySystem> inventory_system_;

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
