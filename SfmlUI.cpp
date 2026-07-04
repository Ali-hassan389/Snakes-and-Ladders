#include "SfmlUI.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace sf;

SfmlUI::SfmlUI() : engine(nullptr), recordManager("data/records.txt") {
    // 1. Load board background file safely from disk
    if (!bgTexture.loadFromFile("board_bg.png")) {
        cout<< "[CRITICAL ERROR]: Failed to load board asset (board_bg.png)!\n";
        assetsLoadedSuccessfully = false;
        return;
    }
    // .emplace() instantly constructs the sprite in-memory only after we are 100% sure the image loaded.
    bgSprite.emplace(bgTexture);

    // Position the background layout to line up with our board offsets
    bgSprite->setPosition({ OFFSET_X, OFFSET_Y });

    // Auto-scale the background image to fit whatever our TILE_SIZE requires
    Vector2u textureSize = bgTexture.getSize();
    float scaleX = (TILE_SIZE * 10.0f) / textureSize.x;
    float scaleY = (TILE_SIZE * 10.0f) / textureSize.y;
    bgSprite->setScale({ scaleX, scaleY });
    // Mark asset loading as fully operational
    assetsLoadedSuccessfully = true;

    // 2. Load font file gracefully. If it fails, we keep the game running without text crashes.
    if (!font.openFromFile("arial.ttf")) {
        cout << "[WARNING]: Could not open 'arial.ttf'! Game text won't draw, but the board will still work.\n";
    }

    // Place all possible 6 players at "Tile 0" (the holding zone off the actual board grid)
    for (int i = 0; i < 6; i++) {
        playerVisualPositions[i] = getTileCenterPosition(0);
    }
}

SfmlUI::~SfmlUI() {}

int SfmlUI::findCurrentPlayerIndex() {
    if (!engine) return 0;

    // Asks the engine for the current active player's name string
    string currentName = engine->getCurrentPlayer().getName();
    auto& players = engine->getPlayers();

    // Matches the string name against the player vector list to get their array index
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i].getName() == currentName) {
            return static_cast<int>(i);
        }
    }
    return 0;
}


Vector2f SfmlUI::getTileCenterPosition(int tile) {
    // If player hasn't stepped on the board yet (Tile 0), tuck them neatly into the bottom-left sidebar margin
    if (tile <= 0) {
        float x = OFFSET_X - (TILE_SIZE / 2.0f);
        float y = OFFSET_Y + (9 * TILE_SIZE) + (TILE_SIZE / 2.0f);
        return Vector2f(x, y);
    }

    // Hard ceiling cap so players can't overshoot visually past tile 100
    if (tile > 100) tile = 100;

    // Calculate 0-indexed row (0 to 9) from the bottom up
    int row = (tile - 1) / 10;
    int col = 0;

    // The classic Snakes & Ladders zig-zag track logic 
    if (row % 2 == 0) {
        // Even rows (0, 2, 4...) run left-to-right
        col = (tile - 1) % 10;
    }
    else {
        // Odd rows (1, 3, 5...) run right-to-left
        col = 9 - ((tile - 1) % 10);
    }
    // Compute pixel coordinates pointing straight to the geometric center of that target square
    float x = OFFSET_X + col * TILE_SIZE + TILE_SIZE / 2.0f;
    float y = OFFSET_Y + (9 - row) * TILE_SIZE + TILE_SIZE / 2.0f;
    return Vector2f(x, y);
}

