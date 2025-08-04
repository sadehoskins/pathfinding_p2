#ifndef RAYLIBSTARTER_ITEMTEXTUREMANAGER_H
#define RAYLIBSTARTER_ITEMTEXTUREMANAGER_H

#include "raylib.h"
#include <unordered_map>
#include <string>

// ******************** ITEM TEXTURE MANAGER CLASS ********************

class ItemTextureManager {
public:
    // Static texture management
    static void LoadAllItemTextures();
    static void UnloadAllItemTextures();
    static bool AreTexturesLoaded();
    static Texture2D GetItemTexture(const std::string& item_name);

private:
    static std::unordered_map<std::string, Texture2D> item_textures_;
    static bool textures_loaded_;

    // Helper methods
    static void LoadTextureForItem(const std::string& item_name, const std::string& texture_path);
};

#endif //RAYLIBSTARTER_ITEMTEXTUREMANAGER_H
