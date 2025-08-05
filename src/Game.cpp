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
        , inventory_system_(nullptr)
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

    // **NEW** Handle inventory system input first (it manages its own state)
    if (inventory_system_) {
        inventory_system_->HandleInput();

        // Check if inventory state changed
        if (inventory_system_->IsInventoryOpen() && current_state_ != GameState::INVENTORY) {
            SetGameState(GameState::INVENTORY);
        } else if (!inventory_system_->IsInventoryOpen() && current_state_ == GameState::INVENTORY) {
            SetGameState(GameState::PLAYING);
        }
    }

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

            // **TASK 3B - SORTING CONTROLS** (moved outside inventory check)
            if (IsKeyPressed(KEY_ONE)) {
                if (inventory_system_) {
                    std::cout << "\n🔸 Sorting inventory by WEIGHT..." << std::endl;
                    inventory_system_->SortByWeight(true);
                }
            }
            if (IsKeyPressed(KEY_TWO)) {
                if (inventory_system_) {
                    std::cout << "\n🔸 Sorting inventory by NAME..." << std::endl;
                    inventory_system_->SortByName(true);
                }
            }
            if (IsKeyPressed(KEY_THREE)) {
                if (inventory_system_) {
                    std::cout << "\n🔸 Sorting inventory by VALUE..." << std::endl;
                    inventory_system_->SortByValue(false); // High to low is more interesting
                }
            }
            if (IsKeyPressed(KEY_FOUR)) {
                if (inventory_system_) {
                    std::cout << "\n🔸 Sorting inventory by TYPE..." << std::endl;
                    inventory_system_->SortByType(true);
                }
            }
            // **TASK 3B - DEMO CONTROLS**
            if (IsKeyPressed(KEY_SIX)) {
                if (inventory_system_) {
                    std::cout << "\n🎯 Generating test inventory for sorting demo..." << std::endl;
                    inventory_system_->GenerateTestInventory();
                }
            }
            if (IsKeyPressed(KEY_SEVEN)) {
                if (inventory_system_) {
                    std::cout << "\n🎯 Running complete sorting demonstration..." << std::endl;
                    inventory_system_->RunSortingDemo();
                }
            }
            // Pathfinding
            if (IsKeyPressed(KEY_EIGHT)) {
                if (pathfinding_system_ && game_map_) {
                    std::cout << "\n🔍 Running pathfinding demonstration..." << std::endl;
                    pathfinding_system_->DemoPathfinding(*game_map_);
                }
            }
            if (IsKeyPressed(KEY_NINE)) {
                if (pathfinding_system_ && game_map_) {
                    std::cout << "\n⚔️ Comparing A* vs Dijkstra algorithms..." << std::endl;
                    pathfinding_system_->CompareAlgorithms(*game_map_);
                }
            }
            if (IsKeyPressed(KEY_ZERO)) {
                if (pathfinding_system_ && game_map_ && player_character_) {
                    std::cout << "\n🎯 Finding path from player to end..." << std::endl;
                    Position player_pos = player_character_->GetPosition();
                    Position end_pos = game_map_->GetEndPosition();

                    PathResult result = pathfinding_system_->FindPathAStar(player_pos, end_pos, *game_map_);
                    pathfinding_system_->PrintPathDetails(result);

                    if (result.path_found) {
                        std::cout << "Player can reach the end in " << result.path.size() - 1 << " moves!" << std::endl;
                    } else {
                        std::cout << "Player cannot reach the end from current position." << std::endl;
                    }
                }
            }

            // **TASK 5A - AUTOMATED TRAVERSAL CONTROLS**
            if (IsKeyPressed(KEY_A) && !automated_traversal_->IsActive()) {
                if (automated_traversal_ && pathfinding_system_ && game_map_ && player_character_) {
                    std::cout << "\n🤖 Starting automated traversal to end position..." << std::endl;
                    bool success = automated_traversal_->StartAutomatedTraversal(
                            player_character_.get(), game_map_.get(), pathfinding_system_.get());

                    if (success) {
                        std::cout << "✅ Automated traversal started successfully!" << std::endl;
                        std::cout << "🎮 Sit back and watch the AI navigate!" << std::endl;
                    } else {
                        std::cout << "❌ Could not start automated traversal." << std::endl;
                    }
                }
            }
            if (IsKeyPressed(KEY_S) && automated_traversal_->IsActive()) {
                // Stop automated traversal
                std::cout << "\n🛑 Stopping automated traversal..." << std::endl;
                automated_traversal_->Stop();
            }
            if (IsKeyPressed(KEY_V)) {
                // Toggle path visualization
                if (automated_traversal_) {
                    automated_traversal_->TogglePathVisualization();
                    std::cout << "Path visualization: " <<
                              (automated_traversal_->IsPathVisualizationEnabled() ? "ON" : "OFF") << std::endl;
                }
            }
            if (IsKeyPressed(KEY_M) && automated_traversal_->IsComplete()) {
                // Show final summary again
                if (automated_traversal_) {
                    automated_traversal_->ShowFinalSummary();
                }
            }

            // Player movement
            if (player_character_) {
                bool moved = false;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                    moved = player_character_->TryMoveUp();
                }
                else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                    moved = player_character_->TryMoveDown();
                }
                else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                    moved = player_character_->TryMoveLeft();
                }
                else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                    moved = player_character_->TryMoveRight();
                }

                // **NEW** Pick up items at current position
                if (IsKeyPressed(KEY_F)) {
                    player_character_->PickUpItemAt(player_character_->GetPosition());
                }

                // Auto-pickup hidden items when stepping on sparkles
                if (moved) {
                    Position player_pos = player_character_->GetPosition();
                    if (game_map_->HasItemsAt(player_pos)) {
                        auto items = game_map_->GetItemManager().GetItemsAtPosition(player_pos);
                        for (const auto* item_with_pos : items) {
                            if (!item_with_pos->is_in_treasure_chest) {
                                std::cout << "Stepped on sparkle! Found: " << item_with_pos->item->GetName() << std::endl;
                                player_character_->PickUpItemAt(player_pos);
                                break; // Pick up one item at a time
                            }
                        }
                    }
                    player_character_->CheckItemsAtCurrentPosition();
                }

                // **NEW** Check current position for items
                if (IsKeyPressed(KEY_E)) {
                    std::cout << "\n=== CHECKING CURRENT POSITION ===" << std::endl;
                    player_character_->CheckItemsAtCurrentPosition();
                    std::cout << "=================================" << std::endl;
                }
            }

            if (IsKeyPressed(KEY_R)) {
                // Regenerate map with clustering
                game_map_->GenerateTerrainWithClustering();

                // **NEW** Respawn player at new start position
                if (player_character_) {
                    Position new_start = game_map_->GetStartPosition();
                    player_character_->SetPosition(new_start);
                    std::cout << "Player respawned at: (" << new_start.x << ", " << new_start.y << ")" << std::endl;
                }
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
                std::cout << "Textures loaded: " << (TextureManager::AreTexturesLoaded() ? "Yes" : "No") << std::endl;
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
            // Treasure chest interaction (spacebar to open nearby chests) uses inventory system
            if (IsKeyPressed(KEY_SPACE)) {
                HandleTreasureChestInteraction();
            }
            // Inventory status shortcut
            if (IsKeyPressed(KEY_P)) {
                if (inventory_system_) {
                    inventory_system_->PrintInventoryStatus();
                }
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
        case GameState::INVENTORY:
            std::cout << "Entering INVENTORY state" << std::endl;
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
    // Load all textures through TextureManager
    TextureManager::LoadAllTextures();

    // Initialize map with default size (15x15)
    game_map_ = std::make_unique<Map<>>(15, 15);

    // **NEW** Create player character at start position
    Position start_pos = game_map_->GetStartPosition();
    player_character_ = std::make_unique<PlayerChar>(start_pos, 10); // 10 base strength
    player_character_->SetMap(game_map_.get()); // Give player reference to map

    // Initialize inventory system
    inventory_system_ = std::make_unique<InventorySystem>();

    // Initialize pathfinding system
    pathfinding_system_ = std::make_unique<Pathfinding>();

    // Initialize automated traversal
    automated_traversal_ = std::make_unique<AutomatedTraversal>();

    // Some test items for inventory demonstration
    inventory_system_->AddItemToInventory(std::make_unique<WeaponSword>());
    inventory_system_->AddItemToInventory(std::make_unique<ArmorKittyBoots>());
    inventory_system_->AddItemToInventory(std::make_unique<AccessoryLuckyPaw>());

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
            // **NEW** automated traversal system
            if (automated_traversal_) {
                automated_traversal_->Update();
            }
        case GameState::INVENTORY:
            // **NEW** Update inventory system
            if (inventory_system_) {
                inventory_system_->Update();
            }
            // Other game logic updates would go here
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

                // **NEW** Render player character
                if (player_character_) {
                    Position player_pos = player_character_->GetPosition();
                    int player_screen_x = offset_x + (player_pos.x * tile_size_);
                    int player_screen_y = offset_y + (player_pos.y * tile_size_);
                    player_character_->Render(player_screen_x, player_screen_y, tile_size_);
                }

                // Draw legend
                DrawText("Legend:", 10, 10, 20, BLACK);
                DrawText("s = Start", 10, 35, 16, GREEN);
                DrawText("e = End", 10, 55, 16, RED);
                DrawText("# = Stone, B = Bushes", 10, 75, 16, DARKGRAY);
                DrawText("T = Tree, ~ = Water", 10, 95, 16, DARKGRAY);
                DrawText(". = Dirt, o = Stone, , = Grass", 10, 115, 16, DARKGRAY);
                DrawText("t = Treasure Chest (Closed)", 10, 135, 16, GOLD);
                DrawText("O = Treasure Chest (Opened)", 10, 155, 16, ORANGE);

                // **NEW** Player info
                if (player_character_) {
                    Position player_pos = player_character_->GetPosition();
                    DrawText(TextFormat("Player: (%d,%d)", player_pos.x, player_pos.y),
                             10, 225, 16, BLUE);
                    DrawText(TextFormat("Strength: %d", player_character_->GetTotalStrength()),
                             10, 245, 16, GREEN);
                    DrawText(TextFormat("Weight: %.1f/%.1f kg",
                                        player_character_->GetCurrentWeight(),
                                        player_character_->GetMaxCarryWeight()),
                             10, 265, 16,
                             player_character_->IsOverweight() ? RED : WHITE);
                }

                // **NEW** Item system info
                DrawText(TextFormat("Items: %d", game_map_->GetItemManager().GetTotalItemCount()),
                         10, 185, 16, PURPLE);
                DrawText(TextFormat("Chests: %d", (int)ItemManager::GetTreasureChestPositions().size()),
                         10, 205, 16, GOLD);

                // **NEW** Inventory info
                if (inventory_system_) {
                    int strength_bonus = inventory_system_->GetTotalStrengthBonus();
                    if (strength_bonus > 0) {
                        DrawText(TextFormat("Equipment Strength: +%d", strength_bonus),
                                 10, 225, 16, GREEN);
                    }
                }
            }

            // **NEW** Render inventory system (minimal UI when closed)
            if (inventory_system_) {
                inventory_system_->Render(kScreenWidth, kScreenHeight);
            }

            // **NEW** Show automated traversal status
            if (automated_traversal_->IsActive()) {
                std::string status = automated_traversal_->GetStatusMessage();
                DrawText(status.c_str(), 10, 300, 18,
                         automated_traversal_->IsComplete() ? GREEN : YELLOW);

                if (automated_traversal_->IsMoving()) {
                    DrawText("🤖 AI is navigating...", 10, 325, 16, SKYBLUE);
                }
            }

            break;

        case GameState::INVENTORY:
            // Render game background (dimmed)
            if (game_map_) {
                int map_width = game_map_->GetWidth() * tile_size_;
                int map_height = game_map_->GetHeight() * tile_size_;
                int offset_x = (kScreenWidth - map_width) / 2;
                int offset_y = (kScreenHeight - map_height) / 2;

                game_map_->Render(offset_x, offset_y, tile_size_);

                // Dim the background
                DrawRectangle(0, 0, kScreenWidth, kScreenHeight, ColorAlpha(BLACK, 0.7f));
            }

            // **NEW** Render full inventory system
            if (inventory_system_) {
                inventory_system_->Render(kScreenWidth, kScreenHeight);
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

    // Show controls based on current state
    if (current_state_ == GameState::PLAYING) {
        if (automated_traversal_ && automated_traversal_->IsActive()) {
            DrawText("AUTOMATED MODE: S=Stop | V=ToggleViz | M=Summary | I=Inventory | 1-4=Sort",
                     10, GetScreenHeight() - 30, 8, YELLOW);
        } else {
            DrawText("Controls: WASD=Move | F=Pick | A=AutoTraversal | I=Inventory | 8=Pathfind | 1-4=Sort | 6=TestItems | 7=Demo",
                     10, GetScreenHeight() - 30, 7, DARKGRAY);
        }

    } else if (current_state_ == GameState::INVENTORY) {
        DrawText("INVENTORY MODE - See inventory window for controls",
                 10, GetScreenHeight() - 30, 14, GOLD);
    }
}


void Game::CleanupResources() {
    // Unload all textures through TextureManager
    TextureManager::UnloadAllTextures();

    // Unload render texture
    UnloadRenderTexture(canvas_);

    // Reset player character
    player_character_.reset();

    // Reset inventory system
    inventory_system_.reset();

    // Reset game map
    game_map_.reset();

    // Reset pathfinding
    pathfinding_system_.reset();

    // Reset automated traversal
    automated_traversal_.reset();
}

// ******************** NEW METHODS FOR TASK 1C ********************

void Game::DemoTreasureChestInteraction() {
    // This method is now replaced by HandleTreasureChestInteraction()
    // Keep for compatibility, but redirect to new method
    HandleTreasureChestInteraction();
}

void Game::HandleTreasureChestInteraction() {
    if (!game_map_ || !inventory_system_) return;

    // Find a random closed treasure chest for demonstration
    auto chest_positions = ItemManager::GetTreasureChestPositions();

    for (const Position& chest_pos : chest_positions) {
        if (game_map_->HasTreasureChestAt(chest_pos) &&
            game_map_->GetTile(chest_pos).IsClosedTreasureChest()) {

            std::cout << "\n=== OPENING TREASURE CHEST ===" << std::endl;
            std::cout << "Opening chest at position (" << chest_pos.x << ", " << chest_pos.y << ")" << std::endl;

            // **NEW** Use inventory system to handle the item
            bool success = inventory_system_->OpenTreasureChest(chest_pos, game_map_->GetItemManager());

            if (success) {
                // Open the chest visually
                game_map_->OpenTreasureChestAt(chest_pos);
                std::cout << "Treasure chest opened and item added to inventory!" << std::endl;
            } else {
                std::cout << "Could not add item to inventory (full or no item)" << std::endl;
            }

            std::cout << "===============================" << std::endl;
            return; // Only open one chest per demo
        }
    }

    std::cout << "No closed treasure chests found to open!" << std::endl;
}


// Optional/Delete later: Add this method for testing
void Game::DemoInventoryIntegration() {
    if (!inventory_system_) return;

    std::cout << "\n=== INVENTORY INTEGRATION DEMO ===" << std::endl;

    // Add some demo items
    inventory_system_->AddItemToInventory(std::make_unique<WeaponStaff>());
    inventory_system_->AddItemToInventory(std::make_unique<ArmorElderWings>());
    inventory_system_->AddItemToInventory(std::make_unique<AccessoryClawNecklace>());

    // Show current inventory status
    inventory_system_->PrintInventoryStatus();

    std::cout << "Total equipment strength bonus: +" << inventory_system_->GetTotalStrengthBonus() << std::endl;
    std::cout << "Press 'I' in-game to open inventory!" << std::endl;
    std::cout << "===================================" << std::endl;
}