void SfmlUI::updateAnimations(float deltaTime) {
    if (!engine) return;

    int activeIdx = findCurrentPlayerIndex();
    Player& current = engine->getCurrentPlayer();

    // STATE 1: Player triggered a dice roll. Let's make it spin like a slot machine!
    if (currentState == GameState::RollingDice) {
        // Spin the box quickly based on real time elapsed
        diceRotationAngle += 720.0f * deltaTime;

        // Use a sine wave curve over the rotation value to create a squishy bounce effect
        diceScale = 1.0f + 0.25f * sin(diceRotationAngle * 3.14159f / 180.0f);

        // Every 0.08 seconds, swap the displayed number to mock up shuffling faces
        if (animationClock.getElapsedTime().asSeconds() > 0.08f) {
            currentDisplayRoll = (rand() % 6) + 1;
            animationClock.restart();
        }

        // Keep rolling for exactly 1 full second total before halting
        static float diceTimer = 0.0f;
        diceTimer += deltaTime;
        if (diceTimer >= 1.0f) {
            diceTimer = 0.0f; // Reset tracking variable
            // Fetch the concrete, true roll from the game core rules backend
            int actualRoll = engine->rollDice();
            currentDisplayRoll = actualRoll;
            diceRotationAngle = 0.0f; // Reset visuals back to straight flat layout
            diceScale = 1.0f;

            int oldPos = current.getPosition();
            // Check if the roll safely keeps them on or below the winning 100 mark
            if (oldPos + actualRoll <= 100) {
                // Queue up each individual step tile by tile into our pathing queue for a smooth walk animation
                for (int i = oldPos + 1; i <= oldPos + actualRoll; i++) {
                    movementPath.push(i);
                }
                currentState = GameState::MovingPlayer;
                visualTargetTile = movementPath.front();
                movementLerpTimer = 0.0f;
                currentStatusMessage = current.getName() + " is moving...";
            }
            else {
                // Overshot! Apply the turn change rules and reset to normal idle state
                engine->applyMove(actualRoll);
                currentStatusMessage = current.getName() + " overshot 100! Turn skipped.";
                engine->nextTurn();
                currentState = GameState::Idle;
            }
        }
    }

    // STATE 2: Visual walk animation step-by-step across tiles
    else if (currentState == GameState::MovingPlayer) {
        Vector2f startPos = playerVisualPositions[activeIdx];
        Vector2f targetPos = getTileCenterPosition(visualTargetTile);

        // Linear Interpolation (LERP) formula: Smoothly slide visuals from current toward destination
        movementLerpTimer += PAWN_SPEED * deltaTime;
        if (movementLerpTimer >= 1.0f) movementLerpTimer = 1.0f;

        // Visual position = Start + DirectionVector * PercentageDone
        playerVisualPositions[activeIdx] = startPos + (targetPos - startPos) * movementLerpTimer;

        // Once the pawn finishes sliding onto the immediate target tile step
        if (movementLerpTimer >= 1.0f) {
            movementPath.pop(); // Remove the tile step we just completed

            if (!movementPath.empty()) {
                // Grab next tile step and reset the slide percentage timer
                visualTargetTile = movementPath.front();
                movementLerpTimer = 0.0f;
            }
            else {
                // No more movement paths queued. Finalize the turn positioning data on the core server
                engine->applyMove(currentDisplayRoll);

                // Check if the backend notes if they just landed on a hazard or boost object
                if (engine->usedSnake() || engine->usedLadder()) {
                    movementPath.push(engine->getLastFinalPos());
                    visualTargetTile = movementPath.front();
                    movementLerpTimer = 0.0f;

                    if (engine->usedLadder()) currentStatusMessage = "Ladder Climb! Up you go!";
                    if (engine->usedSnake()) currentStatusMessage = "Slipped down a Snake!";

                    currentState = GameState::CheckingBoardEffects;
                }
                else {
                    // Check for clean win conditions
                    if (engine->hasWon()) {
                        currentStatusMessage = current.getName() + " WINS THE MATCH!";
                        vector<string> pNames;
                        for (auto& p : engine->getPlayers()) pNames.push_back(p.getName());
                        recordManager.saveRecord(pNames, current.getName(), engine->getTurnCount(), "SFML");
                        currentState = GameState::Idle;
                    }
                    else {
                        // Regular end of move. Give next player control, factoring in bonus mechanics for throwing a 6
                        engine->nextTurn();
                        if (currentDisplayRoll == 6) {
                            currentStatusMessage = "Rolled a 6! 1 Extra Turn granted!";
                        }
                        else {
                            currentStatusMessage = "Press SPACE to Roll!";
                        }
                        currentState = GameState::Idle;
                    }
                }
            }
        }
    }

    // STATE 3: Handle the rapid glide down a snake or fast climb up a ladder rung
    else if (currentState == GameState::CheckingBoardEffects) {
        Vector2f startPos = playerVisualPositions[activeIdx];
        Vector2f targetPos = getTileCenterPosition(visualTargetTile);

        // Slides the token across the snake/ladder path at roughly half normal walk speed for tracking clarity
        movementLerpTimer += (PAWN_SPEED * 0.5f) * deltaTime;
        if (movementLerpTimer >= 1.0f) movementLerpTimer = 1.0f;

        playerVisualPositions[activeIdx] = startPos + (targetPos - startPos) * movementLerpTimer;

        if (movementLerpTimer >= 1.0f) {
            movementPath.pop();
            if (engine->hasWon()) {
                currentStatusMessage = current.getName() + " WINS THE MATCH!";
                vector<string> pNames;
                for (auto& p : engine->getPlayers()) pNames.push_back(p.getName());
                recordManager.saveRecord(pNames, current.getName(), engine->getTurnCount(), "SFML");
            }
            else {
                engine->nextTurn();
                if (currentDisplayRoll == 6) {
                    currentStatusMessage = "Rolled a 6! 1 Extra Turn active!";
                }
                else {
                    currentStatusMessage = "Press SPACE to Roll!";
                }
            }
            currentState = GameState::Idle;
        }
    }

    // GLOBAL STATE OVERRIDE: Block input actions and lock text displays if engine acknowledges a concluded game
    else if (currentState == GameState::Idle && engine->isGameOver()) {
        Player& winner = engine->getPlayers()[engine->getWinnerIndex()];
        currentStatusMessage = winner.getName() + " WINS! Game Over.";
    }
}

