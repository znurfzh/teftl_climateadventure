// Morse Code Hangman Game

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
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",    //A-I
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",  //J-R
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."          //S-Z
};
char* numbers[] = {
    "-----", ".----", "..---", "...--", "....-", ".....","-....", "--...", "---..", "----."
};

/*const int maxWrongGuesses = 6;  // Max number of wrong guesses before game over*/
char targetWord[50];  // Placeholder for selected word to guess
char guessedWord[50]; // Placeholder for guessed letters
String morseInput = ""; // Store Morse code input

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
    Serial.println("Welcome to Morse Code Hangman!");
    Serial.println("Please enter a word:");
}

// Main loop to check for input and process guesses
void loop() {
    if (!gameStarted) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            input.toLowerCase();
            input.toCharArray(targetWord, 50);
            memset(guessedWord, '_', strlen(targetWord));
            guessedWord[strlen(targetWord)] = '\0';
            gameStarted = true;
            printGuessedWord();
            Serial.println("Guess the word by entering Morse code:");
            Serial.println("Short press for dot, long press for dash. Ensure a short pause between inputs.");
        }
    } else {
        int buttonState = digitalRead(buttonPin);

        if (buttonState == LOW) { // Button pressed for Morse input
            if (pressStartTime == 0) {
                pressStartTime = millis();
                // Turn on LEDs and play sound when button is pressed
                digitalWrite(led12, HIGH);
                digitalWrite(led6, HIGH);
                tone(audio8, 1600); // Updated frequency
                Serial.println("Button pressed...");
            }
            charEndDetected = false;
        } else if (buttonState == HIGH && pressStartTime != 0) { // Button released
            unsigned long pressDuration = millis() - pressStartTime;
            if (pressDuration < dashLen) {
                morseInput += '.'; // Dot
                Serial.println("Dot (.) detected");
            } else {
                morseInput += '-'; // Dash
                Serial.println("Dash (-) detected");
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
            } else if (pauseDuration >= charPause && pauseDuration < charPause * 2) {
                // Handle end of letter
                char decodedChar = decodeMorse(morseInput.c_str());
                if (decodedChar != '\0') {
                    Serial.print("Decoded: ");
                    Serial.println(decodedChar);
                    checkGuess(decodedChar);
                } else {
                    Serial.print("Unrecognized input: ");
                    Serial.println(morseInput);
                    Serial.println("Reminder: Short press for dot, long press for dash. Ensure short pause between inputs.");
                }
                morseInput = ""; // Reset Morse input for next character
                charEndDetected = false;
                printGuessedWord(); // Print the guessed word here
            } else if (pauseDuration >= charPause * 2) {
                // Handle long pause between dots/dashes
                Serial.println("Space between dots/dashes too long, please restart.");
                morseInput = ""; // Reset Morse input
                charEndDetected = false;
            }
            if (isGameOver()) {
                Serial.println("Game Over! Restarting the game...");
                gameStarted = false;
                memset(guessedWord, 0, sizeof(guessedWord)); // Clear guessed word
                memset(targetWord, 0, sizeof(targetWord)); // Clear target word
                Serial.println("Please enter a word:");
            }
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
        Serial.print("Correct! The letter ");
        Serial.print(ch);
        Serial.println(" is in the word.");
    } else {
        Serial.print("Wrong guess! The letter ");
        Serial.print(ch);
        Serial.println(" is not in the word.");
    }
}

// Function to print the current state of the guessed word
void printGuessedWord() {
    Serial.print("Word: ");
    for (int i = 0; i < strlen(guessedWord); i++) {
        Serial.print(guessedWord[i]);
        Serial.print(' ');
    }
    Serial.println();
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