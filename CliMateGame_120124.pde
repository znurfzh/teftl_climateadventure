import processing.serial.*;
Serial myPort;

String mainMessage = "";
String keywordPlaceholder = "";
String previousGuesses = "";
String statusMessage = "";
String morseMessage = "";
String detectionFeedback = "";

ArrayList<Raindrop> raindrops = new ArrayList<Raindrop>();  // List to hold raindrops

void setup() {
  size(1024, 800);  // Updated window size
  background(0);
  textAlign(LEFT, TOP);
  textSize(16);  // Updated font size
  PFont font = createFont("Monospaced", 16);  // Use a monospaced font
  textFont(font);
  fill(255);

  // Specify the serial port you are using at the same baud rate as Arduino:
  String portName = "/dev/cu.usbmodem11101"; // For macOS
  myPort = new Serial(this, portName, 9600);
  
  // Initialize with some raindrops
  for (int i = 0; i < 10; i++) {
    raindrops.add(new Raindrop(random(width), random(-height, 0)));
  }
}

void draw() {
  background(0); // Clear the background

  // Draw the rain
  for (Raindrop drop : raindrops) {
    drop.fall();
    drop.show();
  }

  // Print current number of raindrops
  //println("Current number of raindrops: " + raindrops.size());

  // Draw the main message box
  fill(255);
  text("MISSION DIRECTIVE:", 50, 50);
  text(mainMessage, 50, 85, 924, 350);  // Adjusted height for stacking lines

  // Draw the keyword box
  fill(255);
  text("KEYWORD:", 50, 350);
  text(keywordPlaceholder, 50, 385, 924, 50);

  // Draw the previous guesses box
  fill(255);
  text("WRONG INPUT:", 50, 435);
  text(previousGuesses, 50, 470, 924, 50);

  // Draw the status box
  fill(255);
  text("STATUS:", 50, 520);
  text(statusMessage, 50, 555, 924, 50);

  // Draw the morse box
  fill(255);
  text("CODE READ:", 50, 605);
  text(morseMessage, 50, 640, 924, 50);

  // Draw the detection feedback box
  fill(255);
  text("FEEDBACK:", 50, 690);
  text(detectionFeedback, 50, 725, 924, 50);

  // Read serial data from Arduino
  if (myPort.available() > 0) {
    String inString = myPort.readStringUntil('\n');
    if (inString != null) {
      inString = inString.trim(); //Trim any extra whitespace
      //println("Received: " + inString);  // Debugging statement
      if (inString.equals("correct")) {
        //println("Correct signal received, adding raindrop");
        addRaindrops(10);
      } else if (inString.equals("wrong")) {
        //println("Wrong signal received, removing raindrops"); // Debugging statement
        removeRaindrops(10);
      } else if (inString.startsWith("main:")) {
        updateMainMessage(inString.substring(5).trim());
      } else if (inString.startsWith("keyword:")) {
        updateKeywordPlaceholder(inString.substring(8).trim());
      } else if (inString.startsWith("previous:")) {
        updatePreviousGuesses(inString.substring(9).trim());
      } else if (inString.startsWith("status:")) {
        updateStatusMessage(inString.substring(7).trim());
      } else if (inString.startsWith("morse:")) {
        updateMorseMessage(inString.substring(6).trim());
      } else if (inString.startsWith("detection:")) {
        updateDetectionFeedback(inString.substring(10).trim());
      }
    }
  }
}

// Function to add a new raindrop
void addRaindrops(int count) {
  for (int i = 0; i < count; i++){
  Raindrop newDrop = new Raindrop(random(width), 0);
  raindrops.add(newDrop);
  }
  //println("Raindrop added, total raindrops: " + raindrops.size());  // Debugging statement
}

void removeRaindrops(int count) {
  for (int i = 0; i < count && !raindrops.isEmpty(); i++) {
    raindrops.remove(raindrops.size() - 1);
  }
  //println(count + " raindrops removed, total raindrops: " + raindrops.size()); // Debugging statement

}
// Raindrop class
class Raindrop {
  float x, y, speed;

  Raindrop(float x, float y) {
    this.x = x;
    this.y = y;
    this.speed = random(2, 5);  // Random speed for each raindrop
  }

  void fall() {
    y += speed;
    if (y > height) {
      y = 0;
      x = random(width);
    }
  }

  void show() {
    stroke(0, 0, 255);  // Blue color for raindrops
    line(x, y, x, y + 10);  // Draw raindrop as a line
  }
}

// Function to update the main message
void updateMainMessage(String message) {
  if (message.startsWith("Congratulations!")) {
    // Clear the main message and add the game over message
    mainMessage = message;
  } else {
    if (!mainMessage.isEmpty()) {
      mainMessage += "\n";  // Stack new lines
    }
    mainMessage += message;
  }
}

// Function to update the keyword placeholder
void updateKeywordPlaceholder(String placeholder) {
  keywordPlaceholder = placeholder;
}

// Function to update the previous guesses
void updatePreviousGuesses(String guesses) {
  previousGuesses = guesses;
}

// Function to update the status message
void updateStatusMessage(String status) {
  statusMessage = status;
}

// Function to update the morse message
void updateMorseMessage(String morse) {
  morseMessage = morse;
}

// Function to update the detection feedback
void updateDetectionFeedback(String feedback) {
  detectionFeedback = feedback;
}
