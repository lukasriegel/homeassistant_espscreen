#include "screen_text_en.h"
#define LIGHT_CONTROLS_TEST
#include "../components/smart_display/light_controls.h"
#include <cassert>
// The colour card's values (components/smart_display/light_controls.h): what it reads from the light's tile. Home
// Assistant's attributes reach the tile through page_receiver.cpp; the card keeps nothing of its own.
int main() {
  using namespace light_controls;
  // A dense 480-square compact screen must show brightness below colour and temperature.
  // The old minimum added a full touch target to the label and pushed the final track off glass.
  ui::configure(314, "compact");
  const int top = ui::px(52), foot = ui::px(8), gap = ui::px(5);
  const int room = 480 - top - foot;
  const int pitch = row_pitch(room, ui::px(60), gap, 31, 3);
  assert(top + 3 * pitch - gap <= 480 - foot);
  assert(pitch - gap >= ui::touch_min());
  assert(pitch - gap >= 31 + ui::px(12) + ui::px(10));
  // Reference compact and standard screens retain their existing pitch.
  ui::configure(143, "compact");
  assert(row_pitch(300, 60, 5, 14, 3) == 66);
  ui::configure(170, "standard");
  assert(row_pitch(400, 122, 12, 22, 3) == 122);
  Values v;
  // A white shade waits for the light's own range, a sane one.
  assert(!v.temperature_ready());
  v.minimum = 2200; assert(!v.temperature_ready());
  v.maximum = 5000; assert(v.temperature_ready());
  v.maximum = 2000; assert(!v.temperature_ready());
  v.maximum = 30000; assert(!v.temperature_ready());
  v.maximum = 5000;
  assert(clamp(1800, v.minimum, v.maximum) == 2200);
  assert(clamp(6500, v.minimum, v.maximum) == 5000);
  assert(clamp(-1, 0, 360) == 0); assert(clamp(999, 0, 360) == 360);
  // The card is laid out again only when the light gains or loses a row; a new value is painted in place.
  Values a, b;
  a.color = true; b.color = true; b.hue = 120; b.brightness = 40;
  assert(a.same_rows(b));
  b.temperature = true;
  assert(!a.same_rows(b));
}
