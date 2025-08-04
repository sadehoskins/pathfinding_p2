#include "Tile.h"
#include "TextureManager.h"
#include <iostream>

// ******************** STATIC MEMBER DEFINITIONS ********************





// ******************** UTILITY FUNCTIONS ********************

bool Tile::IsBlockedType(TileType type) {
    return type >= TileType::BLOCKED_STONE && type <= TileType::BLOCKED_WATER;
}

bool Tile::IsTraversableType(TileType type) {
    return (type >= TileType::TRAVERSABLE_DIRT && type <= TileType::TRAVERSABLE_GRASS) ||
           IsTreasureChestType(type);
}

bool Tile::IsTreasureChestType(TileType type) {
    return type == TileType::TREASURE_CHEST_CLOSED || type == TileType::TREASURE_CHEST_OPENED;
}

TileType Tile::GetRandomBlockedType() {
    TileType blocked_types[] = {
            TileType::BLOCKED_STONE,
            TileType::BLOCKED_BUSHES,
            TileType::BLOCKED_TREE,
            TileType::BLOCKED_WATER
    };
    return blocked_types[GetRandomValue(0, 3)];
}

TileType Tile::GetRandomTraversableType() {
    TileType traversable_types[] = {
            TileType::TRAVERSABLE_DIRT,
            TileType::TRAVERSABLE_STONE,
            TileType::TRAVERSABLE_GRASS
    };
    return traversable_types[GetRandomValue(0, 2)];
}

// ******************** CONSTRUCTORS ********************

Tile::Tile() : type_(TileType::TRAVERSABLE_DIRT), position_(0, 0) {
    // Default constructor creates a traversable dirt tile at origin
}

Tile::Tile(TileType type, const Position& pos) : type_(type), position_(pos) {
    // Constructor with type and position
}

Tile::Tile(TileType type, int x, int y) : type_(type), position_(x, y) {
    // Constructor with type and coordinates
}

// ******************** DESTRUCTOR ********************

Tile::~Tile() {
    // No dynamic memory to clean up
}

// ******************** GETTERS ********************

TileType Tile::GetType() const {
    return type_;
}

Position Tile::GetPosition() const {
    return position_;
}

char Tile::GetCharRepresentation() const {
    return GetCharForType(type_);
}

Color Tile::GetColor() const {
    return GetColorForType(type_);
}

bool Tile::IsTraversable() const {
    return IsTraversableType(type_) || type_ == TileType::START || type_ == TileType::END;
}

bool Tile::IsTreasureChest() const {
    return IsTreasureChestType(type_);
}

bool Tile::IsClosedTreasureChest() const {
    return type_ == TileType::TREASURE_CHEST_CLOSED;
}

bool Tile::IsOpenTreasureChest() const {
    return type_ == TileType::TREASURE_CHEST_OPENED;
}

// ******************** SETTERS ********************

void Tile::SetType(TileType type) {
    type_ = type;
}

void Tile::SetPosition(const Position& pos) {
    position_ = pos;
}

void Tile::SetPosition(int x, int y) {
    position_ = Position(x, y);
}

// ******************** TREASURE CHEST OPERATIONS ********************

void Tile::OpenTreasureChest() {
    if (type_ == TileType::TREASURE_CHEST_CLOSED) {
        type_ = TileType::TREASURE_CHEST_OPENED;
    }
}

void Tile::CloseTreasureChest() {
    if (type_ == TileType::TREASURE_CHEST_OPENED) {
        type_ = TileType::TREASURE_CHEST_CLOSED;
    }
}

// ******************** RENDERING ********************

void Tile::Render(int screen_x, int screen_y, int tile_size) const {
    if (TextureManager::AreTexturesLoaded()) {  // **UPDATED** Use TextureManager
        // Use texture if available and loaded
        Texture2D texture = GetTextureForType(type_);
        if (texture.id != 0) {
            // Draw texture scaled to tile size
            Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
            Rectangle dest = {(float)screen_x, (float)screen_y, (float)tile_size, (float)tile_size};
            DrawTexturePro(texture, source, dest, {0, 0}, 0.0f, WHITE);

            // Draw border
            DrawRectangleLines(screen_x, screen_y, tile_size, tile_size, BLACK);
            return;
        }
    }

    // Fallback to colored rectangles if textures not available
    DrawRectangle(screen_x, screen_y, tile_size, tile_size, GetColor());
    DrawRectangleLines(screen_x, screen_y, tile_size, tile_size, BLACK);

    // Draw character representation in center
    const char* text = TextFormat("%c", GetCharRepresentation());
    int text_width = MeasureText(text, tile_size / 2);
    DrawText(text,
             screen_x + (tile_size - text_width) / 2,
             screen_y + tile_size / 4,
             tile_size / 2,
             BLACK);
}

void Tile::RenderConsole() const {
    std::cout << GetCharRepresentation();
}

// ******************** UTILITY ********************

