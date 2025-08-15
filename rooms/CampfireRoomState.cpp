#include "CampfireRoomState.h"

CampfireRoomState::CampfireRoomState(Display* disp, Input* inp, Player* p, Enemy* e, DungeonManager* dm) 
    : RoomState(disp, inp, p, e, dm) {
    selectedOption = 0;
    maxOptions = 3;
    screenDrawn = false;
    lastSelectedOption = -1;
    showingInventory = false;
    inventorySelectedItem = 0;
    inventoryMaxItems = 0;
}

void CampfireRoomState::enterRoom() {
    Serial.println("You find a warm campfire crackling in the darkness...");
    selectedOption = 0;
    showingInventory = false;
    screenDrawn = false;
    drawCampfireMenu();
}

void CampfireRoomState::handleRoomInteraction() {
    if (showingInventory) {
        handleInventoryInput();
    } else {
        handleMenuInput();
    }
    
    // Redraw if needed
    if (!showingInventory && (lastSelectedOption != selectedOption || !screenDrawn)) {
        drawCampfireMenu();
    }
}

void CampfireRoomState::exitRoom() {
    Serial.println("You leave the warmth of the campfire behind...");
    showingInventory = false;
}

void CampfireRoomState::drawCampfireMenu() {
    display->clear();
    
    // Title and atmosphere
    display->drawText("Campfire", 50, 15, TFT_YELLOW, 2);
    display->drawText("A warm, safe place", 20, 35, TFT_ORANGE);
    
    // Player status
    display->drawText(("HP: " + String(player->getCurrentHP()) + "/" + String(player->getMaxHP())).c_str(), 
                     10, 55, TFT_WHITE);
    display->drawText(("Gold: " + String(player->getGold())).c_str(), 
                     10, 70, TFT_YELLOW);
    
    // Menu options
    int yStart = 100;
    int ySpacing = 25;
    const char* options[3] = {"Rest (20g)", "Inventory", "Leave"};
    
    for (int i = 0; i < maxOptions; i++) {
        int yPos = yStart + (i * ySpacing);
        
        // Highlight selected option
        if (i == selectedOption) {
            display->fillRect(5, yPos - 3, 160, 20, TFT_BLUE);
            display->drawText(">", 10, yPos, TFT_YELLOW);
            display->drawText(options[i], 25, yPos, TFT_WHITE);
        } else {
            display->drawText(options[i], 25, yPos, TFT_WHITE);
        }
    }
    
    // Special case: show if can't afford rest
    if (selectedOption == 0 && player->getGold() < REST_COST) {
        display->drawText("(Not enough gold!)", 10, 200, TFT_RED);
    }
    
    // Controls
    display->drawText("UP/DOWN: Navigate", 10, 220, TFT_CYAN, 1);
    display->drawText("A: Select", 10, 235, TFT_CYAN, 1);
    display->drawText("B: Leave", 10, 250, TFT_CYAN, 1);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void CampfireRoomState::drawInventoryScreen() {
    display->clear();
    
    // Temporary message until we add inventory to Player
    display->drawText("Inventory System", 20, 60, TFT_CYAN, 2);
    display->drawText("Coming Soon!", 35, 100, TFT_YELLOW);
    display->drawText("For now, you can:", 20, 130, TFT_WHITE);
    display->drawText("- Rest at campfire", 20, 150, TFT_WHITE);
    display->drawText("- Manage potions in", 20, 170, TFT_WHITE);
    display->drawText("  combat menu", 20, 185, TFT_WHITE);
    
    display->drawText("B: Return to campfire", 5, 220, TFT_CYAN);
}

void CampfireRoomState::handleMenuInput() {
    // Navigation
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) {
            selectedOption = maxOptions - 1;
        }
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= maxOptions) {
            selectedOption = 0;
        }
    }
    
    // Selection
    if (input->wasPressed(Button::A)) {
        CampfireAction action = static_cast<CampfireAction>(selectedOption);
        
        switch (action) {
            case CampfireAction::REST:
                performRest();
                break;
            case CampfireAction::INVENTORY:
                openInventory();
                break;
            case CampfireAction::LEAVE:
                completeRoom();
                break;
        }
    }
    
    // Quick exit
    if (input->wasPressed(Button::B)) {
        completeRoom();
    }
}

void CampfireRoomState::handleInventoryInput() {
    // Simple return to menu for now
    if (input->wasPressed(Button::B)) {
        returnToMenu();
    }
}

void CampfireRoomState::performRest() {
    if (player->getGold() < REST_COST) {
        display->clear();
        display->drawText("Not enough gold!", 20, 100, TFT_RED, 2);
        display->drawText("Rest costs 20 gold", 15, 130, TFT_WHITE);
        display->drawText("Press any button", 20, 160, TFT_CYAN);
        // Wait for input then return to menu
        return;
    }
    
    if (player->getCurrentHP() >= player->getMaxHP()) {
        display->clear();
        display->drawText("Already at full", 25, 100, TFT_YELLOW, 2);
        display->drawText("health!", 55, 125, TFT_YELLOW, 2);
        display->drawText("Press any button", 20, 160, TFT_CYAN);
        return;
    }
    
    // Perform rest
    player->spendGold(REST_COST);
    player->heal(player->getMaxHP()); // Full heal
    
    display->clear();
    display->drawText("You rest by the", 25, 80, TFT_WHITE);
    display->drawText("warm fire...", 35, 100, TFT_WHITE);
    display->drawText("HP Restored!", 30, 130, TFT_GREEN, 2);
    display->drawText("Press any button", 20, 160, TFT_CYAN);
    
    Serial.println("Rested at campfire - HP restored for " + String(REST_COST) + " gold");
}

void CampfireRoomState::openInventory() {
    showingInventory = true;
    inventorySelectedItem = 0;
    drawInventoryScreen();
}

void CampfireRoomState::useSelectedItem() {
    // Placeholder for when we add full inventory support
    Serial.println("Item use not yet implemented - need to add inventory to Player class");
}

void CampfireRoomState::returnToMenu() {
    showingInventory = false;
    screenDrawn = false;
    drawCampfireMenu();
}
