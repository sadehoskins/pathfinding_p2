#include "TextureManager.h"
#include <iostream>

// ******************** STATIC MEMBER DEFINITIONS ********************

std::unordered_map<std::string, Texture2D> TextureManager::textures_;
std::unordered_map<std::string, TextureManager::TextureCategory> TextureManager::texture_categories_;
bool TextureManager::textures_loaded_ = false;

// ******************** MAIN TEXTURE MANAGEMENT ********************

void TextureManager::LoadAllTextures() {
    if (textures_loaded_) {
        std::cout << "Textures already loaded, skipping..." << std::endl;
        return;
    }

    std::cout << "Loading all game textures..." << std::endl;

    // Load all texture categories
    LoadTileTextures();
    LoadItemTextures();
    LoadCharacterTextures();
    LoadUITextures();

    textures_loaded_ = true;
    std::cout << "All textures loaded successfully! Total: " << GetTextureCount() << " textures." << std::endl;
}

void TextureManager::UnloadAllTextures() {
    if (!textures_loaded_) {
        return;
    }

    std::cout << "Unloading all game textures..." << std::endl;

    for (auto& pair : textures_) {
        if (pair.second.id != 0) {
            ::UnloadTexture(pair.second);
        }
    }

    textures_.clear();
    texture_categories_.clear();
    textures_loaded_ = false;
    std::cout << "All textures unloaded successfully!" << std::endl;
}

bool TextureManager::AreTexturesLoaded() {
    return textures_loaded_;
}

// ******************** TEXTURE ACCESS METHODS ********************

Texture2D TextureManager::GetTexture(const std::string& texture_name) {
    auto it = textures_.find(texture_name);
    if (it != textures_.end()) {
        return it->second;
    }

    std::cout << "Warning: Texture '" << texture_name << "' not found!" << std::endl;
    return Texture2D{0}; // Return empty texture
}



bool TextureManager::HasTexture(const std::string& texture_name) {
    return textures_.find(texture_name) != textures_.end();
}

// ******************** CATEGORY-SPECIFIC ACCESS ********************

Texture2D TextureManager::GetTileTexture(const std::string& tile_name) {
    return GetTexture("tile_" + tile_name);
}

Texture2D TextureManager::GetItemTexture(const std::string& item_name) {
    return GetTexture("item_" + item_name);
}

Texture2D TextureManager::GetCharacterTexture(const std::string& character_name) {
    return GetTexture("char_" + character_name);
}

Texture2D TextureManager::GetUITexture(const std::string& ui_name) {
    return GetTexture("ui_" + ui_name);
}

// ******************** DYNAMIC LOADING ********************

bool TextureManager::LoadTexture(const std::string& texture_name, const std::string& file_path, TextureCategory category) {
    if (HasTexture(texture_name)) {
        std::cout << "Texture '" << texture_name << "' already loaded." << std::endl;
        return true;
    }

    LoadTextureFromFile(texture_name, file_path, category);
    return HasTexture(texture_name);
}



void TextureManager::UnloadTexture(const std::string& texture_name) {
    auto it = textures_.find(texture_name);
    if (it != textures_.end() && it->second.id != 0) {
        ::UnloadTexture(it->second); // Use global UnloadTexture function
        textures_.erase(it);
        texture_categories_.erase(texture_name);
        std::cout << "Unloaded texture: " << texture_name << std::endl;
    }
}

// ******************** PRIVATE LOADING METHODS ********************

void TextureManager::LoadTileTextures() {
    std::cout << "Loading tile textures..." << std::endl;

    // Start/End tiles
    LoadTextureFromFile("tile_start", "assets/graphics/tiles/start.png", TextureCategory::TILES);
    LoadTextureFromFile("tile_end", "assets/graphics/tiles/end.png", TextureCategory::TILES);

    // Blocked tiles
    LoadTextureFromFile("tile_stone", "assets/graphics/tiles/blocked/stone.png", TextureCategory::TILES);
    LoadTextureFromFile("tile_bushes", "assets/graphics/tiles/blocked/bushes.png", TextureCategory::TILES);
    LoadTextureFromFile("tile_tree", "assets/graphics/tiles/blocked/tree.png", TextureCategory::TILES);
    LoadTextureFromFile("tile_water", "assets/graphics/tiles/blocked/water.png", TextureCategory::TILES);

    // Traversable tiles
    LoadTextureFromFile("tile_dirt_path", "assets/graphics/tiles/traversable/dirt_path.png", TextureCategory::TILES);
    LoadTextureFromFile("tile_stone_tile", "assets/graphics/tiles/traversable/stone_tile.png", TextureCategory::TILES);
    LoadTextureFromFile("tile_grass", "assets/graphics/tiles/traversable/grass.png", TextureCategory::TILES);

    // Treasure chests
    LoadTextureFromFile("tile_treasure_chest_closed", "assets/graphics/items/treasure_chest_closed.png", TextureCategory::TILES);
    LoadTextureFromFile("tile_treasure_chest_opened", "assets/graphics/items/treasure_chest_opened.png", TextureCategory::TILES);
}

