#include "DoorChoiceState.h"

DoorChoiceState::DoorChoiceState(Display* disp, Input* inp, DungeonManager* dm) : GameState(disp, inp) {
    dungeonManager = dm;
    selectedOption = 0;
    maxOptions = 3;  // Left door, right door, campfire
    screenDrawn = false;
    lastSelectedOption = -1;
}

void DoorChoiceState::enter() {
    Serial.println("Entering Door Choice State");
    selectedOption = 0;
    screenDrawn = false;
    lastSelectedOption = -1;
    
    generateDoorChoices();
    drawScreen();
}

void DoorChoiceState::update() {
    handleInput();
    
    // Redraw if selection changed
    if (lastSelectedOption != selectedOption || !screenDrawn) {
        drawScreen();
    }
}

void DoorChoiceState::exit() {
    Serial.println("Exiting Door Choice State");
}

void DoorChoiceState::generateDoorChoices() {
    availableChoices = dungeonManager->getAvailableRooms();
    
    if (availableChoices.size() >= 2) {
        // Two doors available
        leftDoorIcon = getDoorIconText(availableChoices[0].icon);
        leftDoorDesc = availableChoices[0].description;
        rightDoorIcon = getDoorIconText(availableChoices[1].icon);
        rightDoorDesc = availableChoices[1].description;
    } else if (availableChoices.size() == 1) {
        // Only boss room (floor complete)
        leftDoorIcon = getDoorIconText(availableChoices[0].icon);
        leftDoorDesc = availableChoices[0].description;
        rightDoorIcon = "[X]";
        rightDoorDesc = "No door here";
    }
}

void DoorChoiceState::drawScreen() {
    display->clear();
    
    // Header
    display->drawText("Choose Your Path", 30, 15, TFT_CYAN, 2);
    
    // Floor progress
    drawFloorProgress();
    
    // Layout: Two doors side by side, campfire below center
    int doorWidth = 60;
    int doorHeight = 80;  // Shorter to make room for campfire
    int doorSpacing = 20;
    int leftDoorX = (170/2) - doorWidth - (doorSpacing/2);
    int rightDoorX = (170/2) + (doorSpacing/2);
    int doorY = 60;
    
    // Campfire position (centered below doors)
    int campfireWidth = 80;
    int campfireHeight = 50;
    int campfireX = (170 - campfireWidth) / 2;
    int campfireY = doorY + doorHeight + 20;
    
    // Draw doors
    drawDoor(0, leftDoorX, doorY, doorWidth, doorHeight, selectedOption == 0);
    drawDoor(1, rightDoorX, doorY, doorWidth, doorHeight, selectedOption == 1);
    
    // Draw campfire
    drawCampfire(campfireX, campfireY, campfireWidth, campfireHeight, selectedOption == 2);
    
    // Controls (updated for navigation)
    int controlY = campfireY + campfireHeight + 15;
    display->drawText("UP/DOWN: Navigate", 10, controlY, TFT_WHITE);
    display->drawText("A: Select", 10, controlY + 15, TFT_WHITE);
    
    screenDrawn = true;
    lastSelectedOption = selectedOption;
}

void DoorChoiceState::drawFloorProgress() {
    Floor* currentFloor = dungeonManager->getCurrentFloor();
    if (currentFloor) {
        String progress = "Floor " + String(dungeonManager->getCurrentFloorNumber()) + 
                         " - Room " + String(currentFloor->getRoomsCompleted() + 1) + "/10";
        display->drawText(progress.c_str(), 10, 35, TFT_WHITE);
    }
}

