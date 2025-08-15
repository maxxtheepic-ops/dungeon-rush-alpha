#include "Floor.h"
#include <Arduino.h>

Floor::Floor(int floorNum) {
    floorNumber = floorNum;
    currentRoom = nullptr;
    roomsCompleted = 0;
    rooms.clear();
}

void Floor::generateFloor() {
    Serial.println("Generating Floor " + String(floorNumber) + "...");
    
    // Clear existing rooms
    for (Room* room : rooms) {
        delete room;
    }
    rooms.clear();
    
    // Create boss room
    Room* bossRoom = new Room(999, ROOM_BOSS);
    bossRoom->setEnemyType(3); // Orc boss
    rooms.push_back(bossRoom);
    
    Serial.println("Floor " + String(floorNumber) + " initialized.");
}

RoomType Floor::selectRandomRoomType() {
    int roll = random(1, 101); // 1-100
    
    if (roll <= 60) {
        return ROOM_ENEMY;
    } else if (roll <= 85) {
        return ROOM_TREASURE;
    } else {
        return ROOM_SHOP;
    }
}

bool Floor::isFloorComplete() const {
    return roomsCompleted >= 3; // Reduced for testing
}

Room* Floor::getCurrentRoom() const {
    return currentRoom;
}

std::vector<DoorChoice> Floor::getAvailableChoices() {
    std::vector<DoorChoice> choices;
    
    Serial.println("Floor " + String(floorNumber) + ": Getting choices, rooms completed: " + String(roomsCompleted));
    
    if (isFloorComplete()) {
        Serial.println("Floor complete - offering boss room");
        // Only boss room available
        DoorChoice bossChoice;
        bossChoice.room = getBossRoom();
        bossChoice.icon = ICON_SKULL;
        bossChoice.description = "Final challenge awaits";
        choices.push_back(bossChoice);
    } else {
        Serial.println("Floor incomplete - generating 2 random rooms");
        // Generate two fresh rooms
        for (int i = 0; i < 2; i++) {
            Room* newRoom = new Room(roomsCompleted * 10 + i, selectRandomRoomType());
            
            // Setup room content
            switch(newRoom->getType()) {
                case ROOM_ENEMY:
                    {
                        int enemyType = random(1, 4);
                        newRoom->setEnemyType(enemyType);
                    }
                    break;
                    
                case ROOM_TREASURE:
                    {
                        int treasureType = random(1, 4);
                        int treasureValue = floorNumber + random(1, 4);
                        newRoom->setTreasure(treasureType, treasureValue);
                    }
                    break;
                    
                case ROOM_SHOP:
                    // Shop setup handled in room
                    break;
                    
                default:
                    break;
            }
            
            DoorChoice choice;
            choice.room = newRoom;
            choice.icon = newRoom->getDoorIcon();
            choice.description = newRoom->getDescription();
            choices.push_back(choice);
        }
    }
    
    return choices;
}

bool Floor::enterRoom(int choice) {
    std::vector<DoorChoice> choices = getAvailableChoices();
    
    if (choice < 0 || choice >= choices.size()) {
        return false;
    }
    
    currentRoom = choices[choice].room;
    return true;
}

bool Floor::isBossRoomReady() const {
    return isFloorComplete();
}

Room* Floor::getBossRoom() {
    if (rooms.size() > 0) {
        return rooms[0]; // Boss room
    }
    return nullptr;
}

int Floor::getRoomsCompleted() const {
    return roomsCompleted;
}

int Floor::getFloorNumber() const {
    return floorNumber;
}

void Floor::incrementRoomsCompleted() {
    roomsCompleted++;
}

Floor::~Floor() {
    for (Room* room : rooms) {
        delete room;
    }
    rooms.clear();
}
