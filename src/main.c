/*
----------------------------
VERBOSE WATCHFACE FOR PEBBLE
----------------------------
AUTHOR: Marcello Brivio
WEBSITE: http://www.marcellobrivio.com
*/

#include <pebble.h>

// These are used for generating the weather report
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

// Define UI elements
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_uptime_layer;
static TextLayer *s_bluetooth_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_weather_layer;

// Define Watchface Uptime
static int s_uptime = 0;

// TIME AND DATE //
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[200];

  // Write the current hours and minutes into the buffer with funny copy...
  // Refer to http://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
  strftime(buffer, sizeof buffer, "MILITARY TIME: %H:%M%nCIVILIAN TIME: %I:%M %p%nSECONDS: %S/59%nDATE: %F%nWEEKDAY: %A (%u/7)%nWEEK NUMBER: %W/53%nDAY NUMBER: %j/365", tick_time);
    
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

// BLUETOOTH CONNECTION CHECKER
static void bt_handler(bool connected) {
  // Show current connection state
  if (connected) {
    text_layer_set_text(s_bluetooth_layer, "CONNECTED (Y/N): Y");
  } else {
    text_layer_set_text(s_bluetooth_layer, "CONNECTED (Y/N): N");
  }
}

// BATTERY STATUS
static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "BATTERY LEVEL: %d%%", new_state.charge_percent);
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

// WINDOW CREATION //
static void main_window_load(Window *window) {
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorClear);
  text_layer_set_text(s_time_layer, "TIME: 00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  
  // Create uptime TextLayer
  s_uptime_layer = text_layer_create(GRect(0, 98, 144, 14));
  text_layer_set_background_color(s_uptime_layer, GColorBlack);
  text_layer_set_text_color(s_uptime_layer, GColorClear);
  text_layer_set_text(s_uptime_layer, "LOG UPTIME: 0h 0m 0s");
  text_layer_set_font(s_uptime_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_uptime_layer, GTextAlignmentLeft);
  
  // Create Bluetooth TextLayer
  s_bluetooth_layer = text_layer_create(GRect(0, 112, 144, 14));
  text_layer_set_background_color(s_bluetooth_layer, GColorBlack);
  text_layer_set_text_color(s_bluetooth_layer, GColorClear);
  text_layer_set_text(s_bluetooth_layer, "CONNECTED (Y/N): N");
  text_layer_set_font(s_bluetooth_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentLeft);
  
  // Create Battery TextLayer
  s_battery_layer = text_layer_create(GRect(0, 126, 144, 14));
  text_layer_set_background_color(s_battery_layer, GColorBlack);
  text_layer_set_text_color(s_battery_layer, GColorClear);
  text_layer_set_text(s_battery_layer, "BATTERY LEVEL: N/A");
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentLeft);
  
  // Create Battery TextLayer
  s_weather_layer = text_layer_create(GRect(0, 140, 144, 28));
  text_layer_set_background_color(s_weather_layer, GColorBlack);
  text_layer_set_text_color(s_weather_layer, GColorClear);
  text_layer_set_text(s_weather_layer, "TEMPERATURE: Loading...\nWEATHER: Loading...");
  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentLeft);
  
  // Make sure the time is displayed from the start
  update_time();
  
  // Show current connection state
  bt_handler(bluetooth_connection_service_peek());
  
  // Get the current battery level
  battery_handler(battery_state_service_peek());

  // Add child layers to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_uptime_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bluetooth_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
}

// WINDOW DESTRUCTION //
static void main_window_unload(Window *window) {
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_uptime_layer);
  text_layer_destroy(s_bluetooth_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_weather_layer);
}

// TIME-RECURSIVE FUNCTIONS
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // UPDATED THE TIME AND DATE
  update_time();
  
  // CALCULATES PEBBLE UPTIME
  // Use a long-lived buffer
  static char s_uptime_buffer[32];

  // Get time since launch
  int seconds = s_uptime % 60;
  int minutes = (s_uptime % 3600) / 60;
  int hours = s_uptime / 3600;

  // Update the TextLayer
  snprintf(s_uptime_buffer, sizeof(s_uptime_buffer), "LOG UPTIME: %dh %dm %ds", hours, minutes, seconds);
  text_layer_set_text(s_uptime_layer, s_uptime_buffer);
  
  // Increment s_uptime
  s_uptime++;
  
  // Get weather updates every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

// WEATHER SERVICE CALLBACKS
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
      break;
    case KEY_CONDITIONS:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "TEMPERATURE: %s\nWEATHER: %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {
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
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  // Register to Bluetooth updates
  bluetooth_connection_service_subscribe(bt_handler);
  
  // Register to the Battery State Service
  battery_state_service_subscribe(battery_handler);
  
  // Register weather service callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
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