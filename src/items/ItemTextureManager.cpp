
#include "ItemTextureManager.h"
#include <iostream>

// ******************** STATIC MEMBER DEFINITIONS ********************

std::unordered_map<std::string, Texture2D> ItemTextureManager::item_textures_;
bool ItemTextureManager::textures_loaded_ = false;

// ******************** STATIC TEXTURE MANAGEMENT ********************

void ItemTextureManager::LoadAllItemTextures() {
    if (textures_loaded_) {
        std::cout << "Item textures already loaded, skipping..." << std::endl;
        return;
    }

    std::cout << "Loading item textures..." << std::endl;

    // Load weapon textures
    LoadTextureForItem("Iron Sword", "assets/graphics/items/weapons/basic_sword.png");
    LoadTextureForItem("Wooden Staff", "assets/graphics/items/weapons/kitty_staff.png");

    // Load armor textures
    LoadTextureForItem("Kitty Boots", "assets/graphics/items/armor/armor_kitty_boots.png");
    LoadTextureForItem("Elder Wings", "assets/graphics/items/armor/armor_elder_wings.png");

    // Load accessory textures
    LoadTextureForItem("Lucky Paw", "assets/graphics/items/accessories/accessory_lucky_paw.png");
    LoadTextureForItem("Claw Necklace", "assets/graphics/items/accessories/accessory_claw_necklace.png");

    // Load consumable textures
    LoadTextureForItem("Health Potion", "assets/graphics/items/consumables/health_potion.png");
    LoadTextureForItem("Explosive Bomb", "assets/graphics/items/consumables/bomb.png");

    // Load currency textures
    LoadTextureForItem("Kitty Coin", "assets/graphics/items/currency/kittycoin.png");

    // Load collectible textures
    LoadTextureForItem("Blue Gemstone", "assets/graphics/items/collectibles/gemstone_blue.png");

    textures_loaded_ = true;
    std::cout << "Item textures loaded successfully!" << std::endl;
}

void ItemTextureManager::UnloadAllItemTextures() {
    if (!textures_loaded_) {
        return;
    }

    std::cout << "Unloading item textures..." << std::endl;

    for (auto& pair : item_textures_) {
        if (pair.second.id != 0) {
            UnloadTexture(pair.second);
        }
    }

    item_textures_.clear();
    textures_loaded_ = false;
    std::cout << "Item textures unloaded successfully!" << std::endl;
}

bool ItemTextureManager::AreTexturesLoaded() {
    return textures_loaded_;
}

Texture2D ItemTextureManager::GetItemTexture(const std::string& item_name) {
    auto it = item_textures_.find(item_name);
    if (it != item_textures_.end()) {
        return it->second;
    }
    // Return empty texture if not found
    return Texture2D{0};
}

void ItemTextureManager::LoadTextureForItem(const std::string& item_name, const std::string& texture_path) {
    Texture2D texture = LoadTexture(texture_path.c_str());
    if (texture.id == 0) {
        std::cout << "Warning: Failed to load texture for item: " << item_name
                  << " from path: " << texture_path << std::endl;
    }
    item_textures_[item_name] = texture;
}