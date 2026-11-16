#include "../../components/smart_display/overlay_card.h"
#include <cassert>

static lv_obj_t *box(lv_obj_t *parent, int x, int y, int w, int h) {
  auto *o = lv_obj_create(parent);
  lv_obj_remove_style_all(o);
  lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_pos(o, x, y);
  lv_obj_set_size(o, w, h);
  return o;
}
int main() {
  lv_init();
  auto *display = lv_display_create(480, 480);
  ui::configure(314, "compact");
  auto *root = box(lv_screen_active(), 0, 0, 480, 480);
  auto *back = box(root, 22, 18, 88, 88);
  auto *title = box(root, 130, 18, 220, 88);
  // A third header child is an action key, not scrolling content.
  auto *power = box(root, 370, 18, 88, 88);
  auto *control = box(root, 22, 120, 436, 100);
  assert(!overlay_card::scroll_if_needed(root, 110));
  auto *last = box(root, 22, 530, 436, 100);
  int clicks = 0;
  lv_obj_add_event_cb(last, [](lv_event_t *e) {
    ++*static_cast<int *>(lv_event_get_user_data(e));
  }, LV_EVENT_CLICKED, &clicks);
  auto *body = overlay_card::scroll_if_needed(root, 110);
  assert(body && lv_obj_get_parent(body) == root);
  assert(lv_obj_get_parent(back) == root && lv_obj_get_parent(title) == root);
  assert(lv_obj_get_parent(power) == root);
  assert(lv_obj_get_parent(control) == body && lv_obj_get_parent(last) == body);
  assert(lv_obj_get_y(last) == 420 && lv_obj_get_x(last) == 22);
  assert(lv_obj_get_y(body) == 110);
  assert(lv_obj_get_height(body) == 480 - overlay_card::pad() - 110);
  assert(lv_obj_get_scroll_dir(body) == LV_DIR_VER);
  assert(!lv_obj_has_flag(root, LV_OBJ_FLAG_SCROLLABLE));
  lv_obj_scroll_to_y(body, 9999, LV_ANIM_OFF);
  assert(lv_obj_get_scroll_y(body) > 0);
  assert(lv_obj_get_scroll_bottom(body) == 0);
  assert(lv_obj_get_y(back) == 18 && lv_obj_get_y(title) == 18);
  lv_obj_send_event(last, LV_EVENT_CLICKED, nullptr);
  assert(clicks == 1);
  lv_obj_delete(root);

  root = box(lv_screen_active(), 0, 0, 480, 480);
  box(root, 22, 18, 88, 88); box(root, 130, 18, 220, 88);
  auto *hidden = box(root, 22, 600, 436, 100);
  lv_obj_add_flag(hidden, LV_OBJ_FLAG_HIDDEN);
  box(root, 22, 120, 436, 100);
  assert(!overlay_card::scroll_if_needed(root, 110));
  auto *footer = box(root, 0, 400, 480, 80);
  lv_obj_add_flag(footer, LV_OBJ_FLAG_FLOATING);
  box(root, 22, 350, 436, 100);
  body = overlay_card::scroll_if_needed(root, 110);
  assert(body && lv_obj_get_height(body) == 290);
  assert(lv_obj_get_parent(footer) == root);
  lv_obj_delete(root);
  lv_display_delete(display);
  lv_deinit();
}
