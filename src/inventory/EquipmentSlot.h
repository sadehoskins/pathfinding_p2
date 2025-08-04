
#ifndef RAYLIBSTARTER_EQUIPMENTSLOT_H
#define RAYLIBSTARTER_EQUIPMENTSLOT_H

#include <string>

// ******************** EQUIPMENT SLOT TYPE ENUM ********************

enum class EquipmentSlotType {
    WEAPON,
    ARMOR,
    ACCESSORY
};

// ******************** UTILITY FUNCTIONS ********************

inline std::string GetEquipmentSlotName(EquipmentSlotType slot_type) {
    switch (slot_type) {
        case EquipmentSlotType::WEAPON:
            return "Weapon";
        case EquipmentSlotType::ARMOR:
            return "Armor";
        case EquipmentSlotType::ACCESSORY:
            return "Accessory";
        default:
            return "Unknown";
    }
}


#endif //RAYLIBSTARTER_EQUIPMENTSLOT_H
