#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_camera.h>

// Replace with your network credentials
const char* ssid = "ANONYMOUS";
const char* password = "yamarita";

// Replace with your Flask server IP address or hostname
const char* serverUrl = "http://localhost:5000/recognize_faces";

// Pin connected to the lock and buzzer
const int lockPin = 13;
const int buzzerPin = 12;

// Camera settings
const int cameraCaptureTimeout = 5000;  // Timeout for capturing a photo in milliseconds

// Base64 encoding function
String base64Encode(const uint8_t* data, size_t length) {
  const char* base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String encodedData = "";
  size_t dataIndex = 0;
  uint8_t input[3];
  uint8_t output[4];
  while (length--) {
    input[dataIndex++] = *(data++);
    if (dataIndex == 3) {
      output[0] = (input[0] & 0xFC) >> 2;
      output[1] = ((input[0] & 0x03) << 4) | ((input[1] & 0xF0) >> 4);
      output[2] = ((input[1] & 0x0F) << 2) | ((input[2] & 0xC0) >> 6);
      output[3] = input[2] & 0x3F;
      for (size_t i = 0; i < 4; i++) {
        encodedData += base64Table[output[i]];
      }
      dataIndex = 0;
    }
  }
  if (dataIndex > 0) {
    for (size_t i = dataIndex; i < 3; i++) {
      input[i] = 0;
    }
    output[0] = (input[0] & 0xFC) >> 2;
    output[1] = ((input[0] & 0x03) << 4) | ((input[1] & 0xF0) >> 4);
    output[2] = ((input[1] & 0x0F) << 2) | ((input[2] & 0xC0) >> 6);
    for (size_t i = 0; i < dataIndex + 1; i++) {
      encodedData += base64Table[output[i]];
    }
    while (dataIndex++ < 3) {
      encodedData += '=';
    }
  }
  return encodedData;
}

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

  // Set lock and buzzer pins as outputs
  pinMode(lockPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Take a photo
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to capture photo");
    return;
  }

  // Create a HTTPClient object
  HTTPClient http;
  
  // Send POST request with the base64-encoded photo to the server
  String encodedPhoto = base64Encode(fb->buf, fb->len);
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST("image=" + encodedPhoto);
  
  // Check the response from the server
  if (httpResponseCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println("Response: " + response);
    
    // Check if the response is authorized or unauthorized
    if (response == "Authorized: Faces match.") {
      digitalWrite(lockPin, HIGH);   // Trigger the lock
      delay(3000);                   // Lock for 3 seconds
      digitalWrite(lockPin, LOW);    // Release the lock
    } else {
      digitalWrite(buzzerPin, HIGH); // Trigger the buzzer
      delay(3000);                   // Buzz for 3 seconds
      digitalWrite(buzzerPin, LOW);  // Stop buzzing
    }
  } else {
    Serial.print("Error: ");
    Serial.println(httpResponseCode);
  }
  
  // Free the memory used by the photo buffer
  esp_camera_fb_return(fb);
  
  // End the HTTP request
  http.end();

  // Delay before taking the next photo
  delay(5000);
}
