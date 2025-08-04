#ifndef RAYLIBSTARTER_INVENTORY_H
#define RAYLIBSTARTER_INVENTORY_H

#include "../items/base-classes/ItemBase.h"
#include "../items/base-classes/WeaponItem.h"
#include "../items/base-classes/ArmorItem.h"
#include "../items/base-classes/AccessoryItem.h"
#include "EquipmentSlot.h"
#include <vector>
#include <deque>
#include <list>
#include <memory>
#include <iostream>

// ******************** TEMPLATED INVENTORY CLASS ********************

template<template<typename> class Container = std::vector>
class Inventory {
public:
    // Constructor
    explicit Inventory(int max_slots);

    // Destructor
    ~Inventory();

    // Regular inventory management
    bool AddItem(std::unique_ptr<ItemBase> item);
    std::unique_ptr<ItemBase> RemoveItem(int slot);
    const ItemBase* GetItem(int slot) const;

    // Equipment management
    bool EquipItem(int inventory_slot, EquipmentSlotType slot_type);
    bool UnequipItem(EquipmentSlotType slot_type);
    const ItemBase* GetEquippedItem(EquipmentSlotType slot_type) const;

    // Inventory queries
    int GetMaxSlots() const { return max_regular_slots_; }
    int GetUsedSlots() const;
    int GetFreeSlots() const;
    bool IsFull() const;
    bool IsSlotEmpty(int slot) const;

    // Equipment queries
    bool IsEquipmentSlotOccupied(EquipmentSlotType slot_type) const;
    int GetTotalStrengthBonus() const;

    // Utility
    void PrintInventory() const;
    void PrintEquipment() const;

private:
    Container<std::unique_ptr<ItemBase>> regular_items_;
    std::unique_ptr<ItemBase> weapon_slot_;
    std::unique_ptr<ItemBase> armor_slot_;
    std::unique_ptr<ItemBase> accessory_slot_;
    int max_regular_slots_;

    // Helper methods
    bool IsValidSlot(int slot) const;
    bool CanEquipItem(const ItemBase* item, EquipmentSlotType slot_type) const;
    std::unique_ptr<ItemBase>& GetEquipmentSlotReference(EquipmentSlotType slot_type);
    int FindFirstEmptySlot() const;
};

// ******************** TEMPLATE IMPLEMENTATION ********************

template<template<typename> class Container>
Inventory<Container>::Inventory(int max_slots)
        : max_regular_slots_(max_slots), weapon_slot_(nullptr),
          armor_slot_(nullptr), accessory_slot_(nullptr) {

    if (max_slots <= 0) {
        max_regular_slots_ = 10; // Default fallback
    }

    // Initialize container with empty slots
    regular_items_.resize(max_regular_slots_);

    std::cout << "Inventory created with " << max_regular_slots_
              << " regular slots and 3 equipment slots." << std::endl;
}

template<template<typename> class Container>
Inventory<Container>::~Inventory() {
    // Smart pointers handle cleanup automatically
}

// ******************** REGULAR INVENTORY MANAGEMENT ********************

template<template<typename> class Container>
bool Inventory<Container>::AddItem(std::unique_ptr<ItemBase> item) {
    if (!item) {
        std::cout << "Cannot add null item to inventory." << std::endl;
        return false;
    }

    // Find first empty slot
    int empty_slot = FindFirstEmptySlot();
    if (empty_slot == -1) {
        std::cout << "Inventory is full! Cannot add " << item->GetName() << std::endl;
        return false;
    }

    std::cout << "Added " << item->GetName() << " to inventory slot " << empty_slot << std::endl;
    regular_items_[empty_slot] = std::move(item);
    return true;
}

template<template<typename> class Container>
std::unique_ptr<ItemBase> Inventory<Container>::RemoveItem(int slot) {
    if (!IsValidSlot(slot) || !regular_items_[slot]) {
        std::cout << "Cannot remove item from slot " << slot << " (invalid or empty)" << std::endl;
        return nullptr;
    }

    std::unique_ptr<ItemBase> item = std::move(regular_items_[slot]);
    regular_items_[slot] = nullptr;

    std::cout << "Removed " << item->GetName() << " from inventory slot " << slot << std::endl;
    return item;
}

template<template<typename> class Container>
const ItemBase* Inventory<Container>::GetItem(int slot) const {
    if (!IsValidSlot(slot)) {
        return nullptr;
    }
    return regular_items_[slot].get();
}

