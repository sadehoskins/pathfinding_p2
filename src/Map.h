#ifndef RAYLIBSTARTER_MAP_H
#define RAYLIBSTARTER_MAP_H

#include "Tile.h"
#include "items/ItemManager.h"
#include "raylib.h"
#include "TextureManager.h"
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
    void GenerateTerrainWithClustering();

    // Tile Access
    Tile& GetTile(int x, int y);
    const Tile& GetTile(int x, int y) const;
    Tile& GetTile(const Position& pos);
    const Tile& GetTile(const Position& pos) const;

    // Item System Access
    ItemManager& GetItemManager() { return item_manager_; }
    const ItemManager& GetItemManager() const { return item_manager_; }
    bool HasTreasureChestAt(const Position& pos) const;
    bool HasItemsAt(const Position& pos) const;
    void OpenTreasureChestAt(const Position& pos);

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

    // Item management
    ItemManager item_manager_;

    // Helper methods
    void InitializeMap();
    void PlaceStartAndEnd();
    void GenerateBlockedTiles(float blocked_ratio = 0.3f);
    void GenerateClusteredTerrain();
    void PlaceWaterClusters();
    void PlaceVegetationClusters();
    void FillTraversableAreas();

    // Item and treasure chest placement
    void PlaceItemsAndTreasureChests();
    void PlaceTreasureChests();

    int CountBlockedNeighbors(int x, int y) const;
    int CountWaterNeighbors(int x, int y) const;
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
            tiles_[y][x] = Tile(TileType::TRAVERSABLE_DIRT, x, y);
        }
    }
}

template<typename TileContainer>
void Map<TileContainer>::GenerateStaticMap() {
    InitializeMap();
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
                tiles_[y][x].SetType(Tile::GetRandomBlockedType());
            }
        }
    }

    EnsurePathExists();
    PlaceItemsAndTreasureChests();  // Ensure this is called!
}

template<typename TileContainer>
void Map<TileContainer>::GenerateRandomMap() {
    InitializeMap();
    PlaceStartAndEnd();
    GenerateBlockedTiles();
    EnsurePathExists();
    PlaceItemsAndTreasureChests();  // Ensure this is called!
}

template<typename TileContainer>
void Map<TileContainer>::GenerateTerrainWithClustering() {
    PlaceStartAndEnd();
    GenerateClusteredTerrain();
    EnsurePathExists();
    PlaceItemsAndTreasureChests();
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

        tiles_[y][x].SetType(Tile::GetRandomBlockedType());
    }
}

template<typename TileContainer>
void Map<TileContainer>::PlaceStartAndEnd() {
    // Clear any existing start/end tiles first
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (tiles_[y][x].GetType() == TileType::START ||
                tiles_[y][x].GetType() == TileType::END) {
                tiles_[y][x].SetType(TileType::TRAVERSABLE_DIRT);
            }
        }
    }

    // Place start on first row
    int start_x = GetRandomValue(0, width_ - 1);
    start_pos_ = Position(start_x, 0);
    tiles_[0][start_x].SetType(TileType::START);

    // Place end on last row
    int end_x = GetRandomValue(0, width_ - 1);
    end_pos_ = Position(end_x, height_ - 1);
    tiles_[height_ - 1][end_x].SetType(TileType::END);
}

template<typename TileContainer>
void Map<TileContainer>::GenerateClusteredTerrain() {
    // Place water clusters
    PlaceWaterClusters();

    // Place vegetation clusters (trees/bushes)
    PlaceVegetationClusters();

    // Fill remaining traversable areas with different tiles
    FillTraversableAreas();
}

