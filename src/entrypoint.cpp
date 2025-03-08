/**
 * @brief Entrypoint for terminal firmware.
 */

#include "common.h"
#include "nfc/nfc_tags.h"
#include "state/state.h"
#include "ui/ui.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

SerialLogHandler logHandler(
    // Logging level for non-application messages
    LOG_LEVEL_WARN, {
                        {"app", LOG_LEVEL_ALL},
                        {PN532_LOGTAG, LOG_LEVEL_WARN},
                        // {config::display::logtag, LOG_LEVEL_ALL},
                        {oww::state::configuration::logtag, LOG_LEVEL_ALL},
                        {config::nfc::logtag, LOG_LEVEL_WARN},
                    });

using namespace oww::state;

std::shared_ptr<State> state_;

void setup() {
  Log.info("machine-auth-firmware starting");

  {
    // create state_
    state_.reset(new State());
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
}

void loop() { state_->Loop(); }
