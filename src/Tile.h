//
// Created by sadeh on 8/2/2025.
//

#ifndef RAYLIBSTARTER_TILE_H
#define RAYLIBSTARTER_TILE_H

#include "raylib.h"
#include <string>
#include <unordered_map>

// ******************** TILE TYPES ********************

enum class TileType {
    START,
    END,

    // Blocked variants (all return false for IsTraversable())
    BLOCKED_STONE,
    BLOCKED_BUSHES,
    BLOCKED_TREE,
    BLOCKED_WATER,

    // Traversable variants (all return true for IsTraversable())
    TRAVERSABLE_DIRT,
    TRAVERSABLE_STONE,
    TRAVERSABLE_GRASS
};

// ******************** POSITION STRUCT ********************

struct Position {
    int x;
    int y;

    Position() : x(0), y(0) {}
    Position(int x_pos, int y_pos) : x(x_pos), y(y_pos) {}

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

// ******************** TILE CLASS ********************

class Tile {
public:
    // Static texture management
    static void LoadAllTextures();
    static void UnloadAllTextures();
    static bool AreTexturesLoaded();

    // Utility functions for tile type logic
    static bool IsBlockedType(TileType type);
    static bool IsTraversableType(TileType type);
    static TileType GetRandomBlockedType();
    static TileType GetRandomTraversableType();

    // Constructors
    Tile();
    Tile(TileType type, const Position& pos);
    Tile(TileType type, int x, int y);

    // Destructor
    ~Tile();

    // Getters
    TileType GetType() const;
    Position GetPosition() const;
    char GetCharRepresentation() const;
    Color GetColor() const;
    bool IsTraversable() const;

    // Setters
    void SetType(TileType type);
    void SetPosition(const Position& pos);
    void SetPosition(int x, int y);

    // Rendering
    void Render(int screen_x, int screen_y, int tile_size) const;
    void RenderConsole() const;

    // Utility
    std::string GetTypeName() const;

private:
    TileType type_;
    Position position_;

    // Static texture storage
    static std::unordered_map<TileType, Texture2D> textures_;
    static bool textures_loaded_;

    // Helper methods
    char GetCharForType(TileType type) const;
    Color GetColorForType(TileType type) const;
    std::string GetTexturePathForType(TileType type) const;
    Texture2D GetTextureForType(TileType type) const;
};

#endif //RAYLIBSTARTER_TILE_H
