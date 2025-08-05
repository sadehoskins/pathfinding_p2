
#ifndef RAYLIBSTARTER_PATHFINDING_H
#define RAYLIBSTARTER_PATHFINDING_H


#include "Tile.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>

// Forward declaration
template<typename TileContainer> class Map;

// ******************** PATHFINDING NODE STRUCTURE ********************

struct PathNode {
    Position position;
    float g_cost;           // Distance from start
    float h_cost;           // Heuristic distance to goal
    float f_cost;           // g_cost + h_cost
    PathNode* parent;       // For path reconstruction

    PathNode(const Position& pos)
            : position(pos), g_cost(0), h_cost(0), f_cost(0), parent(nullptr) {}

    PathNode(const Position& pos, float g, float h, PathNode* p = nullptr)
            : position(pos), g_cost(g), h_cost(h), f_cost(g + h), parent(p) {}
};

// ******************** PATH RESULT STRUCTURE ********************

struct PathResult {
    std::vector<Position> path;
    float total_cost;
    int nodes_explored;
    bool path_found;

    PathResult() : total_cost(0), nodes_explored(0), path_found(false) {}
};

// ******************** PATHFINDING ALGORITHMS CLASS ********************

class Pathfinding {
public:
    // Constructor
    Pathfinding();

    // Destructor
    ~Pathfinding();

    // A* Algorithm Implementation
    template<typename TileContainer>
    PathResult FindPathAStar(const Position& start, const Position& goal,
                             const Map<TileContainer>& game_map);

    // Dijkstra algorithm implementation  (bonus -> for comparison)
    template<typename TileContainer>
    PathResult FindPathDijkstra(const Position& start, const Position& goal,
                                const Map<TileContainer>& game_map);

    // Utility methods
    void PrintPath(const PathResult& result) const;
    void PrintPathDetails(const PathResult& result) const;

    // Demonstration methods
    template<typename TileContainer>
    void DemoPathfinding(const Map<TileContainer>& game_map) const;

    template<typename TileContainer>
    void CompareAlgorithms(const Map<TileContainer>& game_map) const;

private:
    // Heuristic functions
    float CalculateHeuristic(const Position& from, const Position& to) const;
    float CalculateDistance(const Position& from, const Position& to) const;

    // Path reconstruction
    std::vector<Position> ReconstructPath(PathNode* goal_node) const;

    // Neighbor handling
    template<typename TileContainer>
    std::vector<Position> GetNeighbors(const Position& pos, const Map<TileContainer>& game_map) const;

    template<typename TileContainer>
    bool IsValidMove(const Position& pos, const Map<TileContainer>& game_map) const;

    // Utility helpers
    bool PositionsEqual(const Position& a, const Position& b) const;
    std::string PositionToString(const Position& pos) const;
};

// ******************** COMPARISON OPERATORS FOR PRIORITY QUEUE ********************

struct PathNodeComparator {
    bool operator()(const PathNode* a, const PathNode* b) const {
        return a->f_cost > b->f_cost; // Min-heap based on f_cost
    }
};

// ******************** POSITION HASH FUNCTION ********************

struct PositionHash {
    std::size_t operator()(const Position& pos) const {
        return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
    }
};

struct PositionEqual {
    bool operator()(const Position& a, const Position& b) const {
        return a.x == b.x && a.y == b.y;
    }
};


#endif //RAYLIBSTARTER_PATHFINDING_H