void SfmlUI::runGame(const vector<string>& names, const vector<int>& colorIds) {
    if (!assetsLoadedSuccessfully) return;

    engine = new GameEngine(names, colorIds);

 
    window.create(VideoMode({ static_cast<unsigned int>(WINDOW_WIDTH), static_cast<unsigned int>(WINDOW_HEIGHT) }), "Snakes and Ladders");
    window.setFramerateLimit(60); // Cap simulation velocity to 60hz to eliminate machine variation speed spikes

    for (size_t i = 0; i < engine->getPlayers().size(); i++) {
        playerVisualPositions[i] = getTileCenterPosition(engine->getPlayers()[i].getPosition());
    }

    Clock deltaClock;
    currentStatusMessage = "Press SPACE to Roll!";

    // The Game Loop
    while (window.isOpen()) {
        // Track the precise micro-fraction of a second it took the CPU to compute the previous loop iteration
        float deltaTime = deltaClock.restart().asSeconds();

        // Modern SFML 3 Event Handler: pollEvent returns a std::optional now.
        while (const optional event = window.pollEvent()) {

            // Checking object state variations using modern type-safe templates
            if (event->is<Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                // If user hits space, the game isn't finished, and we aren't currently animating a movement...
                if (keyPressed->code == Keyboard::Key::Space && !engine->isGameOver() && currentState == GameState::Idle) {
                    currentState = GameState::RollingDice;
                    animationClock.restart();
                }
            }
        }

        // Process animations using our computed frame time step slice
        updateAnimations(deltaTime);

        // Wipe, paint off-screen, and swap back-buffer to screen display
        window.clear(Color(40, 40, 40)); // Dark mode workspace slate tint
        drawBoard();
        drawSidebar();
        window.display();
    }

    // Cleanup memory securely if the user exits the window framework
    delete engine;
    engine = nullptr;
}

