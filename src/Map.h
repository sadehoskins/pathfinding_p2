//
// Created by sadeh on 8/2/2025.
//

#ifndef RAYLIBSTARTER_MAP_H
#define RAYLIBSTARTER_MAP_H

#include "Tile.h"
#include "raylib.h"
#include <vector>
#include <memory>
#include <iostream>

// ******************** TEMPLATED MAP CLASS ********************

template<typename TileContainer = std::vector<std::vector<Tile>>>
class Map {
public:
    // Constructors
    Map();
    Map(int width, int height);

    // Destructor
    ~Map();

    // Map Generation
    void GenerateStaticMap();
    void GenerateRandomMap();

    // Tile Access
    Tile& GetTile(int x, int y);
    const Tile& GetTile(int x, int y) const;
    Tile& GetTile(const Position& pos);
    const Tile& GetTile(const Position& pos) const;

    // Map Properties
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    Position GetStartPosition() const { return start_pos_; }
    Position GetEndPosition() const { return end_pos_; }

    // Validation
    bool IsValidPosition(int x, int y) const;
    bool IsValidPosition(const Position& pos) const;
    bool HasValidPath() const;

    // Rendering
    void Render(int offset_x, int offset_y, int tile_size) const;
    void RenderConsole() const;

    // Utility
    void PrintMapInfo() const;

private:
    TileContainer tiles_;
    int width_;
    int height_;
    Position start_pos_;
    Position end_pos_;

    // Helper methods
    void InitializeMap();
    void PlaceStartAndEnd();
    void GenerateBlockedTiles(float blocked_ratio = 0.3f);
    bool ValidatePathExists() const;
    bool DepthFirstSearch(Position current, Position target,
                          std::vector<std::vector<bool>>& visited) const;
    void EnsurePathExists();
};

// ******************** TEMPLATE IMPLEMENTATION ********************

template<typename TileContainer>
Map<TileContainer>::Map() : width_(15), height_(15) {
    InitializeMap();
    GenerateStaticMap();
}

template<typename TileContainer>
Map<TileContainer>::Map(int width, int height) : width_(width), height_(height) {
    if (width_ < 15) width_ = 15;
    if (height_ < 15) height_ = 15;

    InitializeMap();
    GenerateStaticMap();
}

template<typename TileContainer>
Map<TileContainer>::~Map() {
    // No dynamic cleanup needed for std::vector
}

template<typename TileContainer>
void Map<TileContainer>::InitializeMap() {
    tiles_.resize(height_);
    for (int y = 0; y < height_; ++y) {
        tiles_[y].resize(width_);
        for (int x = 0; x < width_; ++x) {
            tiles_[y][x] = Tile(TileType::TRAVERSABLE, x, y);
        }
    }
}

template<typename TileContainer>
void Map<TileContainer>::GenerateStaticMap() {
    PlaceStartAndEnd();

    // Add some blocked tiles in a pattern
    for (int y = 1; y < height_ - 1; ++y) {
        for (int x = 0; x < width_; ++x) {
            // Skip start and end positions
            if (Position(x, y) == start_pos_ || Position(x, y) == end_pos_) {
                continue;
            }

            // Create some blocked patterns (but not too many)
            if ((x + y) % 7 == 0 && (x % 3 != 0)) {
                tiles_[y][x].SetType(TileType::BLOCKED);
            }
        }
    }

    EnsurePathExists();
}

template<typename TileContainer>
void Map<TileContainer>::GenerateRandomMap() {
    PlaceStartAndEnd();
    GenerateBlockedTiles();
    EnsurePathExists();
}

template<typename TileContainer>
void Map<TileContainer>::PlaceStartAndEnd() {
    // Start on first row
    int start_x = GetRandomValue(0, width_ - 1);
    start_pos_ = Position(start_x, 0);
    tiles_[0][start_x].SetType(TileType::START);

    // End on last row
    int end_x = GetRandomValue(0, width_ - 1);
    end_pos_ = Position(end_x, height_ - 1);
    tiles_[height_ - 1][end_x].SetType(TileType::END);
}