template<typename TileContainer>
void Map<TileContainer>::PlaceWaterClusters() {
    int water_clusters = GetRandomValue(2, 4);

    for (int cluster = 0; cluster < water_clusters; ++cluster) {
        // Pick random starting point (avoid first and last rows)
        int start_x = GetRandomValue(1, width_ - 2);
        int start_y = GetRandomValue(2, height_ - 3);

        // Skip if start/end positions
        if (Position(start_x, start_y) == start_pos_ || Position(start_x, start_y) == end_pos_) {
            continue;
        }

        // Create small water cluster (2-4 tiles)
        int cluster_size = GetRandomValue(2, 4);
        std::vector<Position> cluster_positions;
        cluster_positions.push_back(Position(start_x, start_y));

        for (int i = 1; i < cluster_size; ++i) {
            // Try to place water near existing water
            bool placed = false;
            for (int attempts = 0; attempts < 10 && !placed; ++attempts) {
                Position base = cluster_positions[GetRandomValue(0, cluster_positions.size() - 1)];
                int new_x = base.x + GetRandomValue(-1, 1);
                int new_y = base.y + GetRandomValue(-1, 1);

                if (IsValidPosition(new_x, new_y) &&
                    Position(new_x, new_y) != start_pos_ &&
                    Position(new_x, new_y) != end_pos_) {

                    cluster_positions.push_back(Position(new_x, new_y));
                    placed = true;
                }
            }
        }

        // Place the water tiles
        for (const Position& pos : cluster_positions) {
            tiles_[pos.y][pos.x].SetType(TileType::BLOCKED_WATER);
        }
    }
}

template<typename TileContainer>
void Map<TileContainer>::PlaceVegetationClusters() {
    int vegetation_clusters = GetRandomValue(3, 6);

    for (int cluster = 0; cluster < vegetation_clusters; ++cluster) {
        int start_x = GetRandomValue(0, width_ - 1);
        int start_y = GetRandomValue(1, height_ - 2);

        // Skip if start/end positions or already water
        if (Position(start_x, start_y) == start_pos_ ||
            Position(start_x, start_y) == end_pos_ ||
            tiles_[start_y][start_x].GetType() == TileType::BLOCKED_WATER) {
            continue;
        }

        // Choose vegetation type for this cluster
        TileType vegetation_type = (GetRandomValue(0, 1) == 0) ?
                                   TileType::BLOCKED_TREE : TileType::BLOCKED_BUSHES;

        // Create small vegetation cluster
        int cluster_size = GetRandomValue(1, 3);
        tiles_[start_y][start_x].SetType(vegetation_type);

        // Add nearby vegetation
        for (int i = 1; i < cluster_size; ++i) {
            int new_x = start_x + GetRandomValue(-1, 1);
            int new_y = start_y + GetRandomValue(-1, 1);

            if (IsValidPosition(new_x, new_y) &&
                Position(new_x, new_y) != start_pos_ &&
                Position(new_x, new_y) != end_pos_ &&
                !Tile::IsBlockedType(tiles_[new_y][new_x].GetType())) {

                tiles_[new_y][new_x].SetType(vegetation_type);
            }
        }
    }

    // Scatter some stone blocks
    int stone_count = GetRandomValue(width_ * height_ / 20, width_ * height_ / 15);
    for (int i = 0; i < stone_count; ++i) {
        int x = GetRandomValue(0, width_ - 1);
        int y = GetRandomValue(1, height_ - 2);

        if (Position(x, y) != start_pos_ &&
            Position(x, y) != end_pos_ &&
            !Tile::IsBlockedType(tiles_[y][x].GetType())) {

            tiles_[y][x].SetType(TileType::BLOCKED_STONE);
        }
    }
}

template<typename TileContainer>
void Map<TileContainer>::FillTraversableAreas() {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            // Skip if already blocked or start/end
            if (Tile::IsBlockedType(tiles_[y][x].GetType()) ||
                tiles_[y][x].GetType() == TileType::START ||
                tiles_[y][x].GetType() == TileType::END) {
                continue;
            }

            // Count blocked neighbors to determine terrain preference
            int blocked_neighbors = CountBlockedNeighbors(x, y);

            if (blocked_neighbors == 0) {
                // Open area - prefer grass
                if (GetRandomValue(0, 100) < 60) {
                    tiles_[y][x].SetType(TileType::TRAVERSABLE_GRASS);
                } else if (GetRandomValue(0, 100) < 30) {
                    tiles_[y][x].SetType(TileType::TRAVERSABLE_STONE);
                } else {
                    tiles_[y][x].SetType(TileType::TRAVERSABLE_DIRT);
                }
            } else if (blocked_neighbors <= 2) {
                // Some neighbors - mixed terrain
                if (GetRandomValue(0, 100) < 40) {
                    tiles_[y][x].SetType(TileType::TRAVERSABLE_DIRT);
                } else if (GetRandomValue(0, 100) < 35) {
                    tiles_[y][x].SetType(TileType::TRAVERSABLE_STONE);
                } else {
                    tiles_[y][x].SetType(TileType::TRAVERSABLE_GRASS);
                }
            } else {
                // Many blocked neighbors - prefer dirt paths
                if (GetRandomValue(0, 100) < 70) {
                    tiles_[y][x].SetType(TileType::TRAVERSABLE_DIRT);
                } else {
                    tiles_[y][x].SetType(TileType::TRAVERSABLE_STONE);
                }
            }
        }
    }
}

