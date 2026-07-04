#pragma once

#include "GameEngine.h"
#include "RecordManager.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <queue>
#include <optional>
#include <cstdlib>

class SfmlUI {
private:
    // Core Game System Architectures
    GameEngine* engine;
    RecordManager recordManager;

    // Window Setup Specifications
    sf::RenderWindow window;

    // Core Interface Configuration Constants
    const float WINDOW_WIDTH = 1000.0f; // 700px board canvas + 300px HUD sidebar
    const float WINDOW_HEIGHT = 700.0f;
    const float OFFSET_X = 0.0f;
    const float OFFSET_Y = 0.0f;
    const float TILE_SIZE = 70.0f;     // 70px * 10 tiles = 700px perfect canvas size
    const float PAWN_SPEED = 4.0f;     // Interpolation movement velocity scalar
    const float PAWN_RADIUS = 18.0f;    // UPDATED: Increased pawn radius for better visibility

    // Visual Asset Controls
    sf::Texture bgTexture;
    std::optional<sf::Sprite> bgSprite;
    sf::Font font;
    bool assetsLoadedSuccessfully;

    // Finite State Machine Enum
    enum class GameState {
        Idle,
        RollingDice,
        MovingPlayer,
        CheckingBoardEffects
    };
    GameState currentState = GameState::Idle;

    // Structural Component Animation State Trackers
    sf::Vector2f playerVisualPositions[6];
    std::queue<int> movementPath;
    int visualTargetTile = 0;
    float movementLerpTimer = 0.0f;

    // Graphical Dice Animation State Trackers
    sf::Clock animationClock;
    float diceRotationAngle = 0.0f;
    float diceScale = 1.0f;
    int currentDisplayRoll = 1;

    // HUD Text Stream Outputs
    std::string currentStatusMessage;

    // Color Profile Array Definitions
    const sf::Color playerColors[6] = {
        sf::Color::Red,
        sf::Color::Yellow,
        sf::Color::Cyan,
        sf::Color::Green,
        sf::Color::White,
        sf::Color(255, 105, 180) // Hot Pink secondary
    };

    // Structural Internal Interface Utility Helpers
    int findCurrentPlayerIndex();
    sf::Vector2f getTileCenterPosition(int tile);
    void updateAnimations(float deltaTime);

    // Low-Level Graphic Component Processing Layouts
    void drawBoard();
    void drawSidebar();
    void drawSnakesAndLadders();
    void drawPawnToken(sf::Vector2f position, sf::Color fillColor, const std::string& initialLetter);

public:
    // Lifecycle Declarations
    SfmlUI();
    ~SfmlUI();

    // Core Execution Loop Signature
    void runGame(const std::vector<std::string>& names, const std::vector<int>& colorIds);
};