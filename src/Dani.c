#include <pebble.h>
#include "Dani.h"

void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
  Tuple *inverted_tuple = dict_find(received, CONFIG_INVERTED);
  //Tuple *innerhours_tuple = dict_find(received, CONFIG_INNERHOURS);

  if(inverted_tuple) {
    strcpy(inverted_value, inverted_tuple->value->cstring);

    if(strcmp(inverted_value, "invf") == 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Black");
      window_set_background_color(s_main_window, GColorBlack);
      text_layer_set_text_color(text_layers[0], GColorWhite);
      text_layer_set_text_color(text_layers[1], GColorWhite);
    } else {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "White");
      window_set_background_color(s_main_window, GColorWhite);
      text_layer_set_text_color(text_layers[0], GColorBlack);
      text_layer_set_text_color(text_layers[1], GColorBlack);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "No Background Option Set");
  }

  persist_write_string(CONFIG_INVERTED, inverted_value);
  APP_LOG(APP_LOG_LEVEL_INFO, "%s", inverted_value);

  vibes_short_pulse();

  // if(innerhours_tuple) {
  //   strcpy(innerhours_value, innerhours_tuple->value->cstring);
  //
  //   if(strcmp(innerhours_value, "0") == 0) {
  //     APP_LOG(APP_LOG_LEVEL_DEBUG, "Normal");
  //   } else {
  //     APP_LOG(APP_LOG_LEVEL_DEBUG, "Inside");
  //   }
  // } else {
  //   APP_LOG(APP_LOG_LEVEL_DEBUG, "No Hours Option Set");
  // }
}

int main(void) {
  init();

  //register for messages
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
    persist_read_string(CONFIG_INVERTED, inverted_value, sizeof(inverted_value));
    APP_LOG(APP_LOG_LEVEL_INFO, "Stored Value: %s", inverted_value);

    if(strcmp(inverted_value, "inv") == 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "poop");
    }
  } else {
    strcpy(inverted_value, "invf");
  }

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // persist_write_string(CONFIG_INVERTED, inverted_value);
  //persist_write_string(CONFIG_INNERHOURS, innerhours_value);
  window_destroy(s_main_window);
}

static void main_window_load(Window *window) {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  current_hour = tick_time->tm_hour;
  startup = true;

  if(strcmp(inverted_value, "invt") == 0) {
    window_set_background_color(s_main_window, GColorWhite);
  } else {
    window_set_background_color(s_main_window, GColorBlack);
  }

  fill_hour_frames();
  update_time(tick_time);
  updateTimeLayers();

  free(tick_time);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layers[0]));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layers[1]));
}

static void main_window_unload() {
  text_layer_destroy(text_layers[0]);
  text_layer_destroy(text_layers[1]);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void update_time(struct tm *tick_time) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap Used: %d", heap_bytes_used());
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap Free: %d", heap_bytes_free());
  static char hours[] = "00";
  static char mins[] = "00";

  strftime(hours, sizeof(hours), "%I", tick_time);
  strftime(mins, sizeof(mins), "%M", tick_time);

  trigger_custom_animation(tick_time);
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

  current_hour = tick_time->tm_hour;
}

static void anim_stopped_handler_1(Animation *animation, bool finished, void *context) {
  #ifdef PBL_PLATFORM_APLITE
    property_animation_destroy(s_property_animation_1);
  #endif
}

static void trigger_custom_animation(struct tm *tick_time) {
  int x_pos, y_pos, mins, hours;
  hours = tick_time->tm_hour;
  mins = tick_time->tm_min;

  if(hours > 12) {
    hours -= 12;
  }

  if((mins >= 0) && (mins <= 5)) {
    mins += 60;
  }

  if(startup) {
    //RIGHT
    if((mins >= 6) && (mins <= 24)) {
      y_pos = ((mins - 6) * 7) + 13;
      text_layers[0] = text_layer_create(GRect(MIN_LEFT, y_pos, TM_SIZE, TM_SIZE));
    }

    //LEFT
    if((mins >= 36) && (mins <= 54)) {
      y_pos = ((54 - mins) * 9) + 13;
      text_layers[0] = text_layer_create(GRect(50, y_pos, TM_SIZE, TM_SIZE));
    }

    //BOTTOM
    if((mins >= 25) && (mins <= 35)) {
      x_pos = ((35 - mins) * 8) + 50;
      text_layers[0] = text_layer_create(GRect(x_pos, MIN_BOTTOM, TM_SIZE, TM_SIZE));
    }

    //TOP
    if((mins >= 55) && (mins <= 65)) {
      x_pos = ((mins - 55) * 8) + 50;
      text_layers[0] = text_layer_create(GRect(x_pos, 0, TM_SIZE, TM_SIZE));
    }

    text_layers[1] = text_layer_create(hour_pos[hours]);

    startup = false;
  } else {
    GRect from_frame_mins = layer_get_frame((Layer *)text_layers[0]);
    GRect to_frame_mins;

    GRect from_frame_hours = layer_get_frame((Layer *)text_layers[1]);
    GRect to_frame_hours;

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

    if(tick_time->tm_hour != current_hour) {
      s_property_animation_1 = property_animation_create_layer_frame((Layer *)text_layers[1],
        &from_frame_hours, &to_frame_hours);
      animation_set_handlers((Animation*)s_property_animation_1, (AnimationHandlers) {
        .stopped = anim_stopped_handler_1
      }, NULL);
      animation_schedule((Animation*) s_property_animation_1);
    }
  }
}

static void fill_hour_frames() {
  hour_pos[0] = GRect(INNER_LEFT + 53, INNER_TOP, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[1] = GRect(INNER_RIGHT + 4, INNER_TOP, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[2] = GRect(INNER_RIGHT - 4, (INNER_BOTTOM / 4) + 8, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[3] = GRect(INNER_RIGHT - 5, (INNER_BOTTOM / 2) + 5, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[4] = GRect(INNER_RIGHT - 7, INNER_BOTTOM - 27, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[5] = GRect(INNER_RIGHT - 5, INNER_BOTTOM, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[6] = GRect(INNER_LEFT + 58, INNER_BOTTOM, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[7] = GRect(INNER_LEFT, INNER_BOTTOM, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[8] = GRect(INNER_LEFT - 1, INNER_BOTTOM - 27, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[9] = GRect(INNER_LEFT - 1, (INNER_BOTTOM / 2) + 5, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[10] = GRect(INNER_LEFT - 14, (INNER_BOTTOM / 4) + 8, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[11] = GRect(INNER_LEFT - 2, INNER_TOP, TH_SIZE_X, TH_SIZE_Y);
  hour_pos[12] = GRect(INNER_LEFT + 53, INNER_TOP, TH_SIZE_X, TH_SIZE_Y);
}

static void updateTimeLayers() {
  text_layer_set_font(text_layers[0], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text(text_layers[0], "");
  text_layer_set_background_color(text_layers[0], GColorClear);

  text_layer_set_font(text_layers[1], fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text(text_layers[1], "");
  text_layer_set_background_color(text_layers[1], GColorClear);

  if(strcmp(inverted_value, "invt") == 0) {
    text_layer_set_text_color(text_layers[0], GColorBlack);
    text_layer_set_text_color(text_layers[1], GColorBlack);
  } else {
    text_layer_set_text_color(text_layers[0], GColorWhite);
    text_layer_set_text_color(text_layers[1], GColorWhite);
  }
}
