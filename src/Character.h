#ifndef RAYLIBSTARTER_CHARACTER_H
#define RAYLIBSTARTER_CHARACTER_H

#include "Tile.h"
#include "raylib.h"

// ******************** CHARACTER BASE CLASS ********************

class Character {
public:
    // Constructor
    Character(const Position& start_position);

    // Virtual destructor for polymorphism
    virtual ~Character() = default;

    // **INTERFACE FOR TRAVERSING THE MAP** (required by task)
    virtual bool CanMoveTo(const Position& new_position) const = 0;
    virtual void MoveTo(const Position& new_position) = 0;
    virtual Position GetPosition() const { return position_; }

    // Basic character properties
    virtual void SetPosition(const Position& pos) { position_ = pos; }
    virtual void Render(int screen_x, int screen_y, int tile_size) const = 0;

protected:
    Position position_;
};


#endif //RAYLIBSTARTER_CHARACTER_H
