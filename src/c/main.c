#include <pebble.h>
#include <stdlib.h>

static void update_display();

/*********************&*********/
/* Settings / storage handlers */
/*******************************/

// Persistent storage / configuration
#define MY_NAME_KEY 5
#define THEIR_NAME_KEY 6
#define THEIR_STEPS_KEY 7

// Define the settings struct
typedef struct Settings {
  char* myName;
  char* theirName;
  char* theirSteps;
} Settings;
// Create an instance of the struct
static Settings settings;
// Initialize the default settings
static void default_settings() {
  settings.myName = "myNameXXXX";
  settings.theirName = "theirNameX";
  settings.theirSteps = "000000";
}
// Read settings from persistent storage
static void load_settings() {
  APP_LOG(APP_LOG_LEVEL_INFO, "LOADING SETTINGS");
  // Load the default settings
  default_settings();
  // Read settings from persistent storage, if they exist
  // Read the string
  char myNameBuffer[32];
  persist_read_string(MY_NAME_KEY, myNameBuffer, sizeof(myNameBuffer));
 
  char theirNameBuffer[32];
  persist_read_string(THEIR_NAME_KEY, theirNameBuffer, sizeof(theirNameBuffer));
  
  char theirStepsBuffer[32];
  persist_read_string(THEIR_STEPS_KEY, theirStepsBuffer, sizeof(theirStepsBuffer));
  
  strcpy(settings.myName, myNameBuffer);
  strcpy(settings.theirName, theirNameBuffer);
  strcpy(settings.theirSteps, theirStepsBuffer);
  
  APP_LOG(APP_LOG_LEVEL_INFO, settings.myName);
  APP_LOG(APP_LOG_LEVEL_INFO, settings.theirName);
  APP_LOG(APP_LOG_LEVEL_INFO, settings.theirSteps);


}
// Save the settings to persistent storage
static void save_settings() {
  APP_LOG(APP_LOG_LEVEL_INFO, "SAVING SETTINGS");
  APP_LOG(APP_LOG_LEVEL_INFO, settings.myName);
  APP_LOG(APP_LOG_LEVEL_INFO, settings.theirName);
  APP_LOG(APP_LOG_LEVEL_INFO, settings.theirSteps);

  persist_write_string(MY_NAME_KEY, settings.myName);
  persist_write_string(THEIR_NAME_KEY, settings.theirName);
  persist_write_string(THEIR_STEPS_KEY, settings.theirSteps);  // Update the display based on new settings
  
  update_display();
}

static HealthValue getSteps() {
  // get steps
  HealthMetric metric = HealthMetricStepCount;
  time_t start = time_start_of_today();
  time_t end = time(NULL);
    
  // Check the metric has data available for today
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, 
      start, end);
  HealthValue steps = 0;
  if(mask & HealthServiceAccessibilityMaskAvailable) {
    // Data is available!
    steps = health_service_sum_today(metric);
    APP_LOG(APP_LOG_LEVEL_INFO, "Steps today: %d", 
              (int)health_service_sum_today(metric));
  } else {
    // No data recorded yet today
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
  }
  return steps;
}

void will_focus_handler(bool will_focus) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Will %s focus", will_focus ? "gain" : "lose");
  if (!will_focus) {
    save_settings();
  }
}

void did_focus_handler(bool did_focus) {
  APP_LOG(APP_LOG_LEVEL_INFO, "%s focus", did_focus ? "Gained" : "Lost");
  if (did_focus) {
    load_settings();
  }
}

/**************************/
/* Window / draw handling */
/**************************/

static Window *s_main_window;

static TextLayer *s_time_layer;

static TextLayer *s_time_hour_layer;
static TextLayer *s_time_minute_layer;
static TextLayer *s_time_month_layer;

static TextLayer *s_my_name_layer;
static TextLayer *s_their_name_layer;

static Layer *s_canvas_layer;

static GDrawCommandImage *s_weather_image;
static Layer *weather_canvas_layer;
static TextLayer *weather_text_layer;

static void weather_update_proc(Layer *layer, GContext *ctx) {
  // Set the origin offset from the context for drawing the image
  GRect bounds = layer_get_bounds(layer);

  GPoint origin = GPoint(bounds.size.w * .11, bounds.size.h*.1);

  // Draw the GDrawCommandImage to the GContext
  gdraw_command_image_draw(ctx, s_weather_image, origin);
}