void DoorChoiceState::drawDoor(int doorIndex, int x, int y, int width, int height, bool selected) {
    // Door border
    uint16_t borderColor = selected ? TFT_YELLOW : TFT_WHITE;
    uint16_t bgColor = selected ? TFT_BLUE : TFT_BLACK;
    
    // Draw door frame
    display->drawRect(x, y, width, height, borderColor);
    display->fillRect(x+1, y+1, width-2, height-2, bgColor);
    
    // Door label
    String doorLabel = (doorIndex == 0) ? "LEFT" : "RIGHT";
    display->drawText(doorLabel.c_str(), x + 10, y + 8, TFT_WHITE);
    
    // Icon
    String icon = (doorIndex == 0) ? leftDoorIcon : rightDoorIcon;
    display->drawText(icon.c_str(), x + 20, y + 25, TFT_CYAN, 2);
    
    // Description (simplified for narrow doors)
    String desc = (doorIndex == 0) ? leftDoorDesc : rightDoorDesc;
    
    // Break into multiple lines
    int startY = y + 50;
    int lineHeight = 12;
    int maxChars = 8; // Max chars per line for narrow door
    
    String currentLine = "";
    int currentY = startY;
    
    for (int i = 0; i < desc.length(); i++) {
        currentLine += desc.charAt(i);
        
        if (currentLine.length() >= maxChars || i == desc.length() - 1) {
            display->drawText(currentLine.c_str(), x + 2, currentY, TFT_WHITE, 1);
            currentY += lineHeight;
            currentLine = "";
            
            if (currentY > y + height - 15) break; // Don't overflow door
        }
    }
    
    // Selection indicator
    if (selected) {
        display->drawText(">", x - 8, y + height/2, TFT_YELLOW);
        display->drawText("<", x + width + 1, y + height/2, TFT_YELLOW);
    }
}

void DoorChoiceState::handleInput() {
    // Navigation with UP/DOWN
    if (input->wasPressed(Button::UP)) {
        selectedOption--;
        if (selectedOption < 0) {
            selectedOption = maxOptions - 1;  // Wrap to bottom (campfire)
        }
    }
    
    if (input->wasPressed(Button::DOWN)) {
        selectedOption++;
        if (selectedOption >= maxOptions) {
            selectedOption = 0;  // Wrap to top (left door)
        }
    }
    
    // Selection with A button
    if (input->wasPressed(Button::A)) {
        if (selectedOption == 0) {
            // Left door
            Room* selectedRoom = dungeonManager->selectRoom(0);
            if (selectedRoom) {
                Serial.println("Selected LEFT door - " + selectedRoom->getRoomName());
                
                switch (selectedRoom->getType()) {
                    case ROOM_ENEMY:
                    case ROOM_BOSS:
                        requestStateChange(StateTransition::COMBAT);
                        break;
                    default:
                        // For now, other rooms go back to main menu
                        requestStateChange(StateTransition::MAIN_MENU);
                        break;
                }
            }
        } else if (selectedOption == 1 && availableChoices.size() > 1) {
            // Right door (only if available)
            Room* selectedRoom = dungeonManager->selectRoom(1);
            if (selectedRoom) {
                Serial.println("Selected RIGHT door - " + selectedRoom->getRoomName());
                
                switch (selectedRoom->getType()) {
                    case ROOM_ENEMY:
                    case ROOM_BOSS:
                        requestStateChange(StateTransition::COMBAT);
                        break;
                    default:
                        // For now, other rooms go back to main menu
                        requestStateChange(StateTransition::MAIN_MENU);
                        break;
                }
            }
        } else if (selectedOption == 2) {
            // Campfire - go to campfire room state
            Serial.println("Selected CAMPFIRE - entering campfire room");
            requestStateChange(StateTransition::CAMPFIRE);
        }
    }
    
    // B button for quick exit (optional)
    if (input->wasPressed(Button::B)) {
        requestStateChange(StateTransition::MAIN_MENU);
    }
}

void DoorChoiceState::drawCampfire(int x, int y, int width, int height, bool selected) {
    // Campfire border
    uint16_t borderColor = selected ? TFT_YELLOW : TFT_WHITE;
    uint16_t bgColor = selected ? TFT_BLUE : TFT_BLACK;
    
    // Draw campfire frame
    display->drawRect(x, y, width, height, borderColor);
    display->fillRect(x+1, y+1, width-2, height-2, bgColor);
    
    // Campfire icon (centered)
    display->drawText("[*]", x + width/2 - 8, y + 8, TFT_RED, 2);
    
    // Label
    display->drawText("CAMPFIRE", x + 10, y + 25, TFT_WHITE);
    
    // Description
    display->drawText("Rest & Recover", x + 5, y + 35, TFT_CYAN, 1);
    
    // Selection indicator
    if (selected) {
        display->drawText(">", x - 8, y + height/2, TFT_YELLOW);
        display->drawText("<", x + width + 1, y + height/2, TFT_YELLOW);
    }
}

String DoorChoiceState::getDoorIconText(DoorIcon icon) {
    switch(icon) {
        case ICON_SWORD: return "[!]";
        case ICON_QUESTION: return "[?]";
        case ICON_SKULL: return "[X]";
        default: return "[*]";
    }
}
