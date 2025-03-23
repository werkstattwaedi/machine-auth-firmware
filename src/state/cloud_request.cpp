#include "cloud_request.h"

namespace oww::state {

void CloudRequest::Begin() {
  Particle.function("TerminalResponse", &CloudRequest::HandleTerminalResponse,
                    this);
}

std::shared_ptr<CloudResponse> CloudRequest::SendTerminalRequest(
    String command, Variant& payload, system_tick_t timeout_ms) {
  auto deadline = timeout_ms;
  if (deadline != CONCURRENT_WAIT_FOREVER) deadline += millis();

  auto response_container =
      std::make_shared<CloudResponse>(CloudResponse{.deadline = deadline});
  auto requestId = request_counter_++;
  Log.info("SendTerminalRequest(%s):%d", command.c_str(), requestId);

  inflight_requests_[requestId] = response_container;

  payload.set("type", Variant(command));
  payload.set("requestId", Variant(requestId));

  auto publish_future = Particle.publish("terminalRequest", payload, WITH_ACK);
  publish_future.onError([this, requestId](auto error) {
    HandleTerminalFailure(requestId, error);
  });

  return response_container;
}

int CloudRequest::HandleTerminalResponse(String response_payload) {
  auto payload = Variant::fromJSON(response_payload).asMap();

  auto type = payload.get("type").asString();
  auto requestId = payload.get("requestId").asInt();

  Log.info("HandleTerminalResponse(%s:%d)", type.c_str(), requestId);

  auto extracted = inflight_requests_.extract(requestId);
  if (extracted.empty()) return -1;

  extracted.mapped()->result = payload;
  return 0;
}

void CloudRequest::HandleTerminalFailure(int request_id,
                                         particle::Error error) {
  Log.error("HandleTerminalFailure(%d, %s)", request_id, error.message());

  auto extracted = inflight_requests_.extract(request_id);
  if (extracted.empty()) return;

  extracted.mapped()->result = tl::unexpected(ErrorType::kUnspecified);
}

}  // namespace oww::state