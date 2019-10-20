#include <pebble.h>

Window *s_window;
GBitmap *s_res_background_image;
BitmapLayer *s_bitmaplayer_1;
Layer *s_layer_1, *background_layer;

TextLayer *seconds_layer;

GPath *level1, *level2, *level3, *level4, *level5, *level6;

GFont *metalGear9;

TextLayer *time_layer, *bluetooth_layer, *charge_layer;

AppTimer *info_timer;

bool connected;
bool charging;
int battery;

int seconds;
int disp_seconds;

const uint32_t  charge_vibe_segments[] = {200, 200, 400};
VibePattern charge_vibe = {
  .durations = charge_vibe_segments,
  .num_segments = ARRAY_LENGTH(charge_vibe_segments),
};

GPathInfo LEVEL_1_POINTS = {
  4,
  (GPoint[]) {
    {15,130},
    {15,144},
    {40,144},
    {40,130},
  }
};

GPathInfo LEVEL_2_POINTS = {
  4,
  (GPoint[]) {
    {15,110},
    {15,124},
    {40,124},
    {42,110},
  }
};

GPathInfo LEVEL_3_POINTS = {
  5,
  (GPoint[]) {
    {15,90},
    {15,104},
    {43,104},
    {44, 97},
    {46,90},
  }
};

GPathInfo LEVEL_4_POINTS = {
  5,
  (GPoint[]) {
    {15,70},
    {15,84},
    {47,84},
    {50,77},
    {55,70},
  }
};

GPathInfo LEVEL_5_POINTS = {
  6,
  (GPoint[]) {
    {15,50},
    {15,64},
    {58,64},
    {63,59},
    {72,54},
    {88,50},
  }
};

GPathInfo LEVEL_6_POINTS = {
  4,
  (GPoint[]) {
    {15,29},
    {15,43},
    {134,43},
    {134,29},
  }
};

void background_update(Layer *layer, GContext *ctx) {
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorMidnightGreen);
    graphics_fill_rect(ctx, GRect(5,130,5,15), 0, GCornersAll);    
    graphics_fill_rect(ctx, GRect(5,110,5,15), 0, GCornersAll);    
    graphics_fill_rect(ctx, GRect(5,90,5,15), 0, GCornersAll);    
    graphics_fill_rect(ctx, GRect(5,70,5,15), 0, GCornersAll);    
    graphics_fill_rect(ctx, GRect(5,50,5,15), 0, GCornersAll);    
    graphics_fill_rect(ctx, GRect(5,29,5,15), 0, GCornersAll);  
    gpath_draw_filled(ctx, level1);
    gpath_draw_filled(ctx, level2);
    gpath_draw_filled(ctx, level3);
    gpath_draw_filled(ctx, level4);
    gpath_draw_filled(ctx, level5);
    gpath_draw_filled(ctx, level6);
  #else
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_rect(ctx, GRect(5,130,5,15));    
    graphics_draw_rect(ctx, GRect(5,110,5,15));    
    graphics_draw_rect(ctx, GRect(5,90,5,15));    
    graphics_draw_rect(ctx, GRect(5,70,5,15));    
    graphics_draw_rect(ctx, GRect(5,50,5,15));    
    graphics_draw_rect(ctx, GRect(5,29,5,15));  
    gpath_draw_outline(ctx, level1);
    gpath_draw_outline(ctx, level2);
    gpath_draw_outline(ctx, level3);
    gpath_draw_outline(ctx, level4);
    gpath_draw_outline(ctx, level5);
    gpath_draw_outline(ctx, level6);
  #endif
}

