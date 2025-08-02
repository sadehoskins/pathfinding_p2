#include "Game.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <memory>

// ******************** STATIC CONSTANTS ********************

const char* Game::kProjectName = "2D Map Generator";

// ******************** CONSTRUCTOR & DESTRUCTOR ********************

Game::Game()
        : current_state_(GameState::MENU)
        , should_close_(false)
        , game_map_(nullptr)
        , render_scale_(1.0f)
        , tile_size_(30) {
    // Constructor initialization
}

Game::~Game() {
    // Destructor - cleanup handled in Shutdown()
}

// ******************** CORE GAME METHODS ********************

void Game::Initialize() {
    InitializeRaylib();
    InitializeGameSystems();

    std::cout << "Game initialized successfully!" << std::endl;
}

void Game::Run() {
    Initialize();

    // Main game loop
    while (!ShouldClose()) {
        Update();
        Render();
    }

    Shutdown();
}

void Game::Shutdown() {
    CleanupResources();
    CloseWindow();
    std::cout << "Game shutdown complete." << std::endl;
}

// ******************** GAME LOOP COMPONENTS ********************

void Game::Update() {
    HandleInput();
    UpdateGameLogic();
}

void Game::Render() {
    CalculateRenderScale();

    BeginDrawing();

    // Render to canvas for letterboxing
    BeginTextureMode(canvas_);
    RenderGame();
    EndTextureMode();

    // Render canvas to screen with letterboxing
    ClearBackground(BLACK);
    DrawTexturePro(canvas_.texture,
                   Rectangle{0, 0, (float)canvas_.texture.width, (float)-canvas_.texture.height},
                   render_rec_,
                   {}, 0, WHITE);

    RenderUI();

    EndDrawing();
}

void Game::HandleInput() {
    // Handle fullscreen toggle
    HandleFullscreenToggle();

    // Handle game state specific input
    switch (current_state_) {
        case GameState::MENU:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                SetGameState(GameState::PLAYING);
            }
            break;

        case GameState::PLAYING:
            if (IsKeyPressed(KEY_ESCAPE)) {
                SetGameState(GameState::PAUSED);
            }
            if (IsKeyPressed(KEY_R)) {
                // Regenerate map with clustering
                game_map_->GenerateTerrainWithClustering();
                std::cout << "Map regenerated with terrain clustering!" << std::endl;
            }
            if (IsKeyPressed(KEY_C)) {
                // Print map to console
                std::cout << "\n=== CURRENT MAP ===" << std::endl;
                game_map_->RenderConsole();
                game_map_->PrintMapInfo();
                std::cout << "===================" << std::endl;
            }
            if (IsKeyPressed(KEY_T)) {
                // Toggle texture info
                std::cout << "Textures loaded: " << (Tile::AreTexturesLoaded() ? "Yes" : "No") << std::endl;
            }
            break;

        case GameState::PAUSED:
            if (IsKeyPressed(KEY_ESCAPE)) {
                SetGameState(GameState::PLAYING);
            }
            if (IsKeyPressed(KEY_Q)) {
                SetGameState(GameState::EXIT);
            }
            break;

        case GameState::EXIT:
            should_close_ = true;
            break;
    }
}

// ******************** GAME STATE MANAGEMENT ********************

GameState Game::GetCurrentState() const {
    return current_state_;
}

void Game::SetGameState(GameState new_state) {
    current_state_ = new_state;

    // Handle state transitions
    switch (new_state) {
        case GameState::PLAYING:
            std::cout << "Entering PLAYING state" << std::endl;
            break;
        case GameState::PAUSED:
            std::cout << "Game PAUSED" << std::endl;
            break;
        case GameState::EXIT:
            std::cout << "Exiting game..." << std::endl;
            should_close_ = true;
            break;
        default:
            break;
    }
}

bool Game::ShouldClose() const {
    return should_close_ || WindowShouldClose();
}

// ******************** PRIVATE METHODS ********************

void Game::InitializeRaylib() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(kScreenWidth, kScreenHeight, kProjectName);
    SetTargetFPS(60);

    // Initialize canvas for letterboxing
    canvas_ = LoadRenderTexture(kScreenWidth, kScreenHeight);
}