void SfmlUI::drawPawnToken(Vector2f position, Color fillColor, const string& initialLetter) {
    ConvexShape pawn;

    // Build custom geometry outline layout out of 8 localized coordinates
    pawn.setPointCount(8);
    pawn.setPoint(0, Vector2f(-10.0f, 18.75f)); // Bottom-Left base edge
    pawn.setPoint(1, Vector2f(10.0f, 18.75f));  // Bottom-Right base edge
    pawn.setPoint(2, Vector2f(7.5f, 13.75f));   // Lower flank contouring
    pawn.setPoint(3, Vector2f(3.75f, 5.0f));    // Mid body contouring
    pawn.setPoint(4, Vector2f(5.0f, -5.0f));    // Neck point right side
    pawn.setPoint(5, Vector2f(-5.0f, -5.0f));   // Neck point left side
    pawn.setPoint(6, Vector2f(-3.75f, 5.0f));   // Mid flank left contouring
    pawn.setPoint(7, Vector2f(-7.5f, 13.75f));  // Lower flank left contouring

    pawn.setFillColor(fillColor);
    pawn.setOutlineColor(Color::White);
    pawn.setOutlineThickness(1.5f);
    pawn.setPosition(position);

    // Draw circular pawn sphere for the head assembly
    CircleShape head(7.5f);
    head.setFillColor(fillColor);
    head.setOutlineColor(Color::White);
    head.setOutlineThickness(1.5f);
    head.setOrigin({ 7.5f, 7.5f }); // Anchor directly onto geographic core for easier offset mapping
    head.setPosition(position + Vector2f(0.f, -10.0f));

    window.draw(pawn);
    window.draw(head);

    // Draw single identification text initial over the core center base of the token element
    if (font.getInfo().family != "") {
        Text text(font, initialLetter, 12);
        text.setFillColor(Color::Black);
        text.setStyle(Text::Bold);

        Vector2f bounds = text.getLocalBounds().size;
        text.setOrigin({ bounds.x / 2.0f, bounds.y / 2.0f });
        text.setPosition(position + Vector2f(0.f, 6.0f));
        window.draw(text);
    }
}

void SfmlUI::drawBoard() {
    if (bgSprite.has_value()) {
        window.draw(*bgSprite);
    }

    // Overlay vector items
    drawSnakesAndLadders();

    // Iterate through active game player array to trace graphic token positioning metrics
    auto& players = engine->getPlayers();
    for (size_t i = 0; i < players.size(); i++) {
        Vector2f renderedPos = playerVisualPositions[i];

        // Anti-Stacking Math: If multiple players land on the exact same board tile, 
        // calculate micro offsets based on their index loop positions so they don't cover each other.
        float offsetX = ((int)i % 3 - 1) * 12.0f;
        float offsetY = ((int)i / 3 - 1) * 10.0f;

        string initialLetter = string(1, players[i].getName()[0]);
        drawPawnToken(Vector2f(renderedPos.x + offsetX, renderedPos.y + offsetY), playerColors[players[i].getColorId()], initialLetter);
    }
}