// Item and treasure chest placement methods
template<typename TileContainer>
void Map<TileContainer>::PlaceItemsAndTreasureChests() {
    // Generate items using ItemManager
    item_manager_.GenerateItemsForMap(width_, height_, 5);

    // Place treasure chest tiles where items are marked as chest items
    PlaceTreasureChests();
}

template<typename TileContainer>
void Map<TileContainer>::PlaceTreasureChests() {
    // Get treasure chest positions from ItemManager
    auto chest_positions = ItemManager::GetTreasureChestPositions();

    for (const Position& pos : chest_positions) {
        if (IsValidPosition(pos) &&
            tiles_[pos.y][pos.x].IsTraversable() &&
            !tiles_[pos.y][pos.x].IsTreasureChest()) {

            tiles_[pos.y][pos.x].SetType(TileType::TREASURE_CHEST_CLOSED);
        }
    }
}

// Item system access methods
template<typename TileContainer>
bool Map<TileContainer>::HasTreasureChestAt(const Position& pos) const {
    if (!IsValidPosition(pos)) return false;
    return tiles_[pos.y][pos.x].IsTreasureChest();
}

template<typename TileContainer>
bool Map<TileContainer>::HasItemsAt(const Position& pos) const {
    return item_manager_.GetItemCountAtPosition(pos) > 0;
}

template<typename TileContainer>
void Map<TileContainer>::OpenTreasureChestAt(const Position& pos) {
    if (IsValidPosition(pos) && tiles_[pos.y][pos.x].IsClosedTreasureChest()) {
        tiles_[pos.y][pos.x].OpenTreasureChest();
    }
}

template<typename TileContainer>
int Map<TileContainer>::CountBlockedNeighbors(int x, int y) const {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue; // Skip center tile

            int nx = x + dx;
            int ny = y + dy;

            if (IsValidPosition(nx, ny) && Tile::IsBlockedType(tiles_[ny][nx].GetType())) {
                count++;
            }
        }
    }
    return count;
}

template<typename TileContainer>
int Map<TileContainer>::CountWaterNeighbors(int x, int y) const {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;

            int nx = x + dx;
            int ny = y + dy;

            if (IsValidPosition(nx, ny) && tiles_[ny][nx].GetType() == TileType::BLOCKED_WATER) {
                count++;
            }
        }
    }
    return count;
}

