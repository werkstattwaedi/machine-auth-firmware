
#include "state.h"

namespace oww::state {


Status State::Begin(std::unique_ptr<Configuration> configuration) {
 

  configuration_ = std::move(configuration);
  configuration_->Begin();

 
  return Status::kOk;
}

void State::Loop() {}

void State::OnConfigChanged() { System.reset(RESET_REASON_CONFIG_UPDATE); }

void State::OnTagFound() { has_tag = true; }
void State::OnTagBlank() {}
void State::OnTagVerified() {}
void State::OnTagRejected() {}
void State::OnTagRemoved() {has_tag = false;}

}  // namespace oww::state