template<typename TileContainer>
void Map<TileContainer>::GenerateBlockedTiles(float blocked_ratio) {
    int total_tiles = width_ * height_;
    int blocked_count = static_cast<int>(total_tiles * blocked_ratio);

    for (int i = 0; i < blocked_count; ++i) {
        int x = GetRandomValue(0, width_ - 1);
        int y = GetRandomValue(1, height_ - 2); // Avoid first and last row

        // Don't block start or end positions
        if (Position(x, y) == start_pos_ || Position(x, y) == end_pos_) {
            continue;
        }

        tiles_[y][x].SetType(TileType::BLOCKED);
    }
}

template<typename TileContainer>
void Map<TileContainer>::EnsurePathExists() {
    // Simple path creation - make sure there's at least one path down the middle
    int middle_x = width_ / 2;
    for (int y = 0; y < height_; ++y) {
        if (tiles_[y][middle_x].GetType() == TileType::BLOCKED) {
            tiles_[y][middle_x].SetType(TileType::TRAVERSABLE);
        }
    }
}

template<typename TileContainer>
Tile& Map<TileContainer>::GetTile(int x, int y) {
    return tiles_[y][x];
}

template<typename TileContainer>
const Tile& Map<TileContainer>::GetTile(int x, int y) const {
    return tiles_[y][x];
}

template<typename TileContainer>
Tile& Map<TileContainer>::GetTile(const Position& pos) {
    return GetTile(pos.x, pos.y);
}

template<typename TileContainer>
const Tile& Map<TileContainer>::GetTile(const Position& pos) const {
    return GetTile(pos.x, pos.y);
}

template<typename TileContainer>
bool Map<TileContainer>::IsValidPosition(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

template<typename TileContainer>
bool Map<TileContainer>::IsValidPosition(const Position& pos) const {
    return IsValidPosition(pos.x, pos.y);
}

template<typename TileContainer>
bool Map<TileContainer>::HasValidPath() const {
    return ValidatePathExists();
}

template<typename TileContainer>
bool Map<TileContainer>::ValidatePathExists() const {
    std::vector<std::vector<bool>> visited(height_, std::vector<bool>(width_, false));
    return DepthFirstSearch(start_pos_, end_pos_, visited);
}

template<typename TileContainer>
bool Map<TileContainer>::DepthFirstSearch(Position current, Position target,
                                          std::vector<std::vector<bool>>& visited) const {
    if (current == target) {
        return true;
    }

    visited[current.y][current.x] = true;

    // Check all 4 directions
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    for (int i = 0; i < 4; ++i) {
        int new_x = current.x + dx[i];
        int new_y = current.y + dy[i];
        Position new_pos(new_x, new_y);

        if (IsValidPosition(new_pos) && !visited[new_y][new_x]) {
            const Tile& tile = GetTile(new_pos);

            if (tile.IsTraversable()) {
                if (DepthFirstSearch(new_pos, target, visited)) {
                    return true;
                }
            }
        }
    }

    return false;
}

template<typename TileContainer>
void Map<TileContainer>::Render(int offset_x, int offset_y, int tile_size) const {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            int screen_x = offset_x + (x * tile_size);
            int screen_y = offset_y + (y * tile_size);
            tiles_[y][x].Render(screen_x, screen_y, tile_size);
        }
    }
}

template<typename TileContainer>
void Map<TileContainer>::RenderConsole() const {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            tiles_[y][x].RenderConsole();
        }
        std::cout << std::endl;
    }
    std::cout << std::flush;
}

template<typename TileContainer>
void Map<TileContainer>::PrintMapInfo() const {
    std::cout << "Map Info:" << std::endl;
    std::cout << "Size: " << width_ << "x" << height_ << std::endl;
    std::cout << "Start: (" << start_pos_.x << ", " << start_pos_.y << ")" << std::endl;
    std::cout << "End: (" << end_pos_.x << ", " << end_pos_.y << ")" << std::endl;
    std::cout << "Valid path exists: " << (HasValidPath() ? "Yes" : "No") << std::endl;
}

#endif //RAYLIBSTARTER_MAP_H
