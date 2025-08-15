#include "MainMenuState.h"

MainMenuState::MainMenuState(Display* disp, Input* inp) : GameState(disp, inp) {
    mainMenu = new MainMenu(display, input);
}

MainMenuState::~MainMenuState() {
    delete mainMenu;
}

void MainMenuState::enter() {
    Serial.println("Entering Main Menu State");
    mainMenu->activate();
    mainMenu->render();
}

void MainMenuState::update() {
    MenuResult result = mainMenu->handleInput();
    mainMenu->render();
    
    if (result == MenuResult::SELECTED) {
        MainMenuOption option = mainMenu->getSelectedOption();
        
        switch (option) {
            case MainMenuOption::START_GAME:
                requestStateChange(StateTransition::DOOR_CHOICE);
                break;
                
            case MainMenuOption::SETTINGS:
                // Could add reset option here later
                requestStateChange(StateTransition::SETTINGS);
                break;
                
            case MainMenuOption::CREDITS:
                requestStateChange(StateTransition::CREDITS);
                break;
        }
    }
}

void MainMenuState::exit() {
    Serial.println("Exiting Main Menu State");
    mainMenu->deactivate();
}
