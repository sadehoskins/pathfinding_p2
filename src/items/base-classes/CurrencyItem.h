#ifndef RAYLIBSTARTER_CURRENCYITEM_H
#define RAYLIBSTARTER_CURRENCYITEM_H

#include "ItemBase.h"

// ******************** CURRENCY ITEM CLASS ********************

class CurrencyItem : public ItemBase {
public:
    CurrencyItem(const std::string& name, const std::string& description,
                 float weight, int value, int stack_count = 1);

    // Override virtual methods
    bool IsStackable() const override { return true; }
    int GetStackLimit() const override { return 999; }
    std::string GetTypeDescription() const override { return "Currency"; }
    void Use() override; // Currency is spent, not consumed directly

    // Currency-specific methods
    int GetStackCount() const { return stack_count_; }
    void SetStackCount(int count) { stack_count_ = count; }
    void AddToStack(int amount) { stack_count_ += amount; }

private:
    int stack_count_;
};

#endif //RAYLIBSTARTER_CURRENCYITEM_H