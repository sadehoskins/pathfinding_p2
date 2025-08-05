cmake_minimum_required(VERSION 3.11)

include(../cmake/raylib.cmake)

project(game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cpp
        Tile.cpp
        Tile.h
        Map.h
        Game.cpp
        Game.h
        config.h
        items/base-classes/ItemBase.cpp
        items/base-classes/ItemBase.h
        items/ItemManager.cpp
        items/ItemManager.h
        items/base-classes/WeaponItem.cpp
        items/base-classes/WeaponItem.h
        items/base-classes/CurrencyItem.cpp
        items/base-classes/CurrencyItem.h
        items/base-classes/ConsumableItem.cpp
        items/base-classes/ConsumableItem.h
        items/base-classes/CollectibleItem.cpp
        items/base-classes/CollectibleItem.h
        items/weapons/WeaponSword.cpp
        items/weapons/WeaponSword.h
        items/weapons/WeaponStaff.cpp
        items/weapons/WeaponStaff.h
        items/currency/CurrencyKittyCoin.cpp
        items/currency/CurrencyKittyCoin.h
        items/consumables/ConsumablesHealthPotion.cpp
        items/consumables/ConsumablesHealthPotion.h
        items/consumables/ConsumablesBomb.cpp
        items/consumables/ConsumablesBomb.h
        items/collectibles/GemstoneBlue.cpp
        items/collectibles/GemstoneBlue.h
        items/base-classes/ArmorItem.cpp
        items/base-classes/ArmorItem.h
        items/base-classes/AccessoryItem.cpp
        items/base-classes/AccessoryItem.h
        inventory/Inventory.h
        inventory/EquipmentSlot.h
        items/accessories/AccessoryLuckyPaw.cpp
        items/accessories/AccessoryLuckyPaw.h
        items/accessories/AccessoryClawNecklace.cpp
        items/accessories/AccessoryClawNecklace.h
        items/armor/ArmorKittyBoots.cpp
        items/armor/ArmorKittyBoots.h
        items/armor/ArmorElderWings.cpp
        items/armor/ArmorElderWings.h
        inventory/InventorySystem.cpp
        inventory/InventorySystem.h
        Character.cpp
        Character.h
        PlayerChar.cpp
        PlayerChar.h
        TextureManager.cpp
        TextureManager.h
        Pathfinding.cpp
        Pathfinding.h)

# Add config.h.in file
configure_file(config.h.in config.h)
target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        )

#  Link to Raylib
target_link_libraries(${PROJECT_NAME} raylib)

# Copy assets to assets path
add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_LIST_DIR}/../cmake/copy-assets.cmake"
        )
add_dependencies(${PROJECT_NAME} copy_assets)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif ()

# CPack instructions
# call e.g. cpack -G ZIP -C Debug to create a binary distribution
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "${PROJECT_NAME}_Bundle")
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/assets" DESTINATION "${PROJECT_NAME}_Bundle")