static void update_display() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // get hours
  static char s_h_buffer[4];
  strftime(s_h_buffer, sizeof(s_h_buffer), clock_is_24h_style() ? "%H" : "%I", tick_time);
  // get minutes
  static char s_m_buffer[4];
  strftime(s_m_buffer, sizeof(s_m_buffer), "%M", tick_time);
  // get abr month / zero padded day
  static char s_d_buffer[12];
  strftime(s_d_buffer, sizeof(s_d_buffer), "%b %d", tick_time);
  
  
  text_layer_set_text(s_time_hour_layer, s_h_buffer);
  text_layer_set_text(s_time_minute_layer, s_m_buffer);
  text_layer_set_text(s_time_month_layer, s_d_buffer);
  
  static char my_name[30];
  strcpy(my_name, settings.myName);
  static char their_name[30];
  strcpy(their_name, settings.theirName);
  text_layer_set_text(s_my_name_layer, my_name);
  text_layer_set_text(s_their_name_layer, their_name);
  
  s_weather_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_SUNNY_DAY);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Custom drawing happens here!
  GRect bounds = layer_get_bounds(layer);
  
  int corner_radius = 10;
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorClear);
  graphics_context_set_antialiased(ctx, false);

  float i = (float) getSteps();
  GRect my_steps_bounds = GRect(0, bounds.size.h*.68, bounds.size.w*(i/10000.0), bounds.size.h*.15);
  // Draw a rectangle
  graphics_draw_rect(ctx, my_steps_bounds);
  // Fill a rectangle with rounded corners
  graphics_fill_rect(ctx, my_steps_bounds, corner_radius, GCornerNone);
  
  i = (float) atoi(settings.theirSteps);
  GRect their_steps_bounds = GRect(0, bounds.size.h*.84, bounds.size.w*(i/10000.0), bounds.size.h*.15);
  // Draw a rectangle
  graphics_draw_rect(ctx, their_steps_bounds);
  // Fill a rectangle with rounded corners
  graphics_fill_rect(ctx, their_steps_bounds, corner_radius, GCornerNone);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  
  // Create canvas
  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  layer_mark_dirty(s_canvas_layer);

  // Create the TextLayer with specific bounds
  s_time_hour_layer = text_layer_create(
      GRect(bounds.size.w * .58, 0, bounds.size.w*.4, bounds.size.h*.23));
  s_time_minute_layer = text_layer_create(
      GRect(bounds.size.w * .58, bounds.size.h*.23, bounds.size.w*.4, bounds.size.h*.30));
  s_time_month_layer = text_layer_create(
      GRect(bounds.size.w * .58, bounds.size.h*.52, bounds.size.w*.4, bounds.size.h*.15));
  
  s_my_name_layer = text_layer_create(
      GRect(bounds.size.w * .02, bounds.size.h*.68, bounds.size.w*.8, bounds.size.h*.15));
  s_their_name_layer = text_layer_create(
      GRect(bounds.size.w * .02, bounds.size.h*.84, bounds.size.w*.8, bounds.size.h*.15));
  
  // hour layer
  text_layer_set_background_color(s_time_hour_layer, GColorBlack);
  text_layer_set_text_color(s_time_hour_layer, GColorWhite);
  text_layer_set_text(s_time_hour_layer, "00");
  text_layer_set_font(s_time_hour_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_time_hour_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_hour_layer));
  // minute layer
  text_layer_set_background_color(s_time_minute_layer, GColorBlack);
  text_layer_set_text_color(s_time_minute_layer, GColorWhite);
  text_layer_set_text(s_time_minute_layer, "00");
  text_layer_set_font(s_time_minute_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_minute_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_minute_layer));
  // month layer
  text_layer_set_background_color(s_time_month_layer, GColorBlack);
  text_layer_set_text_color(s_time_month_layer, GColorWhite);
  text_layer_set_text(s_time_month_layer, "Jan 01");
  text_layer_set_font(s_time_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_time_month_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_month_layer));
  
  // myName layer
  text_layer_set_text_color(s_my_name_layer, GColorLightGray);
  text_layer_set_background_color(s_my_name_layer, GColorClear);

  text_layer_set_text(s_my_name_layer, settings.myName);
  text_layer_set_font(s_my_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_my_name_layer, GTextAlignmentLeft);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_my_name_layer));
   
  // theirName layer
  text_layer_set_text_color(s_their_name_layer, GColorLightGray);
  text_layer_set_background_color(s_their_name_layer, GColorClear);

  text_layer_set_text(s_their_name_layer, settings.theirName);
  text_layer_set_font(s_their_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_their_name_layer, GTextAlignmentLeft);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_their_name_layer)); 
  
  
  // Create the canvas Layer
  weather_canvas_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  // Set the LayerUpdateProc
  layer_set_update_proc(weather_canvas_layer, weather_update_proc);
  // Add to parent Window
  layer_add_child(window_layer, weather_canvas_layer);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_time_minute_layer);
  text_layer_destroy(s_time_hour_layer);
  text_layer_destroy(s_time_month_layer);
  text_layer_destroy(s_my_name_layer);
  text_layer_destroy(s_their_name_layer);
  // Destroy the canvas Layer
  layer_destroy(s_canvas_layer);
  layer_destroy(weather_canvas_layer);

  // Destroy the PDC image
  gdraw_command_image_destroy(s_weather_image);
}

