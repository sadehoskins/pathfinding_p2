//
// Created by sadeh on 8/2/2025.
//

#include "Tile.h"
#include <iostream>

// ******************** CONSTRUCTORS ********************

Tile::Tile() : type_(TileType::TRAVERSABLE), position_(0, 0) {
    // Default constructor creates a traversable tile at origin
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
    return type_ == TileType::START ||
           type_ == TileType::END ||
           type_ == TileType::TRAVERSABLE;
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
    // Draw filled rectangle for the tile
    DrawRectangle(screen_x, screen_y, tile_size, tile_size, GetColor());

    // Draw border
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
        case TileType::BLOCKED: return "Blocked";
        case TileType::TRAVERSABLE: return "Traversable";
        default: return "Unknown";
    }
}

// ******************** PRIVATE HELPER METHODS ********************

char Tile::GetCharForType(TileType type) const {
    switch (type) {
        case TileType::START: return 's';
        case TileType::END: return 'e';
        case TileType::BLOCKED: return 'b';
        case TileType::TRAVERSABLE: return 'x';
        default: return '?';
    }
}

Color Tile::GetColorForType(TileType type) const {
    switch (type) {
        case TileType::START: return GREEN;
        case TileType::END: return RED;
        case TileType::BLOCKED: return DARKGRAY;
        case TileType::TRAVERSABLE: return LIGHTGRAY;
        default: return MAGENTA;
    }
}