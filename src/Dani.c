#include <pebble.h>
#include "Dani.h"

void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
  Tuple *inverted_tuple = dict_find(received, CONFIG_INVERTED);
  Tuple *fontsize_tuple = dict_find(received, CONFIG_FONTSIZE);

  if(inverted_tuple && fontsize_tuple) {
    inverted_value = inverted_tuple->value->int8;
    font_size_value = fontsize_tuple->value->int8;

    if(inverted_value == 0) {
      window_set_background_color(s_main_window, GColorBlack);
      text_layer_set_text_color(text_layers[0], GColorWhite);
      text_layer_set_text_color(text_layers[1], GColorWhite);
    } else {
      window_set_background_color(s_main_window, GColorWhite);
      text_layer_set_text_color(text_layers[0], GColorBlack);
      text_layer_set_text_color(text_layers[1], GColorBlack);
    }

    if(font_size_value == 0) {
      text_layer_set_font(text_layers[0], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
      text_layer_set_font(text_layers[1], fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
    } else {
      text_layer_set_font(text_layers[0], fonts_get_system_font(FONT_KEY_GOTHIC_24));
      text_layer_set_font(text_layers[1], fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    }

    fill_hour_frames();
    update_time();

    persist_write_int(CONFIG_INVERTED, inverted_value);
    persist_write_int(CONFIG_FONTSIZE, font_size_value);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "No Options Set");
  }
}

int main(void) {
  init();

  app_message_register_inbox_received(in_received_handler);
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);

  app_event_loop();
  deinit();
}

static void init() {
  s_main_window = window_create();

  if(persist_exists(CONFIG_INVERTED)) {
    inverted_value = persist_read_int(CONFIG_INVERTED);
  } else {
    inverted_value = 0;
  }

  if(persist_exists(CONFIG_FONTSIZE)) {
    font_size_value = persist_read_int(CONFIG_FONTSIZE);
  } else {
    font_size_value = 0;
  }

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
  if(inverted_value == 0) {
    window_set_background_color(s_main_window, GColorBlack);
  } else {
    window_set_background_color(s_main_window, GColorWhite);
  }

  updateTimeLayers();
  fill_hour_frames();

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layers[0]));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layers[1]));

  update_time();
}

static void main_window_unload() {
  text_layer_destroy(text_layers[0]);
  text_layer_destroy(text_layers[1]);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap Used: %d", heap_bytes_used());
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap Free: %d", heap_bytes_free());
  static char hours[] = "00";
  static char mins[] = "00";

  strftime(hours, sizeof(hours), "%I", tick_time);
  strftime(mins, sizeof(mins), "%M", tick_time);

  update_positions(tick_time);
  text_layer_set_text(text_layers[0], mins);

  static char single_hour[] = "";
  single_hour[0] = (char) hours[1];

  int hour = tick_time->tm_hour;

  if(hour > 12) {
    hour -= 12;
  }

  if(hour < 10 && hour > 0) {
    text_layer_set_text(text_layers[1], single_hour);
  } else {
    text_layer_set_text(text_layers[1], hours);
  }
}

static void update_positions(struct tm *tick_time) {
  GRect to_frame_mins;
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

  if(font_size_value == 0) {
    nextFrameNormal(x_pos, y_pos, mins, hours, to_frame_mins, to_frame_hours);
  } else {
    nextFrameLarge(x_pos, y_pos, mins, hours, to_frame_mins, to_frame_hours);
  }
}

