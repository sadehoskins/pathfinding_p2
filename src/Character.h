#ifndef RAYLIBSTARTER_CHARACTER_H
#define RAYLIBSTARTER_CHARACTER_H

#include "Tile.h"
#include "raylib.h"

// ******************** CHARACTER TYPE ENUM ********************

enum class CharacterType {
    PLAYER,
    NPC_FRIENDLY,
    NPC_ENEMY,
    NPC_NEUTRAL
};

// ******************** CHARACTER BASE CLASS ********************

class Character {
public:
    // Constructor
    Character(const Position& start_position, CharacterType type = CharacterType::PLAYER);

    // Virtual destructor for polymorphism
    virtual ~Character() = default;

    // **INTERFACE FOR TRAVERSING THE MAP** (required by task)
    virtual bool CanMoveTo(const Position& new_position) const = 0;
    virtual void MoveTo(const Position& new_position) = 0;
    virtual Position GetPosition() const { return position_; }

    // **ENHANCED CHARACTER PROPERTIES** (for NPCs and advanced features)
    virtual void SetPosition(const Position& pos) { position_ = pos; }
    virtual void Render(int screen_x, int screen_y, int tile_size) const = 0;

    // **HEALTH SYSTEM** (mentioned in task requirements)
    virtual int GetHealth() const { return health_; }
    virtual int GetMaxHealth() const { return max_health_; }
    virtual void SetHealth(int health);
    virtual void TakeDamage(int damage);
    virtual void Heal(int amount);
    virtual bool IsAlive() const { return health_ > 0; }

    // **CHARACTER ATTRIBUTES** (for future NPC implementation)
    virtual int GetStrength() const { return base_strength_; }
    virtual void SetStrength(int strength) { base_strength_ = strength; }

    // **CHARACTER TYPE AND STATUS**
    CharacterType GetCharacterType() const { return character_type_; }
    virtual std::string GetName() const { return name_; }
    virtual void SetName(const std::string& name) { name_ = name; }

    // **STATUS METHODS** (useful for both PCs and NPCs)
    virtual void PrintStatus() const;
    virtual bool IsPlayerCharacter() const { return character_type_ == CharacterType::PLAYER; }
    virtual bool IsNPC() const { return character_type_ != CharacterType::PLAYER; }

    // **UPDATE METHOD** (for future AI/behavior systems)
    virtual void Update() {} // Default: do nothing, NPCs can override

protected:
    // Core attributes
    Position position_;
    CharacterType character_type_;
    std::string name_;

    // Health system
    int health_;
    int max_health_;

    // Basic stats (can be overridden by subclasses)
    int base_strength_;

    // **HELPER METHODS**
    void ClampHealth(); // Ensure health stays within valid range
};


#endif //RAYLIBSTARTER_CHARACTER_H
