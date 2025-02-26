#pragma once

// Adds extra logging during development
#if !defined(DEVELOPMENT_BUILD)
#define DEVELOPMENT_BUILD 1
#endif

namespace config {

namespace ui {
constexpr auto logtag = "UI";

constexpr os_thread_prio_t thread_priority = 3;
// Stack size is recommended to be 8k+
// https:  // docs.lvgl.io/master/intro/introduction.html#requirements
constexpr size_t thread_stack_size = 8 * 1024;

namespace display {

constexpr auto logtag = "Display";
constexpr auto resolution_horizontal = 240;
constexpr auto resolution_vertical = 320;

constexpr int8_t pin_reset = D6;
constexpr int8_t pin_chipselect = D5;
constexpr int8_t pin_datacommand = D10;
constexpr int8_t pin_backlight = D7;
constexpr int8_t pin_touch_chipselect = S3;
constexpr int8_t pin_touch_irq = S4;
}  // namespace display

}  // namespace ui

namespace nfc {

constexpr auto logtag = "Nfc";

constexpr int8_t pin_irq = D17;
constexpr int8_t pin_reset = D15;

constexpr os_thread_prio_t thread_priority = OS_THREAD_PRIORITY_DEFAULT;
constexpr size_t thread_stack_size = OS_THREAD_STACK_SIZE_DEFAULT_HIGH;

}  // namespace nfc

namespace tag {

constexpr byte key_application = 0;
constexpr byte key_terminal = 1;
constexpr byte key_authorization = 2;

}  // namespace tag

}  // namespace config