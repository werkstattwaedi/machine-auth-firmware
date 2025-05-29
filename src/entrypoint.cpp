/**
 * @brief Entrypoint for terminal firmware.
 */

#include "Adafruit_MPR121.h"
#include "common.h"
#include "neopixel.h"
#include "nfc/nfc_tags.h"
#include "state/state.h"
#include "ui/ui.h"

#define PIXEL_PIN SPI
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812B

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

SerialLogHandler logHandler(
    // Logging level for non-application messages
    LOG_LEVEL_WARN, {
                        {"app", LOG_LEVEL_ALL},
                        {"cloud_request", LOG_LEVEL_ALL},
                        {"config", LOG_LEVEL_ALL},
                        {"display", LOG_LEVEL_WARN},
                        {"nfc", LOG_LEVEL_ALL},
                        {"pn532", LOG_LEVEL_WARN},
                    });

using namespace oww::state;

std::shared_ptr<State> state_;
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
Adafruit_MPR121 cap = Adafruit_MPR121();

void setup() {
  Log.info("machine-auth-firmware starting");

  {
    // create state_
    state_ = std::make_shared<State>();
    auto config = std::make_unique<Configuration>(std::weak_ptr(state_));
    state_->Begin(std::move(config));
  }

  auto display_setup_result = oww::ui::UserInterface::instance().Begin(state_);

#if defined(DEVELOPMENT_BUILD)
  // Await the terminal connections, so that all log messages during setup are
  // not skipped.
  waitFor(Serial.isConnected, 5000);
#endif

  if (!display_setup_result) {
    Log.info("Failed to start display = %d", (int)display_setup_result.error());
  }

  Status nfc_setup_result = NfcTags::instance().Begin(state_);
  Log.info("NFC Status = %d", (int)nfc_setup_result);
  strip.begin();
  strip.show();  // Initialize all pixels to 'off'

  strip.setPixelColor(0, 10, 0, 0);
  strip.setPixelColor(1, 0, 10, 0);
  strip.setPixelColor(2, 0, 0, 10);
  strip.show();

  if (!cap.begin(0x5A)) {
    Log.info("CAP not found");
  }
  Log.info("CAP");
}

uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void loop() {
  // state_->Loop();
        // Log.warn("loop");



  currtouched = cap.touched();

  for (int i = 0; i < 12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i))) {
      Log.warn("%d touched", i);
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i))) {
      Log.warn("%d released", i);
    }
  }

  lasttouched = currtouched;
}
