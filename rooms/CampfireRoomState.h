#ifndef CAMPFIRE_ROOM_STATE_H
#define CAMPFIRE_ROOM_STATE_H

#include "RoomState.h"

enum class CampfireAction {
    REST = 0,
    INVENTORY = 1,
    LEAVE = 2
};

class CampfireRoomState : public RoomState {
private:
    // Campfire menu state
    int selectedOption;
    int maxOptions;
    bool screenDrawn;
    int lastSelectedOption;
    
    // Inventory management
    bool showingInventory;
    int inventorySelectedItem;
    int inventoryMaxItems;
    
    // Rest costs
    static const int REST_COST = 20;  // Gold cost to rest at campfire
    
    // Drawing methods
    void drawCampfireMenu();
    void drawInventoryScreen();
    void drawRestConfirmation();
    
    // Input handling
    void handleMenuInput();
    void handleInventoryInput();
    
    // Actions
    void performRest();
    void openInventory();
    void useSelectedItem();
    void equipSelectedItem();
    void returnToMenu();
    
public:
    CampfireRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm);
    ~CampfireRoomState() = default;
    
    // RoomState interface implementation
    void enterRoom() override;
    void handleRoomInteraction() override;
    void exitRoom() override;
};

#endif
