#include "authorize.h"

#include <type_traits>

#include "../../config.h"
#include "common/byte_array.h"
#include "state/cloud_request.h"
#include "state/configuration.h"

namespace oww::state::tag {
using namespace authorize;
using namespace config::tag;

std::optional<Authorize> OnStart(Authorize state, Start &start,
                                 Ntag424 &ntag_interface) {
  auto auth_challenge =
      ntag_interface.AuthenticateWithCloud_Begin(start.key_number);
  if (!auth_challenge) {
    return state.WithNestedState(Failed{
        .tag_status = auth_challenge.error(),
        .message = String::format("AuthenticateEV2First_Part1 failed [dna:%d]",
                                  auth_challenge.error())});
  }
  return state.WithNestedState(
      NtagChallenge{.auth_challenge = auth_challenge.value()});
}

std::optional<Authorize> OnNtagChallenge(Authorize state,
                                         NtagChallenge &challenge,
                                         CloudRequest &cloud_interface) {
  auto encoded_challenge = ToHexString(challenge.auth_challenge);
  auto encoded_uid = ToHexString(state.tag_uid);
  Variant payload;
  payload.set("uid", Variant(encoded_uid.c_str()));
  payload.set("challenge", Variant(encoded_challenge.c_str()));
  return state.WithNestedState(
      CloudChallenge{.response = cloud_interface.SendTerminalRequest(
                         "authorize-part1", payload)});
}

std::optional<Authorize> OnCloudChallenge(Authorize state,
                                          CloudChallenge &clound_challenge,
                                          Ntag424 &ntag_interface) {
  auto response = clound_challenge.response;
  if (!response->result.has_value()) {
    if (millis() < response->deadline) return {};
    return state.WithNestedState(Failed{.error = ErrorType::kTimeout});
  }

  auto request_result = response->result.value();
  if (!request_result) {
    return state.WithNestedState(Failed{.error = request_result.error()});
  }

  auto payload = request_result.value();

  // FIXME
  return {};
}

// ---- Loop dispatchers ------------------------------------------------------

std::optional<Authorize> StateLoop(Authorize state,
                                   Configuration &configuration,
                                   CloudRequest &cloud_interface) {
  if (auto nested = std::get_if<NtagChallenge>(state.state.get())) {
    return OnNtagChallenge(state, *nested, cloud_interface);
  }

  return {};
}

std::optional<Authorize> NfcLoop(Authorize state, Ntag424 &ntag_interface) {
  if (auto nested = std::get_if<Start>(state.state.get())) {
    return OnStart(state, *nested, ntag_interface);
  } else if (auto nested = std::get_if<CloudChallenge>(state.state.get())) {
    return OnCloudChallenge(state, *nested, ntag_interface);

  }
  return {};
}

Authorize Authorize::WithNestedState(authorize::State nested_state) {
  return Authorize{.tag_uid = this->tag_uid,
                   .state = std::make_shared<State>(nested_state)};
}

}  // namespace oww::state::tag