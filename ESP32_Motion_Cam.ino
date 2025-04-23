#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#define PIR_PIN 13
#define FLASH_LED_PIN 4

WebServer server(80);
String imageDir = "/images";

void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;
  esp_camera_init(&config);
}

void setupSD() {
  if (!SD_MMC.begin()) return;
  if (!SD_MMC.exists(imageDir)) SD_MMC.mkdir(imageDir);
}

String getTimeStamp() {
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);
  char buf[20];
  strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", t);
  return String(buf);
}

void capturePhoto() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) return;

  String path = imageDir + "/photo_" + getTimeStamp() + ".jpg";
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if (file) {
    file.write(fb->buf, fb->len);
    file.close();
  }
  esp_camera_fb_return(fb);
}

void handleRoot() {
  String html = "<html><head><style>body{background:#000;color:#4af24a;font-family:'Courier New'}button{margin-left:10px;}</style></head><body><h1>ESP32 Motion Camera</h1><ul>";
  File root = SD_MMC.open(imageDir);
  File entry;
  while ((entry = root.openNextFile())) {
    String name = String(entry.name());
    html += "<li><a href='" + name + "'>" + name + "</a>";
    html += "<form method='POST' action='/delete?file=" + name + "' style='display:inline'><button>Delete</button></form></li>";
    entry.close();
  }
  html += "</ul></body></html>";
  server.send(200, "text/html", html);
}

void handleImage() {
  String path = server.uri();
  File file = SD_MMC.open(path);
  if (!file) {
    server.send(404, "text/plain", "Not Found");
    return;
  }
  server.streamFile(file, "image/jpeg");
  file.close();
}

void handleDelete() {
  if (!server.hasArg("file")) {
    server.send(400, "text/plain", "Missing file arg");
    return;
  }
  SD_MMC.remove(server.arg("file"));
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(FLASH_LED_PIN, OUTPUT);

  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) delay(500);
  configTime(0, 0, "pool.ntp.org");

  startCamera();
  setupSD();

  server.on("/", handleRoot);
  server.onNotFound(handleImage);
  server.on("/delete", HTTP_POST, handleDelete);
  server.begin();
}

void loop() {
  server.handleClient();
  if (digitalRead(PIR_PIN) == HIGH) {
    digitalWrite(FLASH_LED_PIN, HIGH);
    delay(100);
    capturePhoto();
    digitalWrite(FLASH_LED_PIN, LOW);
    delay(1000);
  }
}
