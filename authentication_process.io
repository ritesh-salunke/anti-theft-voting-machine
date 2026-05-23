#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

#define RESULT_BTN 6
#define RESET_BTN 7

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int id = 1;
bool enrolling = false;

void setup() {

  Serial.begin(9600);
  mySerial.begin(57600);

  pinMode(RESET_BTN, INPUT_PULLUP);
  pinMode(RESULT_BTN, INPUT_PULLUP);

  if (finger.verifyPassword()) {

    Serial.println("Fingerprint Sensor Ready");

  } else {

    Serial.println("Sensor Not Found");

    while (1);
  }

  Serial.println("--------------------------------");
  Serial.println("Press RESET button");
  Serial.println("to start enrollment");
  Serial.println("--------------------------------");
}

void loop() {

  // ===================================
  // START ENROLLMENT
  // ===================================

  if (!enrolling) {

    if (digitalRead(RESET_BTN) == LOW) {

      delay(300);

      while (digitalRead(RESET_BTN) == LOW);

      Serial.println("\nClearing Database...");

      finger.emptyDatabase();

      Serial.println("Database Cleared");

      enrolling = true;

      id = 1;

      delay(1000);

      Serial.println("\n===== ENROLLMENT STARTED =====");
    }

    return;
  }

  // ===================================
  // STOP ENROLLMENT
  // ===================================

  if (digitalRead(RESULT_BTN) == LOW) {

    delay(300);

    while (digitalRead(RESULT_BTN) == LOW);

    Serial.println("\nEnrollment Finished");

    enrolling = false;

    return;
  }

  // ===================================
  // ENROLL FINGER
  // ===================================

  Serial.println("\n--------------------------------");

  Serial.print("Enrolling Finger For ID ");
  Serial.println(id);

  if (enrollFinger(id)) {

    Serial.println("Fingerprint Stored Successfully");

    Serial.print("Stored ID : ");
    Serial.println(id);

    id++;

  } else {

    Serial.println("Enrollment Failed! Retry...");
  }

  delay(2000);
}

// ===================================
// ENROLL FUNCTION
// ===================================

bool enrollFinger(int id) {

  int p = -1;
  int attempts = 0;

  Serial.println("Place Finger...");

  while (p != FINGERPRINT_OK && attempts < 50) {

    p = finger.getImage();

    attempts++;

    delay(100);
  }

  if (p != FINGERPRINT_OK) {

    Serial.println("Finger Not Detected!");

    return false;
  }

  if (finger.image2Tz(1) != FINGERPRINT_OK) {

    Serial.println("Image Conversion Failed!");

    return false;
  }

  Serial.println("Remove Finger");

  delay(2000);

  p = 0;

  while (p != FINGERPRINT_NOFINGER) {

    p = finger.getImage();
  }

  Serial.println("Place Same Finger Again...");

  p = -1;
  attempts = 0;

  while (p != FINGERPRINT_OK && attempts < 50) {

    p = finger.getImage();

    attempts++;

    delay(100);
  }

  if (p != FINGERPRINT_OK) {

    Serial.println("Second Scan Failed!");

    return false;
  }

  if (finger.image2Tz(2) != FINGERPRINT_OK) {

    Serial.println("Second Image Conversion Failed!");

    return false;
  }

  if (finger.createModel() != FINGERPRINT_OK) {

    Serial.println("Fingerprints Did Not Match!");

    return false;
  }

  if (finger.storeModel(id) != FINGERPRINT_OK) {

    Serial.println("Storage Failed!");

    return false;
  }

  return true;
}