static void updateTimeLayers() {
  text_layers[0] = text_layer_create(GRect(0, 0, TM_SIZE, TM_SIZE));
  text_layers[1] = text_layer_create(GRect(0, 0, TM_SIZE, TM_SIZE));

  if(font_size_value == 0) {
    text_layer_set_font(text_layers[0], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    text_layer_set_font(text_layers[1], fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  } else {
    text_layer_set_font(text_layers[0], fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_font(text_layers[1], fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  }

  if(inverted_value == 0) {
    text_layer_set_text_color(text_layers[0], GColorWhite);
    text_layer_set_text_color(text_layers[1], GColorWhite);
  } else {
    text_layer_set_text_color(text_layers[0], GColorBlack);
    text_layer_set_text_color(text_layers[1], GColorBlack);
  }

  text_layer_set_text(text_layers[0], "00");
  text_layer_set_background_color(text_layers[0], GColorClear);

  text_layer_set_text(text_layers[1], "12");
  text_layer_set_background_color(text_layers[1], GColorClear);
}

static void nextFrameNormal(int x_pos, int y_pos, int mins, int hours, GRect to_frame_mins, GRect to_frame_hours) {
  //RIGHT
  if((mins >= 6) && (mins <= 24)) {
    y_pos = ((mins - 6) * 7) + 13;
    to_frame_mins = GRect(MIN_LEFT, y_pos, TM_SIZE, TM_SIZE);
  }

  //LEFT
  if((mins >= 36) && (mins <= 54)) {
    y_pos = ((54 - mins) * 7) + 13;
    to_frame_mins = GRect(50, y_pos, TM_SIZE, TM_SIZE);
  }

  //BOTTOM
  if((mins >= 25) && (mins <= 35)) {
    x_pos = ((35 - mins) * 8) + 50;
    to_frame_mins = GRect(x_pos, MIN_BOTTOM, TM_SIZE, TM_SIZE);
  }

  //TOP
  if((mins >= 55) && (mins <= 65)) {
    x_pos = ((mins - 55) * 8) + 50;
    to_frame_mins = GRect(x_pos, 0, TM_SIZE, TM_SIZE);
  }

  to_frame_hours = hour_pos[hours];

  layer_set_frame((Layer *)text_layers[0], to_frame_mins);
  layer_set_frame((Layer *)text_layers[1], to_frame_hours);
}

static void nextFrameLarge(int x_pos, int y_pos, int mins, int hours, GRect to_frame_mins, GRect to_frame_hours) {
  //RIGHT
  if((mins >= 6) && (mins <= 24)) {
    y_pos = ((mins - 6) * 7) + 4;
    to_frame_mins = GRect(MIN_LEFT-5, y_pos, TM_SIZE, TM_SIZE);
  }

  //LEFT
  if((mins >= 36) && (mins <= 54)) {
    y_pos = ((54 - mins) * 7) + 4;
    to_frame_mins = GRect(45, y_pos, TM_SIZE, TM_SIZE);
  }

  //BOTTOM
  if((mins >= 25) && (mins <= 35)) {
    x_pos = ((35 - mins) * 8) + 45;
    to_frame_mins = GRect(x_pos, MIN_BOTTOM-10, TM_SIZE, TM_SIZE);
  }

  //TOP
  if((mins >= 55) && (mins <= 65)) {
    x_pos = ((mins - 55) * 8) + 45;
    to_frame_mins = GRect(x_pos, -8, TM_SIZE, TM_SIZE);
  }

  to_frame_hours = hour_pos[hours];

  layer_set_frame((Layer *)text_layers[0], to_frame_mins);
  layer_set_frame((Layer *)text_layers[1], to_frame_hours);
}

static void fill_hour_frames() {
  if(font_size_value == 0) {
    hour_pos[0] = GRect(INNER_LEFT + 53, INNER_TOP, TM_SIZE, TM_SIZE);
    hour_pos[1] = GRect(INNER_RIGHT + 4, INNER_TOP, TM_SIZE, TM_SIZE);
    hour_pos[2] = GRect(INNER_RIGHT - 4, (INNER_BOTTOM / 4) + 8, TM_SIZE, TM_SIZE);
    hour_pos[3] = GRect(INNER_RIGHT - 5, (INNER_BOTTOM / 2) + 5, TM_SIZE, TM_SIZE);
    hour_pos[4] = GRect(INNER_RIGHT - 7, INNER_BOTTOM - 27, TM_SIZE, TM_SIZE);
    hour_pos[5] = GRect(INNER_RIGHT - 5, INNER_BOTTOM, TM_SIZE, TM_SIZE);
    hour_pos[6] = GRect(INNER_LEFT + 58, INNER_BOTTOM, TM_SIZE, TM_SIZE);
    hour_pos[7] = GRect(INNER_LEFT, INNER_BOTTOM, TM_SIZE, TM_SIZE);
    hour_pos[8] = GRect(INNER_LEFT - 1, INNER_BOTTOM - 27, TM_SIZE, TM_SIZE);
    hour_pos[9] = GRect(INNER_LEFT - 1, (INNER_BOTTOM / 2) + 5, TM_SIZE, TM_SIZE);
    hour_pos[10] = GRect(INNER_LEFT - 14, (INNER_BOTTOM / 4) + 8, TM_SIZE, TM_SIZE);
    hour_pos[11] = GRect(INNER_LEFT - 2, INNER_TOP, TM_SIZE, TM_SIZE);
    hour_pos[12] = GRect(INNER_LEFT + 53, INNER_TOP, TM_SIZE, TM_SIZE);
  } else {
    hour_pos[0] = GRect(INNER_LEFT + 53, INNER_TOP, TM_SIZE, TM_SIZE);
    hour_pos[1] = GRect(INNER_RIGHT - 1, INNER_TOP, TM_SIZE, TM_SIZE);
    hour_pos[2] = GRect(INNER_RIGHT - 9, (INNER_BOTTOM / 4) - 6, TM_SIZE, TM_SIZE);
    hour_pos[3] = GRect(INNER_RIGHT - 10, (INNER_BOTTOM / 2) - 3, TM_SIZE, TM_SIZE);
    hour_pos[4] = GRect(INNER_RIGHT - 12, INNER_BOTTOM - 41, TM_SIZE, TM_SIZE);
    hour_pos[5] = GRect(INNER_RIGHT - 10, INNER_BOTTOM - 14, TM_SIZE, TM_SIZE);
    hour_pos[6] = GRect(INNER_LEFT + 53, INNER_BOTTOM - 14, TM_SIZE, TM_SIZE);
    hour_pos[7] = GRect(INNER_LEFT - 6, INNER_BOTTOM - 14, TM_SIZE, TM_SIZE);
    hour_pos[8] = GRect(INNER_LEFT - 7, INNER_BOTTOM - 41, TM_SIZE, TM_SIZE);
    hour_pos[9] = GRect(INNER_LEFT - 7, (INNER_BOTTOM / 2) - 3, TM_SIZE, TM_SIZE);
    hour_pos[10] = GRect(INNER_LEFT - 20, (INNER_BOTTOM / 4) - 6, TM_SIZE, TM_SIZE);
    hour_pos[11] = GRect(INNER_LEFT - 8, INNER_TOP, TM_SIZE, TM_SIZE);
    hour_pos[12] = GRect(INNER_LEFT + 50, INNER_TOP, TM_SIZE, TM_SIZE);
  }
}