void Game::InitializeGameSystems() {
    // Load tile textures
    Tile::LoadAllTextures();

    // Initialize map with default size (15x15)
    game_map_ = std::make_unique<Map<>>(15, 15);

    // Print initial map info
    std::cout << "\n=== INITIAL MAP ===" << std::endl;
    game_map_->RenderConsole();
    game_map_->PrintMapInfo();
    std::cout << "===================" << std::endl;
}

void Game::UpdateGameLogic() {
    // Update game systems based on current state
    switch (current_state_) {
        case GameState::PLAYING:
            // Game logic updates would go here
            break;
        default:
            break;
    }
}

void Game::HandleFullscreenToggle() {
    if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)) {
        if (IsWindowFullscreen()) {
            ToggleFullscreen();
            SetWindowSize(kScreenWidth, kScreenHeight);
        } else {
            SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),
                          GetMonitorHeight(GetCurrentMonitor()));
            ToggleFullscreen();
        }
    }
}

void Game::CalculateRenderScale() {
    // Calculate render scale for letterboxing
    render_scale_ = std::min(GetScreenHeight() / (float)canvas_.texture.height,
                             GetScreenWidth() / (float)canvas_.texture.width);
    render_scale_ = floorf(render_scale_);
    if (render_scale_ < 1) render_scale_ = 1;

    // Calculate render rectangle
    render_rec_.width = canvas_.texture.width * render_scale_;
    render_rec_.height = canvas_.texture.height * render_scale_;
    render_rec_.x = (GetScreenWidth() - render_rec_.width) / 2.0f;
    render_rec_.y = (GetScreenHeight() - render_rec_.height) / 2.0f;
}

void Game::RenderGame() {
    ClearBackground(WHITE);

    switch (current_state_) {
        case GameState::MENU:
            DrawText("2D MAP GENERATOR", 200, 200, 40, DARKGRAY);
            DrawText("Press ENTER to start", 250, 300, 20, GRAY);
            DrawText("Press ALT+ENTER for fullscreen", 220, 350, 16, LIGHTGRAY);
            break;

        case GameState::PLAYING:
            if (game_map_) {
                // Calculate map position to center it
                int map_width = game_map_->GetWidth() * tile_size_;
                int map_height = game_map_->GetHeight() * tile_size_;
                int offset_x = (kScreenWidth - map_width) / 2;
                int offset_y = (kScreenHeight - map_height) / 2;

                // Render the map
                game_map_->Render(offset_x, offset_y, tile_size_);

                // Draw legend
                DrawText("Legend:", 10, 10, 20, BLACK);
                DrawText("s = Start", 10, 35, 16, GREEN);
                DrawText("e = End", 10, 55, 16, RED);
                DrawText("# = Stone, B = Bushes", 10, 75, 16, DARKGRAY);
                DrawText("T = Tree, ~ = Water", 10, 95, 16, DARKGRAY);
                DrawText(". = Dirt, o = Stone, , = Grass", 10, 115, 16, DARKGRAY);
            }
            break;

        case GameState::PAUSED:
            DrawText("PAUSED", 300, 250, 50, RED);
            DrawText("Press ESC to resume", 270, 350, 20, DARKGRAY);
            DrawText("Press Q to quit", 300, 380, 20, DARKGRAY);
            break;

        case GameState::EXIT:
            DrawText("Goodbye!", 320, 280, 40, DARKGRAY);
            break;
    }
}

void Game::RenderUI() {
    // Render UI elements that appear outside the canvas
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_S)) {
        DrawText(TextFormat("Render scale: %.0f", render_scale_), 10, 10, 20, LIGHTGRAY);
    }

    // Show controls in playing state
    if (current_state_ == GameState::PLAYING) {
        DrawText("Controls: R=Regenerate | C=Console | T=Texture Info | ESC=Pause", 10, GetScreenHeight() - 30, 16, DARKGRAY);
    }
}

void Game::CleanupResources() {
    // Unload tile textures
    Tile::UnloadAllTextures();

    // Unload render texture
    UnloadRenderTexture(canvas_);

    // Reset game map
    game_map_.reset();
}