// ******************** EQUIPMENT MANAGEMENT ********************

template<template<typename> class Container>
bool Inventory<Container>::EquipItem(int inventory_slot, EquipmentSlotType slot_type) {
    // Validate inventory slot
    if (!IsValidSlot(inventory_slot) || !regular_items_[inventory_slot]) {
        std::cout << "Invalid inventory slot or empty slot." << std::endl;
        return false;
    }

    const ItemBase* item = regular_items_[inventory_slot].get();

    // Check if item can be equipped in the specified slot
    if (!CanEquipItem(item, slot_type)) {
        std::cout << "Cannot equip " << item->GetName()
                  << " in " << GetEquipmentSlotName(slot_type) << " slot." << std::endl;
        return false;
    }

    // Get reference to equipment slot
    std::unique_ptr<ItemBase>& equipment_slot = GetEquipmentSlotReference(slot_type);

    // If equipment slot is occupied, try to move old item to inventory
    if (equipment_slot) {
        if (IsFull()) {
            std::cout << "Cannot equip item - inventory full and equipment slot occupied." << std::endl;
            return false;
        }

        // Move old equipped item to inventory
        std::unique_ptr<ItemBase> old_item = std::move(equipment_slot);
        if (!AddItem(std::move(old_item))) {
            // This shouldn't happen since we checked IsFull(), but safety first
            equipment_slot = std::move(old_item); // Put it back
            return false;
        }
    }

    // Move item from inventory to equipment slot
    equipment_slot = std::move(regular_items_[inventory_slot]);
    regular_items_[inventory_slot] = nullptr;

    std::cout << "Equipped " << equipment_slot->GetName()
              << " in " << GetEquipmentSlotName(slot_type) << " slot." << std::endl;

    return true;
}

template<template<typename> class Container>
bool Inventory<Container>::UnequipItem(EquipmentSlotType slot_type) {
    std::unique_ptr<ItemBase>& equipment_slot = GetEquipmentSlotReference(slot_type);

    if (!equipment_slot) {
        std::cout << GetEquipmentSlotName(slot_type) << " slot is already empty." << std::endl;
        return false;
    }

    if (IsFull()) {
        std::cout << "Cannot unequip item - inventory is full." << std::endl;
        return false;
    }

    // Move item from equipment slot to inventory
    std::unique_ptr<ItemBase> item = std::move(equipment_slot);
    equipment_slot = nullptr;

    bool success = AddItem(std::move(item));
    if (!success) {
        // This shouldn't happen, but safety first
        std::cout << "Error: Failed to move unequipped item to inventory." << std::endl;
    }

    return success;
}

template<template<typename> class Container>
const ItemBase* Inventory<Container>::GetEquippedItem(EquipmentSlotType slot_type) const {
    switch (slot_type) {
        case EquipmentSlotType::WEAPON:
            return weapon_slot_.get();
        case EquipmentSlotType::ARMOR:
            return armor_slot_.get();
        case EquipmentSlotType::ACCESSORY:
            return accessory_slot_.get();
        default:
            return nullptr;
    }
}

// ******************** INVENTORY QUERIES ********************

template<template<typename> class Container>
int Inventory<Container>::GetUsedSlots() const {
    int used = 0;
    for (const auto& item : regular_items_) {
        if (item) {
            used++;
        }
    }
    return used;
}

template<template<typename> class Container>
int Inventory<Container>::GetFreeSlots() const {
    return max_regular_slots_ - GetUsedSlots();
}

template<template<typename> class Container>
bool Inventory<Container>::IsFull() const {
    return GetFreeSlots() == 0;
}

template<template<typename> class Container>
bool Inventory<Container>::IsSlotEmpty(int slot) const {
    return IsValidSlot(slot) && !regular_items_[slot];
}

template<template<typename> class Container>
bool Inventory<Container>::IsEquipmentSlotOccupied(EquipmentSlotType slot_type) const {
    return GetEquippedItem(slot_type) != nullptr;
}

