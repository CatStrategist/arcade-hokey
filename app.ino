enum GameState
{
    Idle,
    Preparing,
    Game,
    Ending,
};

const uint8_t ENGINE_PHOTOCELL_PIN = 13;
const uint8_t PLAYER_ONE_PHOTOCELL_PIN = 12;
const uint8_t PLAYER_TWO_PHOTOCELL_PIN = 11;
const uint8_t BTN_START_GAME_PIN = 10;
const uint8_t COIN_DROPPED_SIG_PIN = 9;
const uint8_t ENGINE_OUTPUT_PIN = 8;
const uint8_t HOCKEY_PLAYERS_OUTPUT_PIN = 7;

const unsigned long GAME_TIME_MS = 150000;

unsigned long gameEndTimeMs = 0;
unsigned short int credits = 0;

unsigned short int playerOnePoints = 0;
unsigned short int playerTwoPoints = 0;

uint8_t player_one_photocell_prev;
uint8_t player_two_photocell_prev;
uint8_t engine_photocell_prev;
uint8_t coin_drop_prev;

bool isEnginedTurnedOn = false;

GameState gameState = GameState::Idle;

void setup()
{
    pinMode(BTN_START_GAME_PIN, INPUT_PULLUP);
    pinMode(COIN_DROPPED_SIG_PIN, INPUT_PULLUP);
    pinMode(ENGINE_OUTPUT_PIN, OUTPUT);
    pinMode(HOCKEY_PLAYERS_OUTPUT_PIN, OUTPUT);
}

void loop()
{
    switch (gameState)
    {
    case Idle:
        idleLoop();
        break;
    case Preparing:
        preparingLoop();
        break;
    case Game:
        gameLoop();
        break;
    case Ending:
        endingLoop();
        break;
    }

    delay(50);
}

void idleLoop()
{
    uint8_t coin_drop_read = digitalRead(COIN_DROPPED_SIG_PIN);

    if (coin_drop_read == LOW && coin_drop_prev == HIGH)
    {
        credits++;
    }

    coin_drop_prev = coin_drop_read;

    if (digitalRead(BTN_START_GAME_PIN) == LOW && credits > 0)
    {
        gameState = GameState::Preparing;
    }
}

void preparingLoop()
{
    credits--;
    playerOnePoints = 0;
    playerTwoPoints = 0;
    gameEndTimeMs = millis() + GAME_TIME_MS;
    gameState = GameState::Game;
    isEnginedTurnedOn = false;
    digitalWrite(HOCKEY_PLAYERS_OUTPUT_PIN, HIGH);
}

void gameLoop()
{
    uint8_t player_one_photocell = digitalRead(PLAYER_ONE_PHOTOCELL_PIN);
    uint8_t player_two_photocell = digitalRead(PLAYER_TWO_PHOTOCELL_PIN);
    uint8_t engine_photocell = digitalRead(ENGINE_PHOTOCELL_PIN);

    if (!isEnginedTurnedOn)
    {
        digitalWrite(ENGINE_OUTPUT_PIN, HIGH);
        isEnginedTurnedOn = true;
    }

    // we call your update function when the button was low and is now high: a.k.a. falling edge
    if (player_one_photocell == LOW && player_one_photocell_prev == HIGH)
    {
        playerOnePoints++;
    }

    if (player_two_photocell == LOW && player_two_photocell_prev == HIGH)
    {
        playerTwoPoints++;
    }

    if (engine_photocell == HIGH && engine_photocell_prev == LOW)
    {
        digitalWrite(ENGINE_OUTPUT_PIN, LOW);
        isEnginedTurnedOn = false;
    }

    player_one_photocell_prev = player_one_photocell;
    player_two_photocell_prev = player_two_photocell;
    engine_photocell_prev = engine_photocell;

    if (millis() > gameEndTimeMs)
    {
        gameState = GameState::Ending;
    }
}

void endingLoop()
{
    digitalWrite(HOCKEY_PLAYERS_OUTPUT_PIN, LOW);
    uint8_t player_one_photocell_prev = LOW;
    uint8_t player_two_photocell_prev = LOW;
    uint8_t engine_photocell_prev = LOW;
    uint8_t coin_drop_prev = HIGH;

    delay(1000);

    gameState = GameState::Idle;
}