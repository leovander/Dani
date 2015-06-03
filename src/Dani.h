#define TM_SIZE 100
#define MIN_LEFT 130
#define MIN_BOTTOM 153

#define INNER_LEFT 27
#define INNER_RIGHT 112
#define INNER_TOP 9
#define INNER_BOTTOM 119

static Window *s_main_window;
static TextLayer *text_layers[2];
static GRect hour_pos[13];
static int inverted_value;
static int font_size_value;
//static char innerhours_value[1];

static void update_time();
static void main_window_load(Window *window);
static void main_window_unload();
static void fill_hour_frames();
static void updateTimeLayers();
static void nextFrameNormal(int x_pos, int y_pos, int mins, int hours, GRect to_frame_mins, GRect to_frame_hours);
static void nextFrameLarge(int x_pos, int y_pos, int mins, int hours, GRect to_frame_mins, GRect to_frame_hours);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_positions(struct tm *tick_time);
static void init();
static void deinit();

enum {
  CONFIG_INVERTED = 0x0,
  CONFIG_FONTSIZE = 0x1
};

// enum {
//   CONFIG_INVERTED = 0x0,
//   CONFIG_INNERHOURS = 0x1
// };
