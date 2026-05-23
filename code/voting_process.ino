#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

// ---------------- BUTTONS ----------------
#define A_BTN 4
#define B_BTN 5
#define RESULT_BTN 6
#define RESET_BTN 7
#define BUZZER 8

// ------------- FINGERPRINT ----------------
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// ---------------- VOTES -------------------
int A_votes = 0;
int B_votes = 0;

// for IDs 1 to 20
bool voted[21];

void setup() {

  Serial.begin(9600);
  mySerial.begin(57600);

  pinMode(A_BTN, INPUT_PULLUP);
  pinMode(B_BTN, INPUT_PULLUP);
  pinMode(RESULT_BTN, INPUT_PULLUP);
  pinMode(RESET_BTN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  Serial.println("=================================");
  Serial.println(" SMART VOTING SYSTEM STARTED ");
  Serial.println("=================================");

  // Fingerprint sensor check
  if (finger.verifyPassword()) {

    Serial.println("Fingerprint Sensor Ready");

  } else {

    Serial.println("Fingerprint Sensor Error");

    while (1);
  }

  Serial.println("\nWaiting For Voter...");
}

void loop() {

  // =========================================
  // RESULT BUTTON
  // =========================================

  if (digitalRead(RESULT_BTN) == LOW) {

    delay(50);

    if (digitalRead(RESULT_BTN) == LOW) {

      while (digitalRead(RESULT_BTN) == LOW);

      showResult();
    }
  }

  // =========================================
  // RESET BUTTON
  // =========================================

  if (digitalRead(RESET_BTN) == LOW) {

    delay(50);

    if (digitalRead(RESET_BTN) == LOW) {

      while (digitalRead(RESET_BTN) == LOW);

      resetVotes();
    }
  }

  // =========================================
  // GET FINGERPRINT ID
  // =========================================

  int id = getFingerprintID();

  if (id == -1) {
    return;
  }

  // =========================================
  // ALREADY VOTED
  // =========================================

  if (voted[id]) {

    Serial.println("\n---------------------------------");
    Serial.print("ID : ");
    Serial.println(id);

    Serial.println("STATUS : ALREADY VOTED");
    Serial.println("---------------------------------");

    buzz();

    delay(2000);

    return;
  }

  // =========================================
  // VALID VOTER
  // =========================================

  Serial.println("\n---------------------------------");

  Serial.print("VALID VOTER ID : ");
  Serial.println(id);

  Serial.println("\nPress Button");

  Serial.println("A Button = Candidate A");
  Serial.println("B Button = Candidate B");

  // =========================================
  // WAIT FOR VOTE
  // =========================================

  int vote = waitForVote();

  // =========================================
  // STORE VOTE
  // =========================================

  if (vote == 1) {

    A_votes++;

    Serial.println("\nVOTED FOR : Candidate A");

  } else if (vote == 2) {

    B_votes++;

    Serial.println("\nVOTED FOR : Candidate B");
  }

  voted[id] = true;

  // =========================================
  // SUCCESS MESSAGE
  // =========================================

  Serial.println("STATUS : VOTE SUCCESSFUL");

  Serial.println("---------------------------------");

  delay(2000);

  Serial.println("\nWaiting For Next Voter...");
}

// ==================================================
// CLEAN BUTTON HANDLING
// ==================================================

int waitForVote() {

  while (1) {

    // Candidate A
    if (digitalRead(A_BTN) == LOW) {

      delay(50);

      if (digitalRead(A_BTN) == LOW) {

        while (digitalRead(A_BTN) == LOW);

        return 1;
      }
    }

    // Candidate B
    if (digitalRead(B_BTN) == LOW) {

      delay(50);

      if (digitalRead(B_BTN) == LOW) {

        while (digitalRead(B_BTN) == LOW);

        return 2;
      }
    }
  }
}

// ==================================================
// FINGERPRINT MATCH
// ==================================================

int getFingerprintID() {

  int p = finger.getImage();

  if (p != FINGERPRINT_OK) {
    return -1;
  }

  p = finger.image2Tz();

  if (p != FINGERPRINT_OK) {
    return -1;
  }

  p = finger.fingerFastSearch();

  // fingerprint not found
  if (p != FINGERPRINT_OK) {

    Serial.println("\n---------------------------------");
    Serial.println("UNAUTHORIZED PERSON");
    Serial.println("FINGERPRINT NOT FOUND");
    Serial.println("---------------------------------");

    buzz();

    delay(2000);

    return -1;
  }

  return finger.fingerID;
}

// ==================================================
// RESULT DISPLAY
// ==================================================

void showResult() {

  Serial.println("\n=================================");
  Serial.println("         FINAL RESULT");
  Serial.println("=================================");

  Serial.print("Candidate A Votes : ");
  Serial.println(A_votes);

  Serial.print("Candidate B Votes : ");
  Serial.println(B_votes);

  Serial.println("---------------------------------");

  if (A_votes > B_votes) {

    Serial.print("WINNER : Candidate A");
    Serial.print("  BY ");
    Serial.print(A_votes - B_votes);
    Serial.println(" VOTES");

  } else if (B_votes > A_votes) {

    Serial.print("WINNER : Candidate B");
    Serial.print("  BY ");
    Serial.print(B_votes - A_votes);
    Serial.println(" VOTES");

  } else {

    Serial.println("RESULT : DRAW");
  }

  Serial.println("=================================\n");

  delay(3000);
}

// ==================================================
// RESET FUNCTION
// ==================================================

void resetVotes() {

  A_votes = 0;
  B_votes = 0;

  for (int i = 0; i < 21; i++) {

    voted[i] = false;
  }

  Serial.println("\n=================================");
  Serial.println(" ALL VOTES RESET SUCCESSFULLY ");
  Serial.println("=================================");

  delay(2000);
}

// ==================================================
// BUZZER
// ==================================================

void buzz() {

  digitalWrite(BUZZER, HIGH);

  delay(500);

  digitalWrite(BUZZER, LOW);
}