void TextureManager::LoadItemTextures() {
    std::cout << "Loading item textures..." << std::endl;

    // Weapons
    LoadTextureFromFile("item_Iron Sword", "assets/graphics/items/weapons/basic_sword.png", TextureCategory::ITEMS);
    LoadTextureFromFile("item_Wooden Staff", "assets/graphics/items/weapons/kitty_staff.png", TextureCategory::ITEMS);

    // Armor
    LoadTextureFromFile("item_Kitty Boots", "assets/graphics/items/armor/armor_kitty_boots.png", TextureCategory::ITEMS);
    LoadTextureFromFile("item_Elder Wings", "assets/graphics/items/armor/armor_elder_wings.png", TextureCategory::ITEMS);

    // Accessories
    LoadTextureFromFile("item_Lucky Paw", "assets/graphics/items/accessory/accessory_lucky_paw.png", TextureCategory::ITEMS);
    LoadTextureFromFile("item_Claw Necklace", "assets/graphics/items/accessory/accessory_claw_necklace.png", TextureCategory::ITEMS);

    // Consumables
    LoadTextureFromFile("item_Health Potion", "assets/graphics/items/consumables/health_potion.png", TextureCategory::ITEMS);
    LoadTextureFromFile("item_Explosive Bomb", "assets/graphics/items/consumables/bomb.png", TextureCategory::ITEMS);

    // Currency
    LoadTextureFromFile("item_Kitty Coin", "assets/graphics/items/currency/kittycoin.png", TextureCategory::ITEMS);

    // Collectibles
    LoadTextureFromFile("item_Blue Gemstone", "assets/graphics/items/collectibles/gemstone_blue.png", TextureCategory::ITEMS);
}

void TextureManager::LoadCharacterTextures() {
    std::cout << "Loading character textures..." << std::endl;

    // Player character
    LoadTextureFromFile("char_player", "assets/graphics/main_character/main_character.png", TextureCategory::CHARACTERS);

    // Future NPCs can be added here
    // LoadTextureFromFile("char_npc_guard", "assets/graphics/characters/guard.png", TextureCategory::CHARACTERS);
}

void TextureManager::LoadUITextures() {
    std::cout << "Loading UI textures..." << std::endl;

    // **NEW** Load sparkle texture for hidden items
    LoadTextureFromFile("ui_sparkle", "assets/graphics/ui/sparkle.png", TextureCategory::UI);

    // Future UI elements can be added here
    // LoadTextureFromFile("ui_button", "assets/graphics/ui/button.png", TextureCategory::UI);
    // LoadTextureFromFile("ui_panel", "assets/graphics/ui/panel.png", TextureCategory::UI);
}

void TextureManager::LoadTextureFromFile(const std::string& texture_name, const std::string& file_path, TextureCategory category) {
    Texture2D texture = ::LoadTexture(file_path.c_str()); // Use global LoadTexture function
    if (texture.id == 0) {
        std::cout << "Warning: Failed to load texture '" << texture_name
                  << "' from path: " << file_path << std::endl;
    }
    textures_[texture_name] = texture;
    texture_categories_[texture_name] = category;
}

// ******************** UTILITY METHODS ********************

void TextureManager::PrintLoadedTextures() {
    std::cout << "\n=== LOADED TEXTURES ===" << std::endl;
    for (const auto& pair : textures_) {
        std::cout << "- " << pair.first;
        if (pair.second.id == 0) {
            std::cout << " (FAILED)";
        }
        std::cout << std::endl;
    }
    std::cout << "Total: " << GetTextureCount() << " textures" << std::endl;
    std::cout << "======================" << std::endl;
}

int TextureManager::GetTextureCount() {
    return textures_.size();
}

std::string TextureManager::GetCategoryPath(TextureCategory category) {
    switch (category) {
        case TextureCategory::TILES: return "assets/graphics/tiles/";
        case TextureCategory::ITEMS: return "assets/graphics/items/";
        case TextureCategory::CHARACTERS: return "assets/graphics/characters/";
        case TextureCategory::UI: return "assets/graphics/ui/";
        case TextureCategory::EFFECTS: return "assets/graphics/effects/";
        default: return "assets/graphics/";
    }
}