template<template<typename> class Container>
int Inventory<Container>::GetTotalStrengthBonus() const {
    int total_bonus = 0;

    // Check weapon slot
    if (weapon_slot_) {
        if (const WeaponItem* weapon = dynamic_cast<const WeaponItem*>(weapon_slot_.get())) {
            total_bonus += weapon->GetStrengthBonus();
        }
    }

    // Check armor slot
    if (armor_slot_) {
        if (const ArmorItem* armor = dynamic_cast<const ArmorItem*>(armor_slot_.get())) {
            total_bonus += armor->GetStrengthBonus();
        }
    }

    // Check accessory slot
    if (accessory_slot_) {
        if (const AccessoryItem* accessory = dynamic_cast<const AccessoryItem*>(accessory_slot_.get())) {
            total_bonus += accessory->GetStrengthBonus();
        }
    }

    return total_bonus;
}

// ******************** UTILITY METHODS ********************

template<template<typename> class Container>
void Inventory<Container>::PrintInventory() const {
    std::cout << "\n=== INVENTORY (" << GetUsedSlots() << "/" << max_regular_slots_ << ") ===" << std::endl;

    for (int i = 0; i < max_regular_slots_; ++i) {
        std::cout << "Slot " << i << ": ";
        if (regular_items_[i]) {
            const ItemBase* item = regular_items_[i].get();
            std::cout << item->GetName() << " (" << item->GetTypeDescription()
                      << ", " << item->GetRarityName() << ")";
        } else {
            std::cout << "[Empty]";
        }
        std::cout << std::endl;
    }

    std::cout << "====================" << std::endl;
}

template<template<typename> class Container>
void Inventory<Container>::PrintEquipment() const {
    std::cout << "\n=== EQUIPMENT ===" << std::endl;

    std::cout << "Weapon: ";
    if (weapon_slot_) {
        const WeaponItem* weapon = dynamic_cast<const WeaponItem*>(weapon_slot_.get());
        std::cout << weapon_slot_->GetName() << " (+" << (weapon ? weapon->GetStrengthBonus() : 0) << " STR)";
    } else {
        std::cout << "[Empty]";
    }
    std::cout << std::endl;

    std::cout << "Armor: ";
    if (armor_slot_) {
        const ArmorItem* armor = dynamic_cast<const ArmorItem*>(armor_slot_.get());
        std::cout << armor_slot_->GetName() << " (+" << (armor ? armor->GetStrengthBonus() : 0) << " STR)";
    } else {
        std::cout << "[Empty]";
    }
    std::cout << std::endl;

    std::cout << "Accessory: ";
    if (accessory_slot_) {
        const AccessoryItem* accessory = dynamic_cast<const AccessoryItem*>(accessory_slot_.get());
        std::cout << accessory_slot_->GetName() << " (+" << (accessory ? accessory->GetStrengthBonus() : 0) << " STR)";
    } else {
        std::cout << "[Empty]";
    }
    std::cout << std::endl;

    std::cout << "Total Strength Bonus: +" << GetTotalStrengthBonus() << std::endl;
    std::cout << "=================" << std::endl;
}

// ******************** PRIVATE HELPER METHODS ********************

template<template<typename> class Container>
bool Inventory<Container>::IsValidSlot(int slot) const {
    return slot >= 0 && slot < max_regular_slots_;
}

template<template<typename> class Container>
bool Inventory<Container>::CanEquipItem(const ItemBase* item, EquipmentSlotType slot_type) const {
    if (!item) return false;

    switch (slot_type) {
        case EquipmentSlotType::WEAPON:
            return dynamic_cast<const WeaponItem*>(item) != nullptr;
        case EquipmentSlotType::ARMOR:
            return dynamic_cast<const ArmorItem*>(item) != nullptr;
        case EquipmentSlotType::ACCESSORY:
            return dynamic_cast<const AccessoryItem*>(item) != nullptr;
        default:
            return false;
    }
}

template<template<typename> class Container>
std::unique_ptr<ItemBase>& Inventory<Container>::GetEquipmentSlotReference(EquipmentSlotType slot_type) {
    switch (slot_type) {
        case EquipmentSlotType::WEAPON:
            return weapon_slot_;
        case EquipmentSlotType::ARMOR:
            return armor_slot_;
        case EquipmentSlotType::ACCESSORY:
            return accessory_slot_;
        default:
            // This should never happen, but we need to return something
            static std::unique_ptr<ItemBase> null_slot = nullptr;
            return null_slot;
    }
}

template<template<typename> class Container>
int Inventory<Container>::FindFirstEmptySlot() const {
    for (int i = 0; i < max_regular_slots_; ++i) {
        if (!regular_items_[i]) {
            return i;
        }
    }
    return -1; // No empty slot found
}

#endif //RAYLIBSTARTER_INVENTORY_H
