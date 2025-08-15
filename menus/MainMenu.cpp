#include "MainMenu.h"

MainMenu::MainMenu(Display* disp, Input* inp) : MenuBase(disp, inp, 3) {
    // 3 options: Start Game, Settings, Credits
    lastRenderedSelection = -1;
    needsRedraw = true;
}

void MainMenu::activate() {
    MenuBase::activate();
    lastRenderedSelection = -1;
    needsRedraw = true;
}

void MainMenu::render() {
    if (!isActive) return;
    
    // Only redraw if selection changed or forced redraw
    if (selectedOption != lastRenderedSelection || needsRedraw) {
        display->clear();
        drawTitle();
        drawMenuOptions();
        lastRenderedSelection = selectedOption;
        needsRedraw = false;
    }
}

void MainMenu::drawTitle() {
    // Game title - centered and large
    display->drawText("ESP32", 50, 40, TFT_CYAN, 2);
    display->drawText("DUNGEON", 30, 65, TFT_CYAN, 2);
    display->drawText("CRAWLER", 25, 90, TFT_CYAN, 2);
    
    // Version or subtitle
    display->drawText("v0.1.0", 60, 120, TFT_WHITE, 1);
}

void MainMenu::drawMenuOptions() {
    // Menu options centered in the middle area
    int yStart = 160;
    int ySpacing = 30;
    
    for (int i = 0; i < maxOptions; i++) {
        int yPos = yStart + (i * ySpacing);
        int xPos = 35; // Centered for longest option "Start Game"
        
        // Highlight selected option
        if (i == selectedOption) {
            // Draw selection background
            display->fillRect(xPos - 5, yPos - 3, 100, 20, TFT_BLUE);
            display->drawText(menuOptions[i], xPos, yPos, TFT_WHITE, 2);
        } else {
            // Draw unselected option
            display->drawText(menuOptions[i], xPos, yPos, TFT_WHITE, 2);
        }
    }
    
    // Instructions at bottom
    int instructY = 280;
    display->drawText("UP/DOWN: Navigate", 15, instructY, TFT_YELLOW, 1);
    instructY += 15;
    display->drawText("A: Select", 55, instructY, TFT_YELLOW, 1);
}

MenuResult MainMenu::handleInput() {
    if (!isActive) return MenuResult::NONE;
    
    // Handle navigation
    if (input->wasPressed(Button::UP)) {
        moveSelectionUp();
        return MenuResult::NONE;
    }
    
    if (input->wasPressed(Button::DOWN)) {
        moveSelectionDown();
        return MenuResult::NONE;
    }
    
    // Handle selection
    if (input->wasPressed(Button::A)) {
        selectionMade = selectedOption;
        return MenuResult::SELECTED;
    }
    
    // Main menu typically doesn't have a "cancel" option
    // But you could add one if needed
    
    return MenuResult::NONE;
}

MainMenuOption MainMenu::getSelectedOption() const {
    return static_cast<MainMenuOption>(selectionMade);
}