/********************/
/* Service Handlers */
/********************/
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_display();
}


static void send_health_update(char* event, HealthValue steps) {

  // Iterator variable, keeps the state of the creation serialization process:
  DictionaryIterator *iter;
  AppMessageResult result = app_message_outbox_begin(&iter);
  if(result == APP_MSG_OK) {
    // Construct the message
   APP_LOG(APP_LOG_LEVEL_INFO, "Constructing message.");
      // Write the Data:
    dict_write_uint32(iter, MESSAGE_KEY_mySteps, (uint32_t) steps);
    // Write the CString:
    dict_write_cstring(iter, MESSAGE_KEY_myStatus, event);
    // Send this message
    result = app_message_outbox_send();
    
    // Check the result
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

static void update_health_data() {
  // get current activity
  char event[12];
  HealthActivityMask activities = health_service_peek_current_activities();
  
  // Determine which bits are set, and hence which activity is active
  if (activities & HealthActivityNone) {
    APP_LOG(APP_LOG_LEVEL_INFO, "The user is doing nothing peacefully.");
    strcpy(event, "Nothing");
  } else if(activities & HealthActivitySleep) {
    APP_LOG(APP_LOG_LEVEL_INFO, "The user is sleeping peacefully.");
    strcpy(event, "Sleeping");
  } else if(activities & HealthActivityRestfulSleep) {
    APP_LOG(APP_LOG_LEVEL_INFO, "The user is snoozing peacefully.");
    strcpy(event, "Snoozing");
  } else if(activities & HealthActivityWalk) {
    APP_LOG(APP_LOG_LEVEL_INFO, "The user is walking peacefully.");
    strcpy(event, "Running");
  } else if(activities & HealthActivityRun) {
    APP_LOG(APP_LOG_LEVEL_INFO, "The user is running peacefully.");
    strcpy(event, "Running");
  } else if(activities & HealthActivityOpenWorkout) {
    APP_LOG(APP_LOG_LEVEL_INFO, "The user is working out peacefully.");
    strcpy(event, "Gyming");
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "I dunno what the user is doing.");
    strcpy(event, "Unknown");
  } 
  
  HealthValue steps = getSteps();
  send_health_update(event, steps);
}

static void health_handler(HealthEventType event, void *context) {
  update_health_data();
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  // settings updates
  int needs_update = 0;
  
  Tuple *myName_t = dict_find(iter, MESSAGE_KEY_myName);
  if(myName_t) {  
    strcpy(settings.myName, myName_t->value->cstring); 
    APP_LOG(APP_LOG_LEVEL_INFO, "MY NAME");
    APP_LOG(APP_LOG_LEVEL_INFO, settings.myName);
    needs_update = 1;
  }
  
  Tuple *theirName_t = dict_find(iter, MESSAGE_KEY_theirName);
  if(theirName_t) {
    strcpy(settings.theirName, theirName_t->value->cstring); 
    APP_LOG(APP_LOG_LEVEL_INFO, "THEIR NAME");
    APP_LOG(APP_LOG_LEVEL_INFO, settings.theirName);
    needs_update = 1;
  }
  
  Tuple *theirSteps_t = dict_find(iter, MESSAGE_KEY_theirSteps);
  if(theirSteps_t) {
    strcpy(settings.theirSteps, theirSteps_t->value->cstring); 
    APP_LOG(APP_LOG_LEVEL_INFO, "THEIR STEPS");
    APP_LOG(APP_LOG_LEVEL_INFO, settings.theirSteps);
    needs_update = 1;
  }
  if (needs_update) {
    save_settings();
    layer_mark_dirty(s_canvas_layer);
  }
  
  // other data
}

static void init() {
  load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 128);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register with HealthService
  health_service_events_subscribe(health_handler, NULL);
  
  // Subscribe to both types of events
  app_focus_service_subscribe_handlers((AppFocusHandlers) {
    .will_focus = will_focus_handler,
    .did_focus = did_focus_handler
  });
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}