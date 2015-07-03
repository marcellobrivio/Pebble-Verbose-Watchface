/*
----------------------------
VERBOSE WATCHFACE FOR PEBBLE
----------------------------
AUTHOR: Marcello Brivio
WEBSITE: http://www.marcellobrivio.com
*/

#include <pebble.h>
    
static Window *s_main_window;
static TextLayer *s_time_layer;
static int s_uptime = 0;
static TextLayer *s_uptime_layer;
static TextLayer *s_bluetooth_layer;

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
  s_uptime_layer = text_layer_create(GRect(0, 98, 144, 20));
  text_layer_set_background_color(s_uptime_layer, GColorBlack);
  text_layer_set_text_color(s_uptime_layer, GColorClear);
  text_layer_set_text(s_uptime_layer, "LOG UPTIME: 0h 0m 0s");
  text_layer_set_font(s_uptime_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_uptime_layer, GTextAlignmentLeft);
  
  // Create Bluetooth TextLayer
  s_bluetooth_layer = text_layer_create(GRect(0, 112, 144, 20));
  text_layer_set_background_color(s_bluetooth_layer, GColorBlack);
  text_layer_set_text_color(s_bluetooth_layer, GColorClear);
  text_layer_set_text(s_bluetooth_layer, "CONNECTED (Y/N): N");
  text_layer_set_font(s_bluetooth_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentLeft);
  
  // Show current connection state
  bt_handler(bluetooth_connection_service_peek());

  // Add child layers to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_uptime_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bluetooth_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

// WINDOW DESTRUCTION //
static void main_window_unload(Window *window) {
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_uptime_layer);
  text_layer_destroy(s_bluetooth_layer);
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
  
  // Subscribe to Bluetooth updates
  bluetooth_connection_service_subscribe(bt_handler);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
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