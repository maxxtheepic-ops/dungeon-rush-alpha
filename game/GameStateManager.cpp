#include "GameStateManager.h"

GameStateManager::GameStateManager(Display* disp, Input* inp) {
    display = disp;
    input = inp;
    
    // Initialize shared entities
    player = new Player("Hero");
    currentEnemy = new Enemy();
    dungeonManager = new DungeonManager(player);
    
    // Initialize states
    mainMenuState = new MainMenuState(display, input);
    doorChoiceState = new DoorChoiceState(display, input, dungeonManager);
    combatRoomState = new CombatRoomState(display, input, player, currentEnemy, dungeonManager);
    campfireRoomState = new CampfireRoomState(display, input, player, currentEnemy, dungeonManager);
    
    // Start with main menu
    currentState = mainMenuState;
}

GameStateManager::~GameStateManager() {
    delete player;
    delete currentEnemy;
    delete dungeonManager;
    delete mainMenuState;
    delete doorChoiceState;
    delete combatRoomState;
    delete campfireRoomState;
}

void GameStateManager::initialize() {
    Serial.println("=== ESP32 Dungeon Crawler ===");
    Serial.println("Game State Manager Initialized");
    
    // Enter initial state
    currentState->enter();
}

void GameStateManager::update() {
    // Update current state
    currentState->update();
    
    // Check for state transitions
    StateTransition nextState = currentState->getNextState();
    if (nextState != StateTransition::NONE) {
        changeState(nextState);
        currentState->clearTransition();
    }
}

void GameStateManager::changeState(StateTransition newState) {
    // Exit current state
    currentState->exit();
    
    // Change to new state
    switch (newState) {
        case StateTransition::MAIN_MENU:
            currentState = mainMenuState;
            break;
            
        case StateTransition::DOOR_CHOICE:
            currentState = doorChoiceState;
            break;
            
        case StateTransition::COMBAT:
            currentState = combatRoomState;
            break;
            
        case StateTransition::CAMPFIRE:
            currentState = campfireRoomState;
            break;
            
        case StateTransition::GAME_OVER:
            handlePlaceholderState(newState);
            return; // Don't change state, just show screen
            
        case StateTransition::SETTINGS:
        case StateTransition::CREDITS:
            handlePlaceholderState(newState);
            return; // Don't change state, just show screen
            
        default:
            Serial.println("Unknown state transition!");
            currentState = mainMenuState;
            break;
    }
    
    // Enter new state
    currentState->enter();
}

void GameStateManager::handlePlaceholderState(StateTransition state) {
    // Handle simple placeholder screens that return to main menu
    display->clear();
    
    switch (state) {
        case StateTransition::GAME_OVER:
            {
                // Show game over, wait for input, then reset everything
                bool anyButtonPressed = input->wasPressed(Button::UP) || 
                                       input->wasPressed(Button::DOWN) ||
                                       input->wasPressed(Button::A) || 
                                       input->wasPressed(Button::B);
                
                if (anyButtonPressed) {
                    fullGameReset();  // Reset all progress on death
                    currentState = mainMenuState;
                    currentState->enter();
                }
            }
            break;
            
        case StateTransition::SETTINGS:
            display->drawText("Settings", 50, 80, TFT_WHITE, 2);
            display->drawText("Coming Soon!", 30, 120, TFT_YELLOW);
            display->drawText("Press A to return", 10, 160, TFT_CYAN);
            
            if (input->wasPressed(Button::A)) {
                currentState = mainMenuState;
                currentState->enter();
            }
            break;
            
        case StateTransition::CREDITS:
            display->drawText("Credits", 50, 80, TFT_WHITE, 2);
            display->drawText("Made with ESP32", 20, 120, TFT_GREEN);
            display->drawText("& TFT_eSPI", 35, 140, TFT_GREEN);
            display->drawText("Press A to return", 10, 180, TFT_CYAN);
            
            if (input->wasPressed(Button::A)) {
                currentState = mainMenuState;
                currentState->enter();
            }
            break;
            
        default:
            break;
    }
}

void GameStateManager::resetPlayer() {
    // Only reset health and potions, keep equipment/progress
    player->heal(player->getMaxHP());
    player->addHealthPotions(3);
    Serial.println("Player health restored");
}

void GameStateManager::resetDungeonProgress() {
    // Use the proper reset method instead of recreating
    dungeonManager->resetToFirstFloor();
    Serial.println("Dungeon progress reset - starting from Floor 1");
}

void GameStateManager::fullGameReset() {
    // Reset player to base stats (lose all equipment/progress)
    player->resetToBaseStats();
    player->heal(player->getMaxHP());
    player->addHealthPotions(3);
    
    // Reset dungeon progress
    resetDungeonProgress();
    
    Serial.println("Full game reset - fresh start!");
}
