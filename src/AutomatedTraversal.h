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

    // **MAIN TRAVERSAL METHODS**
    bool StartAutomatedTraversal(PlayerChar* player, Map<>* game_map, Pathfinding* pathfinder);
    void Update(); // Called each frame to progress movement
    void Stop(); // Stop current traversal

    // **STATE QUERIES**
    bool IsActive() const { return is_active_; }
    bool IsComplete() const { return is_complete_; }
    bool IsMoving() const { return is_moving_; }

    // **PATH VISUALIZATION**
    void RenderPathVisualization(int offset_x, int offset_y, int tile_size) const;
    bool IsPathVisualizationEnabled() const { return show_path_visualization_; }
    void TogglePathVisualization() { show_path_visualization_ = !show_path_visualization_; }

    // **PROGRESS INFO**
    int GetCurrentStep() const { return current_step_; }
    int GetTotalSteps() const { return static_cast<int>(calculated_path_.size()); }
    float GetProgress() const;
    std::string GetStatusMessage() const { return status_message_; }

    // **FINAL SUMMARY**
    void ShowFinalSummary() const;

private:
    // **CORE STATE**
    bool is_active_;
    bool is_complete_;
    bool is_moving_;
    bool show_path_visualization_;

    // **PATH DATA**
    std::vector<Position> calculated_path_;
    int current_step_;
    Position target_position_;

    // **GAME REFERENCES**
    PlayerChar* player_character_;
    Map<>* game_map_;
    Pathfinding* pathfinding_system_;

    // **MOVEMENT TIMING**
    float movement_timer_;
    float movement_delay_; // Time between steps (in seconds)

    // **STATUS TRACKING**
    std::string status_message_;
    int items_picked_up_;
    int items_equipped_;
    int total_items_found_;

    // **PRIVATE METHODS**
    void ProcessCurrentStep();
    void MoveToNextStep();
    void HandleItemPickup(const Position& pos);
    void HandleAutoEquipment();
    bool ShouldAutoEquipItem(const ItemBase* new_item, const ItemBase* current_item) const;
    void UpdateStatusMessage();
    void CompleteTraversal();

    // **VISUALIZATION HELPERS**
    Color GetPathColor(int step_index) const;
    void RenderPathStep(int x, int y, int tile_size, int step_index, bool is_current) const;
};



#endif //RAYLIBSTARTER_AUTOMATEDTRAVERSAL_H
