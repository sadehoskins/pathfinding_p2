
#include "Character.h"
#include <iostream>
#include <algorithm>

// ******************** CHARACTER BASE CLASS IMPLEMENTATION ********************

Character::Character(const Position& start_position, CharacterType type)
        : position_(start_position)
        , character_type_(type)
        , name_("Unnamed Character")
        , health_(100)
        , max_health_(100)
        , base_strength_(10) {

    std::cout << "Character created at position (" << start_position.x << ", "
              << start_position.y << ") with type " << static_cast<int>(type) << std::endl;
}

// ******************** HEALTH SYSTEM IMPLEMENTATION ********************

void Character::SetHealth(int health) {
    health_ = health;
    ClampHealth();

    if (health_ <= 0) {
        std::cout << name_ << " has been defeated!" << std::endl;
    }
}

void Character::TakeDamage(int damage) {
    if (damage < 0) {
        std::cout << "Warning: Negative damage value, treating as heal." << std::endl;
        Heal(-damage);
        return;
    }

    int old_health = health_;
    health_ -= damage;
    ClampHealth();

    int actual_damage = old_health - health_;
    std::cout << name_ << " takes " << actual_damage << " damage! ("
              << health_ << "/" << max_health_ << " HP remaining)" << std::endl;

    if (health_ <= 0) {
        std::cout << name_ << " has been defeated!" << std::endl;
    }
}

void Character::Heal(int amount) {
    if (amount < 0) {
        std::cout << "Warning: Negative heal value, treating as damage." << std::endl;
        TakeDamage(-amount);
        return;
    }

    int old_health = health_;
    health_ += amount;
    ClampHealth();

    int actual_heal = health_ - old_health;
    if (actual_heal > 0) {
        std::cout << name_ << " heals for " << actual_heal << " HP! ("
                  << health_ << "/" << max_health_ << " HP)" << std::endl;
    } else {
        std::cout << name_ << " is already at full health!" << std::endl;
    }
}

// ******************** STATUS AND UTILITY METHODS ********************

void Character::PrintStatus() const {
    std::cout << "\n=== " << name_ << " STATUS ===" << std::endl;
    std::cout << "Position: (" << position_.x << ", " << position_.y << ")" << std::endl;
    std::cout << "Type: ";

    switch (character_type_) {
        case CharacterType::PLAYER:
            std::cout << "Player Character";
            break;
        case CharacterType::NPC_FRIENDLY:
            std::cout << "Friendly NPC";
            break;
        case CharacterType::NPC_ENEMY:
            std::cout << "Enemy NPC";
            break;
        case CharacterType::NPC_NEUTRAL:
            std::cout << "Neutral NPC";
            break;
    }
    std::cout << std::endl;

    std::cout << "Health: " << health_ << "/" << max_health_ << " HP";
    if (!IsAlive()) {
        std::cout << " [DEFEATED]";
    }
    std::cout << std::endl;

    std::cout << "Base Strength: " << base_strength_ << std::endl;
    std::cout << "Status: " << (IsAlive() ? "Alive" : "Defeated") << std::endl;
    std::cout << "=========================" << std::endl;
}

// ******************** PRIVATE HELPER METHODS ********************

void Character::ClampHealth() {
    health_ = std::max(0, std::min(health_, max_health_));
}