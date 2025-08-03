
#include "CurrencyItem.h"
#include <iostream>

// ******************** CURRENCY ITEM IMPLEMENTATION ********************

CurrencyItem::CurrencyItem(const std::string& name, const std::string& description,
                           float weight, int value, int stack_count)
        : ItemBase(name, description, weight, value, ItemRarity::COMMON),
          stack_count_(stack_count) {
}

void CurrencyItem::Use() {
    std::cout << "Currency is spent at stores, not consumed directly!" << std::endl;
}