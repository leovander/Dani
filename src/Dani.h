#define TM_SIZE 15
#define MIN_LEFT 130
#define MIN_BOTTOM 153

#define TH_SIZE_X 35
#define TH_SIZE_Y 35
#define INNER_LEFT 27
#define INNER_RIGHT 114
#define INNER_TOP 9
#define INNER_BOTTOM 119

static Window *s_main_window;
static TextLayer *text_layers[2];
static GRect hour_pos[13];
static PropertyAnimation *s_property_animation_1;
static PropertyAnimation *s_property_animation_2;
static bool startup;

static void update_time(struct tm *tick_time);
static void main_window_load(Window *window);
static void main_window_unload();
static void fill_hour_frames();
static void updateTimeLayers();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void trigger_custom_animation(struct tm *tick_time);
static void init();
static void deinit();
