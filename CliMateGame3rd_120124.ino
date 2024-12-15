// Morse Code CliMate Game

// Define output pins for LEDs and buzzer
int led12 = 12;      
int led6 = 6;        
int audio8 = 8;     
int buttonPin = 4;  // Input pin for the button

// Timing constants for Morse code
int dotLen = 100;
int dashLen = dotLen * 3;
int elemPause = dotLen;
int charPause = dotLen * 7;
unsigned long pressStartTime = 0; // Start time of button press
unsigned long releaseTime = 0; // Time when button was released
bool charEndDetected = false; // Flag to detect end of Morse character
bool gameStarted = false;

// Morse code arrays for letters and numbers
char* letters[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};
char* numbers[] = {
    "-----", ".----", "..---", "...--", "....-", ".....","-....", "--...", "---..", "----."
};

char targetWord[11] = "wastewater";  // Target word for third challenge
char guessedWord[11]; // Placeholder for guessed letters
String morseInput = ""; // Store Morse code input
String previousGuesses = ""; // Store previous guesses

// Setup function to initialize pins and start serial communication
void setup() {
    pinMode(led12, OUTPUT);
    pinMode(led6, OUTPUT);
    pinMode(audio8, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.begin(9600);
    delay(2000); // Ensure the serial connection stabilizes
    while (Serial.available() > 0) {
        Serial.read(); // Clear out the buffer 
    }
    for (int i = 0; i < 10; i++) { // Extra clearing, just in case
        Serial.read();
    }
    Serial.flush();
    /*sendMainMessage("Welcome to CliMate, a climate justice adventure!");
    sendMainMessage("You are humanity’s last hope in fighting climate change.");
    sendMainMessage("From now on, you are CliMate Agents.");
    sendMainMessage("Get ready, we’re about to start our adventure!");*/
    sendMainMessage("Read the classified document and find the third keyword.");
    startGame();
}

void startGame() {
    memset(guessedWord, '_', strlen(targetWord));
    guessedWord[strlen(targetWord)] = '\0';
    gameStarted = true;
    sendMainMessage("");
    sendMainMessage("3rd Challenge - Pollution Source");
    sendMainMessage("It is the major pollutant affecting our water sources.");
    sendMainMessage("It primarily comes from domestic and industrial discharge.");
    sendMainMessage("It severely impacts water quality, especially during droughts.");
    sendMainMessage("Translate the keyword into Morse code and input it into MorScribe.");
    sendKeywordPlaceholder();
    sendStatusMessage("Guess the keyword by entering Morse code. Game started! Good luck!");
}

void loop() {
    int buttonState = digitalRead(buttonPin);

    if (buttonState == LOW) { // Button pressed
        if (pressStartTime == 0) {
            pressStartTime = millis();
            // Turn on LEDs and play sound when button is pressed
            digitalWrite(led12, HIGH);
            digitalWrite(led6, HIGH);
            tone(audio8, 1600); // Updated frequency
            sendDetectionFeedback("Button pressed...");
        }
        charEndDetected = false;
    } else if (buttonState == HIGH && pressStartTime != 0) { // Button released
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < dashLen) {
            morseInput += '.'; // Dot
            sendDetectionFeedback("Dot ( . ) detected.");
        } else {
            morseInput += '-'; // Dash
            sendDetectionFeedback("Dash ( - ) detected.");
        }
        pressStartTime = 0; // Reset the press time
        releaseTime = millis();
        // Turn off LEDs and stop sound when button is released
        digitalWrite(led12, LOW);
        digitalWrite(led6, LOW);
        noTone(audio8);
        charEndDetected = true;
    }

    // Confirm end of character input
    if (charEndDetected) {
        unsigned long pauseDuration = millis() - releaseTime;
        if (pauseDuration > elemPause && pauseDuration < charPause) {
            // Handle end of element within a character
            delay(elemPause); // Ensure pause between dots and dashes within a character
        } else if (pauseDuration >= charPause) {
            // Handle end of letter
            char decodedChar = decodeMorse(morseInput.c_str());
            if (decodedChar != '\0') {
                sendMorseMessage(morseInput);
                morseInput = ""; // Reset Morse input for next character
                sendStatusMessage("Decoded: " + String(decodedChar));
                checkGuess(decodedChar);
                sendKeywordPlaceholder();
            } else {
                sendStatusMessage("Unrecognized input: " + morseInput);
                morseInput = ""; // Reset Morse input for next character
                sendKeywordPlaceholder();
            }
            charEndDetected = false;
        } else if (pauseDuration >= charPause * 2) {
            // Handle long pause between dots/dashes
            sendStatusMessage("Space between dots/dashes too long, please restart.");
            morseInput = ""; // Reset Morse input
            charEndDetected = false;
        }
        if (isGameOver()) {
            sendMainMessage("Congratulations! You've guessed the keyword!");
            sendMainMessage("Treating wastewater can reclaim about 50% of the water for reuse in New York City!");
            sendMainMessage("Untreated wastewater can contaminate drinking water sources and harm aquatic life.");
            sendMainMessage("Our field agents are dispatched to ensure 14 Wastewater Resource Recovery Facilities");
            sendMainMessage("in New York City can process wastewater effectively to reduce harmful impact.");
            sendMainMessage("Ready for the next challenge?");
            gameStarted = false; // End the game
        }
    }
}

// Function to decode Morse code sequence to a character
char decodeMorse(const char* morse) {
    for (int i = 0; i < 26; i++) {
        if (strcmp(morse, letters[i]) == 0) {
            return 'a' + i;
        }
    }
    for (int i = 0; i < 10; i++) {
        if (strcmp(morse, numbers[i]) == 0) {
            return '0' + i;
        }
    }
    return '\0'; // Return null character if no match found
}

// Function to check if guess is correct and update guessed word
void checkGuess(char ch) {
  bool correct = false;
  for (int i = 0; i < strlen(targetWord); i++) {
    if (targetWord[i] == ch) {
      guessedWord[i] = ch;
      correct = true;
    }
  }
  if (correct) {
    sendStatusMessage("Correct! " + String(ch) + " is in the word.");
    Serial.println("correct");
  } else {
    sendStatusMessage("Wrong guess! " + String(ch) + " is not in the word.");
    if (previousGuesses.length() > 0) {
      previousGuesses += ", ";
    }
    previousGuesses += String(ch);
    sendPreviousGuesses();
    Serial.println("wrong");
  }
}

// Function to send main message
void sendMainMessage(String message) {
    Serial.println("main:" + message);
}

// Function to send keyword placeholder
void sendKeywordPlaceholder() {
    String placeholder = "";
    for (int i = 0; i < strlen(guessedWord); i++) {
        placeholder += guessedWord[i];
        if (i < strlen(guessedWord) - 1) {
            placeholder += " ";
        }
    }
    Serial.println("keyword:" + placeholder);
}

// Function to send previous guesses
void sendPreviousGuesses() {
    Serial.println("previous:" + previousGuesses);
}

// Function to send status message
void sendStatusMessage(String message) {
    Serial.println("status:" + message);
}

// Function to send morse message
void sendMorseMessage(String morse) {
    Serial.println("morse:" + morse);
}

// Function to send detection feedback
void sendDetectionFeedback(String feedback) {
    Serial.println("detection:" + feedback);
}

// Function to check if the game is over
bool isGameOver() {
    for (int i = 0; i < strlen(guessedWord); i++) {
        if (guessedWord[i] == '_') {
            return false;
        }
    }
    return true;
}