template<typename TileContainer>
void Map<TileContainer>::EnsurePathExists() {
    // Simple path creation (at least one path down the middle)
    int middle_x = width_ / 2;
    for (int y = 0; y < height_; ++y) {
        if (Tile::IsBlockedType(tiles_[y][middle_x].GetType())) {
            tiles_[y][middle_x].SetType(TileType::TRAVERSABLE_DIRT);
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
    // Render all tiles first
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            int screen_x = offset_x + (x * tile_size);
            int screen_y = offset_y + (y * tile_size);
            tiles_[y][x].Render(screen_x, screen_y, tile_size);
        }
    }

    // Render sparkle effects for hidden items
    if (TextureManager::AreTexturesLoaded()) {
        Texture2D sparkle_texture = TextureManager::GetUITexture("sparkle");

        for (const auto& item_with_pos : item_manager_.GetAllItems()) {
            if (!item_with_pos.is_in_treasure_chest) { // Only render hidden items
                int item_screen_x = offset_x + (item_with_pos.position.x * tile_size);
                int item_screen_y = offset_y + (item_with_pos.position.y * tile_size);

                if (sparkle_texture.id != 0) {
                    // Sparkle texture -> Draw sparkle with animated effect

                    // Create pulsing/glowing effect based on time
                    float time = GetTime();
                    float pulse = (sin(time * 3.0f) + 1.0f) * 0.5f; // 0.0 to 1.0
                    float alpha = 0.6f + (pulse * 0.4f); // 0.6 to 1.0

                    // Color based on rarity
                    Color sparkle_color = WHITE;
                    switch (item_with_pos.item->GetRarity()) {
                        case ItemRarity::COMMON:
                            sparkle_color = ColorAlpha(LIGHTGRAY, alpha);
                            break;
                        case ItemRarity::UNCOMMON:
                            sparkle_color = ColorAlpha(GREEN, alpha);
                            break;
                        case ItemRarity::RARE:
                            sparkle_color = ColorAlpha(SKYBLUE, alpha);
                            break;
                        case ItemRarity::LEGENDARY:
                            sparkle_color = ColorAlpha(GOLD, alpha);
                            break;
                    }

                    // Draw sparkle texture centered on tile
                    int sparkle_size = tile_size / 2; // Half the tile size
                    int sparkle_x = item_screen_x + (tile_size - sparkle_size) / 2;
                    int sparkle_y = item_screen_y + (tile_size - sparkle_size) / 2;

                    Rectangle source = {0, 0, (float)sparkle_texture.width, (float)sparkle_texture.height};
                    Rectangle dest = {(float)sparkle_x, (float)sparkle_y, (float)sparkle_size, (float)sparkle_size};

                    // Add slight rotation for more dynamic effect
                    float rotation = time * 30.0f; // Slow rotation
                    Vector2 origin = {sparkle_size / 2.0f, sparkle_size / 2.0f};

                    DrawTexturePro(sparkle_texture, source, dest, origin, rotation, sparkle_color);

                } else {
                    // Fallback - Simple colored circle if no sparkle texture
                    Color item_color = WHITE;
                    switch (item_with_pos.item->GetRarity()) {
                        case ItemRarity::COMMON: item_color = LIGHTGRAY; break;
                        case ItemRarity::UNCOMMON: item_color = GREEN; break;
                        case ItemRarity::RARE: item_color = BLUE; break;
                        case ItemRarity::LEGENDARY: item_color = GOLD; break;
                    }

                    // Draw pulsing circle
                    float time = GetTime();
                    float pulse = (sin(time * 4.0f) + 1.0f) * 0.5f;
                    float radius = 3.0f + (pulse * 2.0f);

                    DrawCircle(item_screen_x + tile_size / 2,
                               item_screen_y + tile_size / 2,
                               radius, ColorAlpha(item_color, 0.8f));
                }
            }
        }
    } else {
        // Fallback - Simple indicators if textures not loaded
        for (const auto& item_with_pos : item_manager_.GetAllItems()) {
            if (!item_with_pos.is_in_treasure_chest) {
                int item_screen_x = offset_x + (item_with_pos.position.x * tile_size);
                int item_screen_y = offset_y + (item_with_pos.position.y * tile_size);

                Color item_color = WHITE;
                switch (item_with_pos.item->GetRarity()) {
                    case ItemRarity::COMMON: item_color = LIGHTGRAY; break;
                    case ItemRarity::UNCOMMON: item_color = GREEN; break;
                    case ItemRarity::RARE: item_color = BLUE; break;
                    case ItemRarity::LEGENDARY: item_color = GOLD; break;
                }

                DrawCircle(item_screen_x + tile_size - 8, item_screen_y + 8, 4, item_color);
                DrawText("*", item_screen_x + tile_size - 6, item_screen_y + 4, 8, BLACK);
            }
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
    std::cout << "Total items: " << item_manager_.GetTotalItemCount() << std::endl;
    std::cout << "Treasure chests: " << ItemManager::GetTreasureChestPositions().size() << std::endl;
}

#endif //RAYLIBSTARTER_MAP_H