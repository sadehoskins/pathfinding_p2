#include "Tile.h"
#include <iostream>

// ******************** STATIC MEMBER DEFINITIONS ********************

std::unordered_map<TileType, Texture2D> Tile::textures_;
bool Tile::textures_loaded_ = false;

// ******************** STATIC TEXTURE MANAGEMENT ********************

void Tile::LoadAllTextures() {
    if (textures_loaded_) {
        std::cout << "Textures already loaded, skipping..." << std::endl;
        return;
    }

    std::cout << "Loading tile textures..." << std::endl;

    // Load textures for each tile type
    textures_[TileType::START] = LoadTexture("assets/graphics/start.png");
    textures_[TileType::END] = LoadTexture("assets/graphics/end.png");

    // Blocked variants
    textures_[TileType::BLOCKED_STONE] = LoadTexture("assets/graphics/blocked/stone.png");
    textures_[TileType::BLOCKED_BUSHES] = LoadTexture("assets/graphics/blocked/bushes.png");
    textures_[TileType::BLOCKED_TREE] = LoadTexture("assets/graphics/blocked/tree.png");
    textures_[TileType::BLOCKED_WATER] = LoadTexture("assets/graphics/blocked/water.png");

    // Traversable variants
    textures_[TileType::TRAVERSABLE_DIRT] = LoadTexture("assets/graphics/traversable/dirt_path.png");
    textures_[TileType::TRAVERSABLE_STONE] = LoadTexture("assets/graphics/traversable/stone_tile.png");
    textures_[TileType::TRAVERSABLE_GRASS] = LoadTexture("assets/graphics/traversable/grass.png");

    // Check if textures loaded successfully
    for (const auto& pair : textures_) {
        if (pair.second.id == 0) {
            std::cout << "Warning: Failed to load texture for tile type "
                      << static_cast<int>(pair.first) << std::endl;
        }
    }

    textures_loaded_ = true;
    std::cout << "Tile textures loaded successfully!" << std::endl;
}

void Tile::UnloadAllTextures() {
    if (!textures_loaded_) {
        return;
    }

    std::cout << "Unloading tile textures..." << std::endl;

    for (auto& pair : textures_) {
        if (pair.second.id != 0) {
            UnloadTexture(pair.second);
        }
    }

    textures_.clear();
    textures_loaded_ = false;
    std::cout << "Tile textures unloaded successfully!" << std::endl;
}

bool Tile::AreTexturesLoaded() {
    return textures_loaded_;
}

// ******************** UTILITY FUNCTIONS ********************

bool Tile::IsBlockedType(TileType type) {
    return type >= TileType::BLOCKED_STONE && type <= TileType::BLOCKED_WATER;
}

bool Tile::IsTraversableType(TileType type) {
    return type >= TileType::TRAVERSABLE_DIRT && type <= TileType::TRAVERSABLE_GRASS;
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

// ******************** RENDERING ********************

void Tile::Render(int screen_x, int screen_y, int tile_size) const {
    if (textures_loaded_) {
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
        default: return MAGENTA;
    }
}

std::string Tile::GetTexturePathForType(TileType type) const {
    switch (type) {
        case TileType::START: return "assets/graphics/start.png";
        case TileType::END: return "assets/graphics/end.png";
        case TileType::BLOCKED_STONE: return "assets/graphics/blocked/stone.png";
        case TileType::BLOCKED_BUSHES: return "assets/graphics/blocked/bushes.png";
        case TileType::BLOCKED_TREE: return "assets/graphics/blocked/tree.png";
        case TileType::BLOCKED_WATER: return "assets/graphics/blocked/water.png";
        case TileType::TRAVERSABLE_DIRT: return "assets/graphics/traversable/dirt_path.png";
        case TileType::TRAVERSABLE_STONE: return "assets/graphics/traversable/stone_tile.png";
        case TileType::TRAVERSABLE_GRASS: return "assets/graphics/traversable/grass.png";
        default: return "";
    }
}

Texture2D Tile::GetTextureForType(TileType type) const {
    auto it = textures_.find(type);
    if (it != textures_.end()) {
        return it->second;
    }
    // Return empty texture if not found
    return Texture2D{0};
}