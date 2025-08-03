#include "CurrencyKittyCoin.h"

// ******************** CURRENCY KITTY COIN IMPLEMENTATION ********************

CurrencyKittyCoin::CurrencyKittyCoin(int amount)
        : CurrencyItem("Kitty Coin", "Adorable currency featuring cute cat faces.",
                       0.1f, 1, amount) {
}