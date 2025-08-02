//
// Created by sadeh on 8/2/2025.
//

#ifndef RAYLIBSTARTER_GAME_H
#define RAYLIBSTARTER_GAME_H

#include "raylib.h"
#include "Map.h"
#include "Tile.h"
#include <memory>

// ******************** GAME STATE ENUM ********************

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    EXIT
};

// ******************** GAME CLASS ********************

class Game {
public:
    // Static constants for screen configuration
    static const int kScreenWidth = 800;
    static const int kScreenHeight = 600;
    static const char* kProjectName;

    // Constructor & Destructor
    Game();
    ~Game();

    // Core game methods
    void Initialize();
    void Run();
    void Shutdown();

    // Game loop components
    void Update();
    void Render();
    void HandleInput();

    // Game state management
    GameState GetCurrentState() const;
    void SetGameState(GameState new_state);

    // Utility
    bool ShouldClose() const;

private:
    // Game state
    GameState current_state_;
    bool should_close_;

    // Map system
    std::unique_ptr<Map<>> game_map_;

    // Rendering properties
    RenderTexture2D canvas_;
    float render_scale_;
    Rectangle render_rec_;
    int tile_size_;

    // Private methods
    void InitializeRaylib();
    void InitializeGameSystems();
    void UpdateGameLogic();
    void HandleFullscreenToggle();
    void CalculateRenderScale();
    void RenderGame();
    void RenderUI();
    void CleanupResources();
};


#endif //RAYLIBSTARTER_GAME_H
