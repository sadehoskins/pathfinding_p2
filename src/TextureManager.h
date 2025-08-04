
#ifndef RAYLIBSTARTER_TEXTUREMANAGER_H
#define RAYLIBSTARTER_TEXTUREMANAGER_H

#include "raylib.h"
#include <unordered_map>
#include <string>

// ******************** UNIFIED TEXTURE MANAGER CLASS ********************

class TextureManager {
public:
    // Texture categories for organization
    enum class TextureCategory {
        TILES,
        ITEMS,
        CHARACTERS,
        UI,
        EFFECTS
    };

    // Static texture management
    static void LoadAllTextures();
    static void UnloadAllTextures();
    static bool AreTexturesLoaded();

    // Generic texture access
    static Texture2D GetTexture(const std::string& texture_name);
    static bool HasTexture(const std::string& texture_name);

    // Category-specific convenience methods
    static Texture2D GetTileTexture(const std::string& tile_name);
    static Texture2D GetItemTexture(const std::string& item_name);
    static Texture2D GetCharacterTexture(const std::string& character_name);
    static Texture2D GetUITexture(const std::string& ui_name);

    // Dynamic texture loading (for runtime loading)
    static bool LoadTexture(const std::string& texture_name, const std::string& file_path, TextureCategory category = TextureCategory::ITEMS);
    static void UnloadTexture(const std::string& texture_name);

    // Utility
    static void PrintLoadedTextures();
    static int GetTextureCount();

private:
    static std::unordered_map<std::string, Texture2D> textures_;
    static std::unordered_map<std::string, TextureCategory> texture_categories_;
    static bool textures_loaded_;

    // Helper methods
    static void LoadTextureFromFile(const std::string& texture_name, const std::string& file_path, TextureCategory category);
    static void LoadTileTextures();
    static void LoadItemTextures();
    static void LoadCharacterTextures();
    static void LoadUITextures();

    // Path helpers
    static std::string GetCategoryPath(TextureCategory category);
};


#endif //RAYLIBSTARTER_TEXTUREMANAGER_H
