#pragma once

#include "../../common.h"
#include "../state_request.h"
#include "nfc/driver/Ntag424.h"

namespace oww::state::tag {

namespace personalize {

struct Wait {
  const system_tick_t timeout = CONCURRENT_WAIT_FOREVER;
};

struct RequestedKeys {
  const RequestId request_id = RequestId::kInvalid;
};

struct UpdateTag {
  const std::array<std::byte, 16> application_key;
  const std::array<std::byte, 16> terminal_key;
  const std::array<std::byte, 16> card_key;
  const std::array<std::byte, 16> reserved_1_key;
  const std::array<std::byte, 16> reserved_2_key;
};

struct Completed {};

struct Failed {};

using State = std::variant<Wait, RequestedKeys, UpdateTag, Completed, Failed>;

State PersonalizeTag(Ntag424 &ntag_interface, std::array<std::byte, 7> tag_uid,
                     UpdateTag update_tag);

}  // namespace personalize

struct Personalize {
  std::array<std::byte, 7> tag_uid;
  std::shared_ptr<personalize::State> state;
};

}  // namespace oww::state::tag
