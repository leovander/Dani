#define TM_SIZE 15
#define MIN_LEFT 132
#define MIN_BOTTOM 153

#define TH_SIZE_X 35
#define TH_SIZE_Y 35
#define INNER_LEFT 15
#define INNER_RIGHT 114
#define INNER_TOP 5
#define INNER_BOTTOM 120

static Window *s_main_window;
static TextLayer *text_layers[2];
static PropertyAnimation *s_property_animation;

static void update_time();
static void main_window_load(Window *window);
static void main_window_unload(Window *window);
static void set_postions();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void trigger_custom_animation(struct tm *tick_time);
static void init();
static void deinit();
