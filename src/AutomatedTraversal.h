#ifndef RAYLIBSTARTER_AUTOMATEDTRAVERSAL_H
#define RAYLIBSTARTER_AUTOMATEDTRAVERSAL_H

#include "Pathfinding.h"
#include "PlayerChar.h"
#include "Map.h"
#include <vector>
#include <string>

// ******************** AUTOMATED TRAVERSAL SYSTEM ********************

class AutomatedTraversal {
public:
    // Constructor
    AutomatedTraversal();

    // Destructor
    ~AutomatedTraversal();

    // Main traversal methods
    bool StartAutomatedTraversal(PlayerChar* player, Map<>* game_map, Pathfinding* pathfinder);
    void Update(); // Called each frame to progress movement
    void Stop(); // Stop current traversal

    // State queries
    bool IsActive() const { return is_active_; }
    bool IsComplete() const { return is_complete_; }
    bool IsMoving() const { return is_moving_; }

    // Path visualization
    void RenderPathVisualization(int offset_x, int offset_y, int tile_size) const;
    bool IsPathVisualizationEnabled() const { return show_path_visualization_; }
    void TogglePathVisualization() { show_path_visualization_ = !show_path_visualization_; }

    // Progress info
    int GetCurrentStep() const { return current_step_; }
    int GetTotalSteps() const { return static_cast<int>(calculated_path_.size()); }
    float GetProgress() const;
    std::string GetStatusMessage() const { return status_message_; }

    // Final summary
    void ShowFinalSummary() const;

private:
    // Core state
    bool is_active_;
    bool is_complete_;
    bool is_moving_;
    bool show_path_visualization_;

    // Path data
    std::vector<Position> calculated_path_;
    int current_step_;
    Position target_position_;

    // Game references
    PlayerChar* player_character_;
    Map<>* game_map_;
    Pathfinding* pathfinding_system_;

    // Movement timing
    float movement_timer_;
    float movement_delay_; // Time between steps (in seconds)

    // Status tracking
    std::string status_message_;
    int items_picked_up_;
    int items_equipped_;
    int total_items_found_;

    // private methods
    void ProcessCurrentStep();
    void MoveToNextStep();
    void HandleItemPickup(const Position& pos);
    void HandleAutoEquipment();
    bool ShouldAutoEquipItem(const ItemBase* new_item, const ItemBase* current_item) const;
    void UpdateStatusMessage();
    void CompleteTraversal();

    // Visualization helpers
    Color GetPathColor(int step_index) const;
    void RenderPathStep(int x, int y, int tile_size, int step_index, bool is_current) const;
};



#endif //RAYLIBSTARTER_AUTOMATEDTRAVERSAL_H
