#include <WiFi.h>
#include <HTTPClient.h>
#include <String.h>
#include <esp_camera.h>
#define LED_BUILTIN 4

// Replace with your network credentials
const char* ssid = "ANONYMOUS";
const char* password = "yamarita";

// Replace with your Flask server IP address or hostname
const char* serverUrl = "http://192.168.0.102:5000/get_last_response";

// Pin connected to the lock and buzzer
const int lockPin = 13; //Pin connected to solenoid lock
const int buzzerPin = 12; //Pin connected to buzzer
const int triggerPin = 14; // Pin to receive the signal from arduino uno

String cmp;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

// Configure the camera
  camera_config_t cameraConfig;
  cameraConfig.ledc_channel = LEDC_CHANNEL_0;
  cameraConfig.ledc_timer = LEDC_TIMER_0;
  cameraConfig.pin_d0 = 5;
  cameraConfig.pin_d1 = 18;
  cameraConfig.pin_d2 = 19;
  cameraConfig.pin_d3 = 21;
  cameraConfig.pin_d4 = 36;
  cameraConfig.pin_d5 = 39;
  cameraConfig.pin_d6 = 34;
  cameraConfig.pin_d7 = 35;
  cameraConfig.pin_xclk = 0;
  cameraConfig.pin_pclk = 22;
  cameraConfig.pin_vsync = 25;
  cameraConfig.pin_href = 23;
  cameraConfig.pin_sscb_sda = 26;
  cameraConfig.pin_sscb_scl = 27;
  cameraConfig.pin_pwdn = 32;
  cameraConfig.pin_reset = -1;
  cameraConfig.xclk_freq_hz = 20000000;
  cameraConfig.pixel_format = PIXFORMAT_JPEG;
  
  // Initialize the camera
  esp_err_t error = esp_camera_init(&cameraConfig);
  if (error != ESP_OK) {
    Serial.printf("Camera initialization failed with error 0x%x", error);
    return;
  }


  // Set lock, buzzer, and trigger pins as outputs
  pinMode(lockPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(triggerPin, INPUT_PULLUP);
}

void loop() {
digitalWrite(lockPin,0);
digitalWrite(buzzerPin,0); 
  // Check the state of the trigger pin

  if (digitalRead(triggerPin) == 1) {
    digitalWrite(LED_BUILTIN,1);

  // Take a photo
    camera_fb_t* fb = NULL;
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Failed to capture photo");
      return;
    }
    // Create a HTTPClient object
    HTTPClient http;
    
      // Send POST request with the photo to the server
    http.begin(serverUrl);
    http.addHeader("Content-Type", "image/jpeg");
    int httpResponseCode = http.POST((uint8_t*)fb->buf, fb->len);
    // Send GET request to the server
    http.begin(serverUrl);
    int httpResponseCode = http.GET();
    Serial.println(httpResponseCode);

    // Check the response from the server
    if (httpResponseCode == HTTP_CODE_OK) {
      // String response = http.getString();
      // Serial.println("Response: " + response);
      // cmp = String("{\"response\":\"Authorized: Faces match.\"}");
      // Serial.println(response);
      // Serial.println(cmp);
      // // Check if the response is authorized or unauthorized
      // if (cmp.equals(cmp)) {
      
        Serial.println("ACCESS IS GRANTED "); 
        digitalWrite(lockPin,1);   // Trigger the lock
        delay(3000);                   // Lock for 3 seconds
        digitalWrite(lockPin,0); // Release the lock
         
      // } 
    }else {
        Serial.println("ACCESS IS NOT-GRANTED "); 
        digitalWrite(lockPin,0);
        delay(3000); 
        digitalWrite(buzzerPin,1); // Trigger the buzzer
        delay(3000);                   // Buzz for 3 seconds
        digitalWrite(buzzerPin,0); // Stop buzzing
       
      } 
    // Free the memory used by the photo buffer
    esp_camera_fb_return(fb);
    // End the HTTP request
    http.end();

    // Delay before sending the next request
    delay(5000);
  } else {
    digitalWrite(LED_BUILTIN,0); // Code does nothing and switches off the LED when shouldRunCode is false
    delay(1000); //delay by 1 second
  }
}