void levels_update(Layer *layer, GContext *ctx) {
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorElectricBlue);
    graphics_context_set_antialiased(ctx, true);
  #else
      graphics_context_set_fill_color(ctx, GColorWhite);
  #endif
  if(charging) {
    if(battery >= 10) {
      graphics_fill_rect(ctx, GRect(5,130,5,15), 0, GCornersAll);  
      graphics_fill_rect(ctx, GRect(16,130,24,15), 0, GCornersAll);
    }
    if(battery >= 30) {
      graphics_fill_rect(ctx, GRect(5,110,5,15), 0, GCornersAll);    
      gpath_draw_filled(ctx, level2);
    }
    if(battery >= 50) {
      graphics_fill_rect(ctx, GRect(5,90,5,15), 0, GCornersAll);  
      gpath_draw_filled(ctx, level3);
    }
    if(battery >= 70) {
      graphics_fill_rect(ctx, GRect(5,70,5,15), 0, GCornersAll);    
      gpath_draw_filled(ctx, level4);
    }
    if(battery >= 90) {
      graphics_fill_rect(ctx, GRect(5,50,5,15), 0, GCornersAll);  
      gpath_draw_filled(ctx, level5);
    }
    if(battery == 100) {
      graphics_fill_rect(ctx, GRect(5,29,5,15), 0, GCornersAll);  
      graphics_fill_rect(ctx, GRect(16,29,119,15), 0, GCornersAll);
      #ifdef PBL_COLOR
        graphics_context_set_text_color(ctx, GColorElectricBlue);
      #else
        graphics_context_set_text_color(ctx, GColorWhite);
      #endif
    } else {
      #ifdef PBL_COLOR
        graphics_context_set_text_color(ctx, GColorMidnightGreen);
      #else
        graphics_context_set_text_color(ctx, GColorBlack);
      #endif
    }
  } else {
    graphics_fill_rect(ctx, GRect(5,130,5,15), 0, GCornersAll);  
    graphics_fill_rect(ctx, GRect(16,130,24,15), 0, GCornersAll);
    if(seconds >= 10) {
       graphics_fill_rect(ctx, GRect(5,110,5,15), 0, GCornersAll);    
      gpath_draw_filled(ctx, level2);
    }
   if(seconds >= 20) { 
      graphics_fill_rect(ctx, GRect(5,90,5,15), 0, GCornersAll);  
      gpath_draw_filled(ctx, level3);
    }
    if(seconds >= 30) {
      graphics_fill_rect(ctx, GRect(5,70,5,15), 0, GCornersAll);    
      gpath_draw_filled(ctx, level4);
    }
    if(seconds >= 40) {  
      graphics_fill_rect(ctx, GRect(5,50,5,15), 0, GCornersAll);  
      gpath_draw_filled(ctx, level5);
    }
    if (seconds >= 50) {
      graphics_fill_rect(ctx, GRect(5,29,5,15), 0, GCornersAll);  
      graphics_fill_rect(ctx, GRect(16,29,119,15), 0, GCornersAll);
      #ifdef PBL_COLOR
        graphics_context_set_text_color(ctx, GColorElectricBlue);
      #else
        graphics_context_set_text_color(ctx, GColorWhite);
      #endif
    } else {
      #ifdef PBL_COLOR
        graphics_context_set_text_color(ctx, GColorMidnightGreen);
      #else
        graphics_context_set_text_color(ctx, GColorBlack);
      #endif
    }
  }
   graphics_draw_text(ctx, "MaX",  metalGear9, GRect(114,43,30,30), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

void set_time(struct tm *t) {
 static char time_text[] = "00.00";
  static char seconds_text[] = "000%";
  seconds = t->tm_sec;
  if(seconds == 10 || seconds == 20 || seconds == 30 || seconds == 40 || seconds == 50 || seconds == 60) {
      layer_mark_dirty(s_layer_1);
  }
  if(clock_is_24h_style() == true) {
    strftime(time_text, sizeof(time_text), "%H.%M", t);
  } else {
    strftime(time_text, sizeof(time_text), "%I.%M", t);
  }
  disp_seconds = seconds;
  while(disp_seconds >= 0) {
    disp_seconds -= 10;
  }
  disp_seconds += 10;
  if(charging) {
    snprintf(seconds_text, sizeof(seconds_text), "%i", battery);
  } else {
    snprintf(seconds_text, sizeof(seconds_text), "%i", disp_seconds);
  }
  text_layer_set_text(time_layer, time_text);
  text_layer_set_text(seconds_layer, seconds_text);
}

void tick_handler(struct tm *t, TimeUnits units_changed) {
  set_time(t);
}

void handle_battery(BatteryChargeState charge_state) {
  if(charge_state.is_plugged) {
    battery = charge_state.charge_percent;
    charging = true;
    #ifdef PBL_COLOR
      layer_set_hidden((Layer*)charge_layer, false);
      if(charge_state.charge_percent == 100) {
         text_layer_set_background_color(charge_layer, GColorGreen);
      } else {
         text_layer_set_background_color(charge_layer, GColorChromeYellow);
      }
    #else
      layer_set_hidden((Layer*)charge_layer, false);
    #endif
  } else {
    charging = false;
    #ifdef PBL_COLOR
      text_layer_set_background_color(charge_layer, GColorRed);
    #endif
    if(charge_state.charge_percent <= 20) {
      if(charge_state.charge_percent == 20) {
      vibes_enqueue_custom_pattern(charge_vibe);
      }
      layer_set_hidden((Layer*)charge_layer, false);
    } else {
      layer_set_hidden((Layer*)charge_layer, true);
    }
  }
}

void handle_bluetooth(bool connection) {
  if(connection) {
    #ifdef PBL_COLOR
      text_layer_set_text_color(bluetooth_layer, GColorElectricBlue);
    #else
      text_layer_set_text_color(bluetooth_layer, GColorWhite);
    #endif
  } else {
    vibes_short_pulse();
    #ifdef PBL_COLOR
      text_layer_set_text_color(bluetooth_layer, GColorMidnightGreen);
    #else
      text_layer_set_text_color(bluetooth_layer, GColorBlack);
    #endif
  }
}

void initialise_ui() {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
  s_res_background_image = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_IMAGE);
  metalGear9 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_METAL_GEAR_9));
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_background_image);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  
  //background_layer
  background_layer = layer_create(GRect(0,0,144,168));
  layer_set_update_proc(background_layer, background_update);
  layer_add_child(window_get_root_layer(s_window), (Layer*)background_layer);
  
  // s_layer_1
  s_layer_1 = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_layer_1, levels_update);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_layer_1);
  
  time_layer = text_layer_create(GRect(40,90,100,40));
  text_layer_set_text(time_layer, "141.12");
  #ifdef PBL_COLOR
    text_layer_set_text_color(time_layer, GColorElectricBlue);
  #else
    text_layer_set_text_color(time_layer, GColorWhite);
  #endif
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_alignment(time_layer, GTextAlignmentRight);
  text_layer_set_font(time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_METAL_GEAR_17)));
  layer_add_child(window_get_root_layer(s_window), (Layer*)time_layer);
  
  charge_layer = text_layer_create(GRect(52,112,88,21));
  text_layer_set_text(charge_layer, "CHRG");
  #ifdef PBL_COLOR
    text_layer_set_background_color(charge_layer, GColorRed);
    text_layer_set_text_color(charge_layer, GColorBlack);
  #else
    text_layer_set_background_color(charge_layer, GColorWhite);
    text_layer_set_text_color(charge_layer, GColorBlack);
  #endif
  text_layer_set_text_alignment(charge_layer, GTextAlignmentCenter);
  text_layer_set_font(charge_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_METAL_GEAR_16)));
  layer_add_child(window_get_root_layer(s_window), (Layer*)charge_layer);
  layer_set_hidden((Layer*)charge_layer, true);
  
  bluetooth_layer = text_layer_create(GRect(16,10,100,40));
  text_layer_set_text(bluetooth_layer, "PTT");
  #ifdef PBL_COLOR
    text_layer_set_text_color(bluetooth_layer, GColorElectricBlue);
  #else
    text_layer_set_text_color(bluetooth_layer, GColorWhite);
  #endif
  text_layer_set_background_color(bluetooth_layer, GColorClear);
  text_layer_set_font(bluetooth_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_METAL_GEAR_13)));
  layer_add_child(window_get_root_layer(s_window), (Layer*)bluetooth_layer);
  
  seconds_layer = text_layer_create(GRect(44, 136, 120, 30));
  text_layer_set_text(seconds_layer, "0");
  text_layer_set_font(seconds_layer, metalGear9);
  #ifdef PBL_COLOR
    text_layer_set_text_color(seconds_layer, GColorElectricBlue);
  #else
    text_layer_set_text_color(seconds_layer, GColorWhite);
  #endif
  text_layer_set_background_color(seconds_layer, GColorClear);
  layer_add_child(window_get_root_layer(s_window), (Layer*)seconds_layer);
    
  level1 = gpath_create(&LEVEL_1_POINTS);
  level2 = gpath_create(&LEVEL_2_POINTS);
  level3 = gpath_create(&LEVEL_3_POINTS);
  level4 = gpath_create(&LEVEL_4_POINTS);
  level5 = gpath_create(&LEVEL_5_POINTS);
  level6 = gpath_create(&LEVEL_6_POINTS);
  
  handle_battery(battery_state_service_peek());
  handle_bluetooth(bluetooth_connection_service_peek());
  
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  time_t temp = time(NULL);
  struct tm *t = localtime(&temp);
  set_time(t);
}

void destroy_ui() {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer_1);
  layer_destroy(s_layer_1);
  bluetooth_connection_service_unsubscribe();
  gbitmap_destroy(s_res_background_image);
}
// END AUTO-GENERATED UI CODE

void handle_window_unload(Window* window) {
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  destroy_ui();
}

void show_main_window() {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  battery_state_service_subscribe(&handle_battery);
  window_stack_push(s_window, true);
}

void hide_main_window() {
  window_stack_remove(s_window, true);
}

int main() {
  show_main_window();
  app_event_loop();
  hide_main_window();
}
