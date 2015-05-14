#include <pebble.h>
#include "Dani.h"

int main(void) {
  init();
  app_event_loop();
  deinit();
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

static void main_window_load(Window *window) {
  fill_hour_frames();
  set_postions();
  text_layer_set_font(text_layers[0], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text(text_layers[0], "00");
  text_layer_set_background_color(text_layers[0], GColorClear);
  text_layer_set_text_color(text_layers[0], GColorWhite);

  text_layer_set_font(text_layers[1], fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text(text_layers[1], "12");
  text_layer_set_background_color(text_layers[1], GColorClear);
  text_layer_set_text_color(text_layers[1], GColorWhite);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layers[0]));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layers[1]));

  update_time();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layers[0]);
  text_layer_destroy(text_layers[1]);
  property_animation_destroy(s_property_animation);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char hours[] = "00";
  static char mins[] = "00";

  strftime(hours, sizeof(hours), "%I", tick_time);
  strftime(mins, sizeof(mins), "%M", tick_time);

  trigger_custom_animation(tick_time);
  text_layer_set_text(text_layers[0], mins);
  text_layer_set_text(text_layers[1], hours);

  static char single_hour[] = "";
  single_hour[0] = (char) hours[1];

  if(tick_time->tm_hour < 10) {
    text_layer_set_text(text_layers[1], single_hour);
  }
}

static void set_postions() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  int x_pos, y_pos, mins, hours;
  mins = tick_time->tm_min;
  hours = tick_time->tm_hour;

  if(hours > 12) {
    hours -= 12;
  }

  if((mins >= 0) && (mins <= 5)) {
    x_pos = (mins * 6) + 60;
    text_layers[0] = text_layer_create(GRect(x_pos, 0, TM_SIZE, TM_SIZE));
  }

  if((mins >= 6) && (mins <= 24)) {
    y_pos = (mins - 6) * 8;
    text_layers[0] = text_layer_create(GRect(MIN_LEFT, y_pos, TM_SIZE, TM_SIZE));
  }

  if((mins >= 25) && (mins <= 35)) {
    x_pos = (35 - mins) * 13;
    text_layers[0] = text_layer_create(GRect(x_pos, MIN_BOTTOM, TM_SIZE, TM_SIZE));
  }

  if((mins >= 36) && (mins <= 54)) {
    y_pos = (54 - mins) * 8;
    text_layers[0] = text_layer_create(GRect(0, y_pos, TM_SIZE, TM_SIZE));
  }

  if((mins >= 55) && (mins <= 59)) {
    x_pos = (mins - 55) * 6;
    text_layers[0] = text_layer_create(GRect(x_pos, 0, TM_SIZE, TM_SIZE));
  }

  text_layers[1] = text_layer_create(hour_pos[hours]);
}

static void trigger_custom_animation(struct tm *tick_time) {
  GRect from_frame_mins = layer_get_frame((Layer *)text_layers[0]);
  GRect to_frame_mins;

  GRect from_frame_hours = layer_get_frame((Layer *)text_layers[1]);
  GRect to_frame_hours;

  int x_pos, y_pos, mins, hours;
  hours = tick_time->tm_hour;
  mins = tick_time->tm_min;

  if(hours > 12) {
    hours -= 12;
  }

  if((mins >= 0) && (mins <= 5)) {
    mins += 60;
  }

  if((mins >= 6) && (mins <= 24)) {
    y_pos = ((mins - 6) * 7) + 13;
    to_frame_mins = GRect(MIN_LEFT, y_pos, TM_SIZE, TM_SIZE);
  }

  if((mins >= 36) && (mins <= 54)) {
    y_pos = ((54 - mins) * 7) + 13;
    to_frame_mins = GRect(0, y_pos, TM_SIZE, TM_SIZE);
  }

  if((mins >= 25) && (mins <= 35)) {
    x_pos = ((35 - mins) * 13);
    to_frame_mins = GRect(x_pos, MIN_BOTTOM, TM_SIZE, TM_SIZE);
  }

  if((mins >= 55) && (mins <= 65)) {
    x_pos = ((mins - 55) * 13) + 2;
    to_frame_mins = GRect(x_pos, 0, TM_SIZE, TM_SIZE);
  }

  to_frame_hours = hour_pos[hours];

  s_property_animation = property_animation_create_layer_frame((Layer *)text_layers[1], &from_frame_hours, &to_frame_hours);
  animation_schedule((Animation*) s_property_animation);

  s_property_animation = property_animation_create_layer_frame((Layer *)text_layers[0], &from_frame_mins, &to_frame_mins);
  animation_schedule((Animation*) s_property_animation);
}

static void fill_hour_frames() {
  hour_pos[0] = GRect(INNER_LEFT + 43, INNER_TOP, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[1] = GRect(INNER_RIGHT + 7, INNER_TOP, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[2] = GRect(INNER_RIGHT - 1, (INNER_BOTTOM / 4) + 2, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[3] = GRect(INNER_RIGHT - 2, INNER_BOTTOM / 2, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[4] = GRect(INNER_RIGHT - 4, INNER_BOTTOM - 30, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[5] = GRect(INNER_RIGHT - 2, INNER_BOTTOM, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[6] = GRect(INNER_LEFT + 45, INNER_BOTTOM, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[7] = GRect(INNER_LEFT - 1, INNER_BOTTOM, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[8] = GRect(INNER_LEFT - 1, INNER_BOTTOM - 30, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[9] = GRect(INNER_LEFT - 1, INNER_BOTTOM / 2, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[10] = GRect(INNER_LEFT, (INNER_BOTTOM / 4) + 2, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[11] = GRect(INNER_LEFT, INNER_TOP, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[12] = GRect(INNER_LEFT + 43, INNER_TOP, TH_SIZE_X, TH_SIZE_Y);
}