std::string Tile::GetTypeName() const {
    switch (type_) {
        case TileType::START: return "Start";
        case TileType::END: return "End";
        case TileType::BLOCKED_STONE: return "Blocked Stone";
        case TileType::BLOCKED_BUSHES: return "Blocked Bushes";
        case TileType::BLOCKED_TREE: return "Blocked Tree";
        case TileType::BLOCKED_WATER: return "Blocked Water";
        case TileType::TRAVERSABLE_DIRT: return "Dirt Path";
        case TileType::TRAVERSABLE_STONE: return "Stone Tile";
        case TileType::TRAVERSABLE_GRASS: return "Grass";
        case TileType::TREASURE_CHEST_CLOSED: return "Treasure Chest (Closed)";
        case TileType::TREASURE_CHEST_OPENED: return "Treasure Chest (Opened)";
        default: return "Unknown";
    }
}

// ******************** PRIVATE HELPER METHODS ********************

char Tile::GetCharForType(TileType type) const {
    switch (type) {
        case TileType::START: return 's';
        case TileType::END: return 'e';
        case TileType::BLOCKED_STONE: return '#';
        case TileType::BLOCKED_BUSHES: return 'B';
        case TileType::BLOCKED_TREE: return 'T';
        case TileType::BLOCKED_WATER: return '~';
        case TileType::TRAVERSABLE_DIRT: return '.';
        case TileType::TRAVERSABLE_STONE: return 'o';
        case TileType::TRAVERSABLE_GRASS: return ',';
        case TileType::TREASURE_CHEST_CLOSED: return 't';
        case TileType::TREASURE_CHEST_OPENED: return 'O';
        default: return '?';
    }
}

Color Tile::GetColorForType(TileType type) const {
    switch (type) {
        case TileType::START: return GREEN;
        case TileType::END: return RED;
        case TileType::BLOCKED_STONE: return GRAY;
        case TileType::BLOCKED_BUSHES: return DARKGREEN;
        case TileType::BLOCKED_TREE: return BROWN;
        case TileType::BLOCKED_WATER: return BLUE;
        case TileType::TRAVERSABLE_DIRT: return BEIGE;
        case TileType::TRAVERSABLE_STONE: return LIGHTGRAY;
        case TileType::TRAVERSABLE_GRASS: return LIME;
        case TileType::TREASURE_CHEST_CLOSED: return GOLD;
        case TileType::TREASURE_CHEST_OPENED: return ORANGE;
        default: return MAGENTA;
    }
}

/*std::string Tile::GetTexturePathForType(TileType type) const {
    switch (type) {
        case TileType::START: return "assets/graphics/tiles/start.png";
        case TileType::END: return "assets/graphics/tiles/end.png";
        case TileType::BLOCKED_STONE: return "assets/graphics/tiles/blocked/stone.png";
        case TileType::BLOCKED_BUSHES: return "assets/graphics/tiles/blocked/bushes.png";
        case TileType::BLOCKED_TREE: return "assets/graphics/tiles/blocked/tree.png";
        case TileType::BLOCKED_WATER: return "assets/graphics/tiles/blocked/water.png";
        case TileType::TRAVERSABLE_DIRT: return "assets/graphics/tiles/traversable/dirt_path.png";
        case TileType::TRAVERSABLE_STONE: return "assets/graphics/tiles/traversable/stone_tile.png";
        case TileType::TRAVERSABLE_GRASS: return "assets/graphics/tiles/traversable/grass.png";
        case TileType::TREASURE_CHEST_CLOSED: return "assets/graphics/items/treasure_chest_closed.png";
        case TileType::TREASURE_CHEST_OPENED: return "assets/graphics/items/treasure_chest_opened.png";
        default: return "";
    }
}*/

Texture2D Tile::GetTextureForType(TileType type) const {
    switch (type) {
        case TileType::START: return TextureManager::GetTileTexture("start");
        case TileType::END: return TextureManager::GetTileTexture("end");
        case TileType::BLOCKED_STONE: return TextureManager::GetTileTexture("stone");
        case TileType::BLOCKED_BUSHES: return TextureManager::GetTileTexture("bushes");
        case TileType::BLOCKED_TREE: return TextureManager::GetTileTexture("tree");
        case TileType::BLOCKED_WATER: return TextureManager::GetTileTexture("water");
        case TileType::TRAVERSABLE_DIRT: return TextureManager::GetTileTexture("dirt_path");
        case TileType::TRAVERSABLE_STONE: return TextureManager::GetTileTexture("stone_tile");
        case TileType::TRAVERSABLE_GRASS: return TextureManager::GetTileTexture("grass");
        case TileType::TREASURE_CHEST_CLOSED: return TextureManager::GetTileTexture("treasure_chest_closed");
        case TileType::TREASURE_CHEST_OPENED: return TextureManager::GetTileTexture("treasure_chest_opened");
        default: return Texture2D{0};
    }
}