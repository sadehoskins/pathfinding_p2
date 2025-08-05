#include "Pathfinding.h"
#include "Map.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>

// ******************** CONSTRUCTOR & DESTRUCTOR ********************

Pathfinding::Pathfinding() {
    std::cout << "Pathfinding system initialized." << std::endl;
}

Pathfinding::~Pathfinding() {
    // No dynamic cleanup needed
}

// ******************** A* ALGORITHM IMPLEMENTATION ********************

template<typename TileContainer>
PathResult Pathfinding::FindPathAStar(const Position& start, const Position& goal, const Map<TileContainer>& game_map) {
    auto start_time = std::chrono::high_resolution_clock::now();

    PathResult result;
    result.nodes_explored = 0;

    // Validate start and goal positions
    if (!game_map.IsValidPosition(start) || !game_map.IsValidPosition(goal)) {
        std::cout << "Invalid start or goal position!" << std::endl;
        return result;
    }

    if (!game_map.GetTile(start).IsTraversable() || !game_map.GetTile(goal).IsTraversable()) {
        std::cout << "Start or goal position is not traversable!" << std::endl;
        return result;
    }

    // Priority queue for open set (nodes to explore)
    std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator> open_set;

    // Hash maps for tracking visited nodes and costs
    std::unordered_map<Position, PathNode*, PositionHash, PositionEqual> all_nodes;
    std::unordered_map<Position, float, PositionHash, PositionEqual> g_costs;

    // Create and add start node
    PathNode* start_node = new PathNode(start, 0, CalculateHeuristic(start, goal));
    open_set.push(start_node);
    all_nodes[start] = start_node;
    g_costs[start] = 0;

    std::cout << "\n=== A* PATHFINDING ===" << std::endl;
    std::cout << "Start: (" << start.x << ", " << start.y << ")" << std::endl;
    std::cout << "Goal: (" << goal.x << ", " << goal.y << ")" << std::endl;
    std::cout << "Searching..." << std::endl;

    while (!open_set.empty()) {
        // Get node with lowest f_cost
        PathNode* current = open_set.top();
        open_set.pop();
        result.nodes_explored++;

        // Check if we reached the goal
        if (PositionsEqual(current->position, goal)) {
            std::cout << "Path found!" << std::endl;
            result.path = ReconstructPath(current);
            result.total_cost = current->g_cost;
            result.path_found = true;

            // Cleanup memory
            for (auto& pair : all_nodes) {
                delete pair.second;
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            std::cout << "A* completed in " << duration.count() << " microseconds" << std::endl;

            return result;
        }

        // Explore neighbors
        std::vector<Position> neighbors = GetNeighbors(current->position, game_map);

        for (const Position& neighbor_pos : neighbors) {
            if (!IsValidMove(neighbor_pos, game_map)) {
                continue;
            }

            float tentative_g_cost = current->g_cost + CalculateDistance(current->position, neighbor_pos);

            // Check if we found a better path to this neighbor
            auto g_cost_it = g_costs.find(neighbor_pos);
            if (g_cost_it != g_costs.end() && tentative_g_cost >= g_cost_it->second) {
                continue; // Not a better path
            }

            // Create or update neighbor node
            PathNode* neighbor_node;
            auto node_it = all_nodes.find(neighbor_pos);
            if (node_it == all_nodes.end()) {
                neighbor_node = new PathNode(neighbor_pos);
                all_nodes[neighbor_pos] = neighbor_node;
            } else {
                neighbor_node = node_it->second;
            }

            // Update neighbor with better path
            neighbor_node->g_cost = tentative_g_cost;
            neighbor_node->h_cost = CalculateHeuristic(neighbor_pos, goal);
            neighbor_node->f_cost = neighbor_node->g_cost + neighbor_node->h_cost;
            neighbor_node->parent = current;

            g_costs[neighbor_pos] = tentative_g_cost;
            open_set.push(neighbor_node);
        }
    }

    // No path found
    std::cout << "No path found to goal!" << std::endl;

    // Cleanup memory
    for (auto& pair : all_nodes) {
        delete pair.second;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "A* completed in " << duration.count() << " microseconds (no path)" << std::endl;

    return result;
}

// ******************** DIJKSTRA ALGORITHM IMPLEMENTATION ********************

template<typename TileContainer>
PathResult Pathfinding::FindPathDijkstra(const Position& start, const Position& goal, const Map<TileContainer>& game_map) {
    auto start_time = std::chrono::high_resolution_clock::now();

    PathResult result;
    result.nodes_explored = 0;

    // Validate positions
    if (!game_map.IsValidPosition(start) || !game_map.IsValidPosition(goal)) {
        return result;
    }

    if (!game_map.GetTile(start).IsTraversable() || !game_map.GetTile(goal).IsTraversable()) {
        return result;
    }

    // Priority queue for Dijkstra (only uses g_cost, no heuristic)
    std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator> open_set;
    std::unordered_map<Position, PathNode*, PositionHash, PositionEqual> all_nodes;
    std::unordered_map<Position, float, PositionHash, PositionEqual> distances;

    // Initialize start node
    PathNode* start_node = new PathNode(start, 0, 0); // No heuristic for Dijkstra
    start_node->f_cost = 0; // f_cost = g_cost for Dijkstra
    open_set.push(start_node);
    all_nodes[start] = start_node;
    distances[start] = 0;

    std::cout << "\n=== DIJKSTRA PATHFINDING ===" << std::endl;
    std::cout << "Start: (" << start.x << ", " << start.y << ")" << std::endl;
    std::cout << "Goal: (" << goal.x << ", " << goal.y << ")" << std::endl;
    std::cout << "Searching..." << std::endl;

    while (!open_set.empty()) {
        PathNode* current = open_set.top();
        open_set.pop();
        result.nodes_explored++;

        if (PositionsEqual(current->position, goal)) {
            std::cout << "Path found with Dijkstra!" << std::endl;
            result.path = ReconstructPath(current);
            result.total_cost = current->g_cost;
            result.path_found = true;

            // Cleanup
            for (auto& pair : all_nodes) {
                delete pair.second;
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            std::cout << "Dijkstra completed in " << duration.count() << " microseconds" << std::endl;

            return result;
        }

        std::vector<Position> neighbors = GetNeighbors(current->position, game_map);

        for (const Position& neighbor_pos : neighbors) {
            if (!IsValidMove(neighbor_pos, game_map)) {
                continue;
            }

            float new_distance = current->g_cost + CalculateDistance(current->position, neighbor_pos);

            auto dist_it = distances.find(neighbor_pos);
            if (dist_it != distances.end() && new_distance >= dist_it->second) {
                continue;
            }

            PathNode* neighbor_node;
            auto node_it = all_nodes.find(neighbor_pos);
            if (node_it == all_nodes.end()) {
                neighbor_node = new PathNode(neighbor_pos);
                all_nodes[neighbor_pos] = neighbor_node;
            } else {
                neighbor_node = node_it->second;
            }

            neighbor_node->g_cost = new_distance;
            neighbor_node->h_cost = 0; // Dijkstra doesn't use heuristic
            neighbor_node->f_cost = new_distance; // f_cost = g_cost for priority queue
            neighbor_node->parent = current;

            distances[neighbor_pos] = new_distance;
            open_set.push(neighbor_node);
        }
    }

    std::cout << "No path found with Dijkstra!" << std::endl;

    // Cleanup
    for (auto& pair : all_nodes) {
        delete pair.second;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Dijkstra completed in " << duration.count() << " microseconds (no path)" << std::endl;

    return result;
}

// ******************** UTILITY METHODS ********************

void Pathfinding::PrintPath(const PathResult& result) const {
    if (!result.path_found) {
        std::cout << "No path to print!" << std::endl;
        return;
    }

    std::cout << "\n=== PATH FOUND ===" << std::endl;
    std::cout << "Path length: " << result.path.size() << " steps" << std::endl;
    std::cout << "Total cost: " << result.total_cost << std::endl;
    std::cout << "Nodes explored: " << result.nodes_explored << std::endl;

    std::cout << "Path: ";
    for (size_t i = 0; i < result.path.size(); ++i) {
        std::cout << "(" << result.path[i].x << "," << result.path[i].y << ")";
        if (i < result.path.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
    std::cout << "==================" << std::endl;
}

void Pathfinding::PrintPathDetails(const PathResult& result) const {
    PrintPath(result);

    if (result.path_found && result.path.size() > 1) {
        std::cout << "\n=== DETAILED PATH ANALYSIS ===" << std::endl;
        for (size_t i = 0; i < result.path.size(); ++i) {
            std::cout << "Step " << i << ": (" << result.path[i].x << ", " << result.path[i].y << ")";
            if (i == 0) {
                std::cout << " [START]";
            } else if (i == result.path.size() - 1) {
                std::cout << " [GOAL]";
            }
            std::cout << std::endl;
        }
        std::cout << "===============================" << std::endl;
    }
}

// ******************** PRIVATE HELPER METHODS ********************

float Pathfinding::CalculateHeuristic(const Position& from, const Position& to) const {
    // Manhattan distance heuristic (good for grid-based maps)
    return static_cast<float>(std::abs(from.x - to.x) + std::abs(from.y - to.y));
}

float Pathfinding::CalculateDistance(const Position& from, const Position& to) const {
    // For grid movement, use Manhattan distance as actual cost
    return static_cast<float>(std::abs(from.x - to.x) + std::abs(from.y - to.y));
}

std::vector<Position> Pathfinding::ReconstructPath(PathNode* goal_node) const {
    std::vector<Position> path;
    PathNode* current = goal_node;

    while (current != nullptr) {
        path.push_back(current->position);
        current = current->parent;
    }

    // Reverse to get path from start to goal
    std::reverse(path.begin(), path.end());
    return path;
}

template<typename TileContainer>
std::vector<Position> Pathfinding::GetNeighbors(const Position& pos, const Map<TileContainer>& game_map) const {
    std::vector<Position> neighbors;

    // 4-directional movement (up, right, down, left)
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};

    for (int i = 0; i < 4; ++i) {
        Position neighbor(pos.x + dx[i], pos.y + dy[i]);
        if (game_map.IsValidPosition(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

template<typename TileContainer>
bool Pathfinding::IsValidMove(const Position& pos, const Map<TileContainer>& game_map) const {
    if (!game_map.IsValidPosition(pos)) {
        return false;
    }

    const Tile& tile = game_map.GetTile(pos);
    return tile.IsTraversable();
}

bool Pathfinding::PositionsEqual(const Position& a, const Position& b) const {
    return a.x == b.x && a.y == b.y;
}

std::string Pathfinding::PositionToString(const Position& pos) const {
    return "(" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ")";
}

// ******************** DEMONSTRATION METHODS ********************

template<typename TileContainer>
void Pathfinding::DemoPathfinding(const Map<TileContainer>& game_map) const {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "           PATHFINDING DEMONSTRATION" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    Position start = game_map.GetStartPosition();
    Position goal = game_map.GetEndPosition();

    std::cout << "Map size: " << game_map.GetWidth() << "x" << game_map.GetHeight() << std::endl;
    std::cout << "Finding path from START to END..." << std::endl;

    // Create a non-const copy of this for the algorithm call
    Pathfinding* pathfinder = const_cast<Pathfinding*>(this);
    PathResult result = pathfinder->FindPathAStar(start, goal, game_map);
    pathfinder->PrintPathDetails(result);

    if (result.path_found) {
        std::cout << "\n🎉 SUCCESS: A* found the shortest path!" << std::endl;
        std::cout << "Algorithm efficiency: " << result.nodes_explored << " nodes explored" << std::endl;
        std::cout << "Path optimality: " << result.total_cost << " total movement cost" << std::endl;
    } else {
        std::cout << "\n❌ FAILURE: No path exists between start and end!" << std::endl;
    }

    std::cout << std::string(50, '=') << std::endl;
}

template<typename TileContainer>
void Pathfinding::CompareAlgorithms(const Map<TileContainer>& game_map) const {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           ALGORITHM COMPARISON: A* vs DIJKSTRA" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    Position start = game_map.GetStartPosition();
    Position goal = game_map.GetEndPosition();

    // Create a non-const copy for algorithm calls
    Pathfinding* pathfinder = const_cast<Pathfinding*>(this);

    // Test A* algorithm
    std::cout << "\n>>> TESTING A* ALGORITHM <<<" << std::endl;
    PathResult astar_result = pathfinder->FindPathAStar(start, goal, game_map);

    // Test Dijkstra algorithm
    std::cout << "\n>>> TESTING DIJKSTRA ALGORITHM <<<" << std::endl;
    PathResult dijkstra_result = pathfinder->FindPathDijkstra(start, goal, game_map);

    // Compare results
    std::cout << "\n" << std::string(40, '-') << std::endl;
    std::cout << "           COMPARISON RESULTS" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    std::cout << "Algorithm    | Nodes Explored | Path Cost | Path Found" << std::endl;
    std::cout << "-------------|----------------|-----------|------------" << std::endl;
    std::cout << "A*           | " << std::setw(14) << astar_result.nodes_explored
              << " | " << std::setw(9) << astar_result.total_cost
              << " | " << (astar_result.path_found ? "Yes" : "No") << std::endl;
    std::cout << "Dijkstra     | " << std::setw(14) << dijkstra_result.nodes_explored
              << " | " << std::setw(9) << dijkstra_result.total_cost
              << " | " << (dijkstra_result.path_found ? "Yes" : "No") << std::endl;

    if (astar_result.path_found && dijkstra_result.path_found) {
        std::cout << "\n🏆 WINNER: ";
        if (astar_result.nodes_explored < dijkstra_result.nodes_explored) {
            std::cout << "A* (more efficient - explored "
                      << (dijkstra_result.nodes_explored - astar_result.nodes_explored)
                      << " fewer nodes)" << std::endl;
        } else if (astar_result.nodes_explored > dijkstra_result.nodes_explored) {
            std::cout << "Dijkstra (more efficient - explored "
                      << (astar_result.nodes_explored - dijkstra_result.nodes_explored)
                      << " fewer nodes)" << std::endl;
        } else {
            std::cout << "TIE (both algorithms explored the same number of nodes)" << std::endl;
        }
    }

    std::cout << std::string(60, '=') << std::endl;
}

// ******************** EXPLICIT TEMPLATE INSTANTIATIONS ********************
// This tells the compiler to generate the template methods for the specific Map type we use

template PathResult Pathfinding::FindPathAStar<std::vector<std::vector<Tile>>>(
        const Position& start, const Position& goal,
        const Map<std::vector<std::vector<Tile>>>& game_map);

template PathResult Pathfinding::FindPathDijkstra<std::vector<std::vector<Tile>>>(
        const Position& start, const Position& goal,
        const Map<std::vector<std::vector<Tile>>>& game_map);

template void Pathfinding::DemoPathfinding<std::vector<std::vector<Tile>>>(
        const Map<std::vector<std::vector<Tile>>>& game_map) const;

template void Pathfinding::CompareAlgorithms<std::vector<std::vector<Tile>>>(
        const Map<std::vector<std::vector<Tile>>>& game_map) const;

template std::vector<Position> Pathfinding::GetNeighbors<std::vector<std::vector<Tile>>>(
        const Position& pos, const Map<std::vector<std::vector<Tile>>>& game_map) const;

template bool Pathfinding::IsValidMove<std::vector<std::vector<Tile>>>(
        const Position& pos, const Map<std::vector<std::vector<Tile>>>& game_map) const;