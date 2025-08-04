#include "Game.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <memory>

// ******************** STATIC CONSTANTS ********************

const char* Game::kProjectName = "2D Map Generator - Task 1c";

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
                std::cout << "Map regenerated with terrain clustering and items!" << std::endl;
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
            if (IsKeyPressed(KEY_I)) {
                // Show item information - DEBUG VERSION
                std::cout << "\n=== DEBUG ITEM INFO ===" << std::endl;
                std::cout << "Total items in manager: " << game_map_->GetItemManager().GetTotalItemCount() << std::endl;
                std::cout << "Treasure chest positions: " << ItemManager::GetTreasureChestPositions().size() << std::endl;

                if (game_map_->GetItemManager().GetTotalItemCount() > 0) {
                    game_map_->GetItemManager().PrintItemsInfo();
                } else {
                    std::cout << "NO ITEMS GENERATED! Item generation failed." << std::endl;
                }
                std::cout << "=======================" << std::endl;
            }
            // **NEW** Treasure chest interaction (spacebar to open nearby chests)
            if (IsKeyPressed(KEY_SPACE)) {
                DemoTreasureChestInteraction();
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
    game_map_->GetItemManager().PrintItemsInfo();
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
            DrawText("2D MAP GENERATOR - TASK 1C", 150, 180, 40, DARKGRAY);
            DrawText("Features: Treasure Chests & Items!", 200, 230, 20, GRAY);
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
                DrawText("t = Treasure Chest (Closed)", 10, 135, 16, GOLD);
                DrawText("O = Treasure Chest (Opened)", 10, 155, 16, ORANGE);

                // **NEW** Item system info
                DrawText(TextFormat("Items: %d", game_map_->GetItemManager().GetTotalItemCount()),
                         10, 185, 16, PURPLE);
                DrawText(TextFormat("Chests: %d", (int)ItemManager::GetTreasureChestPositions().size()),
                         10, 205, 16, GOLD);
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
        DrawText("Controls: R=Regenerate | C=Console | I=Items | SPACE=Demo Chest | T=Texture | ESC=Pause",
                 10, GetScreenHeight() - 30, 14, DARKGRAY);
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

// ******************** NEW METHODS FOR TASK 1C ********************

void Game::DemoTreasureChestInteraction() {
    if (!game_map_) return;

    // Find a random closed treasure chest for demonstration
    auto chest_positions = ItemManager::GetTreasureChestPositions();

    for (const Position& chest_pos : chest_positions) {
        if (game_map_->HasTreasureChestAt(chest_pos) &&
            game_map_->GetTile(chest_pos).IsClosedTreasureChest()) {

            std::cout << "\n=== OPENING TREASURE CHEST ===" << std::endl;
            std::cout << "Opening chest at position (" << chest_pos.x << ", " << chest_pos.y << ")" << std::endl;

            // Get the item from the chest
            auto chest_item = game_map_->GetItemManager().TakeItemAtPosition(chest_pos, true);
            if (chest_item) {
                std::cout << "Found item: " << chest_item->GetName() << std::endl;
                std::cout << "Description: " << chest_item->GetDescription() << std::endl;
                std::cout << "Value: " << chest_item->GetValue() << " kitty coins" << std::endl;
                std::cout << "Weight: " << chest_item->GetWeight() << " kg" << std::endl;
                std::cout << "Rarity: " << chest_item->GetRarityName() << std::endl;
                std::cout << "Type: " << chest_item->GetTypeDescription() << std::endl;

                // Demonstrate item usage
                std::cout << "\nTrying to use item..." << std::endl;
                chest_item->Use();
            }

            // Open the chest visually
            game_map_->OpenTreasureChestAt(chest_pos);
            std::cout << "Treasure chest opened!" << std::endl;
            std::cout << "=============================" << std::endl;

            return; // Only open one chest per demo
        }
    }

    std::cout << "No closed treasure chests found to demonstrate!" << std::endl;
}