void SfmlUI::drawSidebar() {
    float startX = 700.0f; // Shift rendering path tracking rightward away from active board space

    if (!engine->isGameOver() && font.getInfo().family != "") {
        Player& current = engine->getCurrentPlayer();
        Text turnText(font, "Current Turn:", 18);
        turnText.setFillColor(Color::White);
        turnText.setPosition({ startX, 50.0f });
        window.draw(turnText);

        Text nameText(font, current.getName(), 26);
        nameText.setFillColor(playerColors[current.getColorId()]);
        nameText.setStyle(Text::Bold);
        nameText.setPosition({ startX, 80.0f });
        window.draw(nameText);
    }
    else if (engine->isGameOver() && font.getInfo().family != "") {
        Player& winner = engine->getPlayers()[engine->getWinnerIndex()];
        Text winTitle(font, "WINNER!", 22);
        winTitle.setFillColor(Color::Yellow);
        winTitle.setPosition({ startX, 50.0f });
        window.draw(winTitle);

        Text winName(font, winner.getName(), 28);
        winName.setFillColor(playerColors[winner.getColorId()]);
        winName.setStyle(Text::Bold);
        winName.setPosition({ startX, 80.0f });
        window.draw(winName);
    }

    // Render 3D style white block shape to visualize traditional physical cube elements
    RectangleShape diceBox(Vector2f(80.0f, 80.0f));
    diceBox.setOrigin({ 40.0f, 40.0f });
    diceBox.setPosition({ startX + 40.0f, 200.0f });
    diceBox.setFillColor(Color(245, 245, 245));
    diceBox.setOutlineColor(Color::White);
    diceBox.setOutlineThickness(2.0f);

    diceBox.setRotation(degrees(diceRotationAngle));
    diceBox.setScale({ diceScale, diceScale });
    window.draw(diceBox);

    // Layer active roll count indicators over top center axes of dice blocks
    if (font.getInfo().family != "") {
        Text diceText(font, to_string(currentDisplayRoll), 36);
        diceText.setFillColor(Color::Black);
        diceText.setStyle(Text::Bold);
        Vector2f dBounds = diceText.getLocalBounds().size;
        diceText.setOrigin({ dBounds.x / 2.0f, dBounds.y / 2.0f });
        diceText.setPosition({ startX + 40.0f, 195.0f });

        diceText.setRotation(degrees(diceRotationAngle));
        diceText.setScale({ diceScale, diceScale });
        window.draw(diceText);

        // Status informational alert strings line text output mapping
        Text statusText(font, currentStatusMessage, 15);
        statusText.setFillColor(Color(220, 220, 220));
        statusText.setPosition({ startX, 280.0f });
        window.draw(statusText);

        // Subordinate interface controls help description labels
        Text controlHint(font, engine->isGameOver() ? "Game Finished!\nClose window to exit." : "Controls:\n[SPACE] -> Roll Dice", 14);
        controlHint.setFillColor(Color(130, 130, 130));
        controlHint.setPosition({ startX, 550.0f });
        window.draw(controlHint);
    }
}
void SfmlUI::drawSnakesAndLadders() {
    // Spatial index array collections containing board connectivity parameters
    int ladderStarts[] = { 2, 8, 20, 32, 50, 71 };
    int ladderEnds[] = { 23, 34, 77, 68, 91, 92 };
    int snakeStarts[] = { 17, 54, 62, 64, 87, 93, 95, 98 };
    int snakeEnds[] = { 4, 19, 18, 60, 24, 73, 75, 79 };

    for (int i = 0; i < 6; i++) {
        Vector2f startPos = getTileCenterPosition(ladderStarts[i]);
        Vector2f endPos = getTileCenterPosition(ladderEnds[i]);
        Vector2f direction = endPos - startPos;

        // Pythagorean distance equation mapping vector path distances
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        // Tangent inversion to find precise angular degrees to rotate structural rails
        float angle = atan2(direction.y, direction.x) * 180.0f / 3.14159265f;

        const float RAIL_THICKNESS = 4.0f;
        const float LADDER_WIDTH = 14.0f;

        // Left Side Rail
        RectangleShape leftRail(Vector2f(length, RAIL_THICKNESS));
        leftRail.setFillColor(Color(30, 144, 255)); // Dodger Blue
        leftRail.setOutlineColor(Color::White);
        leftRail.setOutlineThickness(1.0f);
        leftRail.setOrigin({ 0.0f, LADDER_WIDTH / 2.0f });
        leftRail.setPosition(startPos);
        leftRail.setRotation(degrees(angle));
        window.draw(leftRail);

        // Right Side Rail
        RectangleShape rightRail(Vector2f(length, RAIL_THICKNESS));
        rightRail.setFillColor(Color(30, 144, 255));
        rightRail.setOutlineColor(Color::White);
        rightRail.setOutlineThickness(1.0f);
        rightRail.setOrigin({ 0.0f, -LADDER_WIDTH / 2.0f });
        rightRail.setPosition(startPos);
        rightRail.setRotation(degrees(angle));
        window.draw(rightRail);

        // Step generation loop. Evenly spaces horizontal steps across calculated lengths
        int numRungs = static_cast<int>(length / 16.0f);
        Vector2f unitDir = direction / length;
        for (int r = 1; r < numRungs; r++) {
            Vector2f rungPos = startPos + unitDir * (r * 16.0f);

            RectangleShape rung(Vector2f(RAIL_THICKNESS, LADDER_WIDTH));
            rung.setFillColor(Color(135, 206, 250)); // Light Sky Blue
            rung.setOrigin({ RAIL_THICKNESS / 2.0f, LADDER_WIDTH / 2.0f });
            rung.setPosition(rungPos);
            rung.setRotation(degrees(angle + 90.0f)); // Mounts steps perpendicular (+90 deg) to rails
            window.draw(rung);
        }
    }

    for (int i = 0; i < 8; i++) {
        Vector2f startPos = getTileCenterPosition(snakeStarts[i]); // Head tile
        Vector2f endPos = getTileCenterPosition(snakeEnds[i]);     // Tail tile
        Vector2f direction = endPos - startPos;
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);

        Vector2f unitDir = direction / length;
        Vector2f perpDir(-unitDir.y, unitDir.x); // Perpendicular axis mapping to support waving offsets

        int segments = static_cast<int>(length / 3.0f);

        // Plots hundreds of overlapping circular matrices following a Sine Wave to mimic slithering skin
        for (int j = 0; j <= segments; j++) {
            float factor = static_cast<float>(j) / segments;
            float currentDist = factor * length;

            // Sine Wave Slither dynamics equation: Oscillates circles back and forth along perpendicular lines. 
            // The tail multiplier safely dampens the size scale down near zero at structural end points.
            float wiggle = sin(factor * 3.14159f * 3.5f) * 9.0f * (1.0f - factor * 0.4f);
            Vector2f segmentPos = startPos + unitDir * currentDist + perpDir * wiggle;

            // Gradually tapers body thickness thin towards the rear end
            float bodyRadius = 9.0f * (1.0f - factor * 0.7f);

            CircleShape bodySegment(bodyRadius);

            // Striped Skin Effect: Every 5 segments, flip between primary crimson tints and darker deep red
            if ((j / 5) % 2 == 0) {
                bodySegment.setFillColor(Color(220, 20, 60)); // Crimson
            }
            else {
                bodySegment.setFillColor(Color(139, 0, 0));   // Dark Red
            }

            bodySegment.setOrigin({ bodyRadius, bodyRadius });
            bodySegment.setPosition(segmentPos);
            window.draw(bodySegment);
        }

        // Distinct custom vector properties for Head components
        float headRadius = 11.5f;
        CircleShape snakeHead(headRadius);
        snakeHead.setFillColor(Color(255, 40, 70));
        snakeHead.setOutlineColor(Color(50, 0, 0));
        snakeHead.setOutlineThickness(1.5f);
        snakeHead.setOrigin({ headRadius, headRadius });
        snakeHead.setPosition(startPos);
        window.draw(snakeHead);

        // Small yellow slitted snake eyes
        CircleShape eye(2.0f);
        eye.setFillColor(Color::Yellow);
        eye.setOrigin({ 1.0f, 1.0f });
        // Place Left Eye offset using calculated perpendicular vectors
        eye.setPosition(startPos + perpDir * 4.5f + unitDir * 2.0f);
        window.draw(eye);
        // Place Right Eye symmetrically opposite
        eye.setPosition(startPos - perpDir * 4.5f + unitDir * 2.0f);
        window.draw(eye);

        // Labels text drawing placement tracking (e.g., "S1", "S2" indicators)
        if (font.getInfo().family != "") {
            Text label(font, "S" + to_string(i + 1), 12);
            label.setFillColor(Color::White);
            label.setStyle(Text::Bold);
            Vector2f labelBounds = label.getLocalBounds().size;
            label.setOrigin({ labelBounds.x / 2.0f, labelBounds.y / 2.0f });
            label.setPosition(startPos + direction * 0.35f);
            window.draw(label);
        }
    }
}