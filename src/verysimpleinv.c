#include "analog_hands.h"

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xA4, 0x1B, 0xB0, 0xE2, 0xD2, 0x62, 0x4E, 0xA6, 0xAA, 0x30, 0xED, 0xBE, 0x01, 0xE3, 0x8A, 0x02}
PBL_APP_INFO(MY_UUID, "VerySimple", "Pebble Technology & Oliver Bowe", 3, 1.1 /* App version */, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

TextLayer text_date_layer;
TextLayer text_time_layer;


Layer line_layer;


  GPath dow_arrow;
  Layer dow_layer;


void line_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorWhite);

    //graphics_draw_line(ctx, GPoint(8, 68), GPoint(131, 68));
    //graphics_draw_line(ctx, GPoint(8, 67), GPoint(131, 67));
    
  graphics_draw_line(ctx, GPoint(8, 97), GPoint(131, 97));
  graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));

}

Layer hour_display_layer, minute_display_layer, second_display_layer;
Layer center_display_layer,dow_layer;
GPath hour_hand_path, minute_hand_path;
GPath dow_arrow;

void initLayerPathAndCenter(Layer *layer, GPath *path, const GPathInfo *pathInfo, const void *updateProc) {
	layer_init(layer, GRect(0, 0, 70, 70));
	layer->update_proc = updateProc;
	layer_add_child(&window.layer, layer);
	gpath_init(path, pathInfo);
	gpath_move_to(path, grect_center_point(&layer->frame));
}

void center_display_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  GPoint center =  grect_center_point(&me->frame);
	
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 6);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 4);
}

void hour_display_layer_update_callback(Layer *me, GContext* ctx) {
	(void)me;
    
	PblTm t;
	get_time(&t);
    
	unsigned int angle = (t.tm_hour * 30) + (t.tm_min / 2);
	gpath_rotate_to(&hour_hand_path, (TRIG_MAX_ANGLE / 360) * angle);

	
	graphics_context_set_fill_color(ctx, GColorWhite);
	gpath_draw_filled(ctx, &hour_hand_path);
}

void minute_display_layer_update_callback(Layer *me, GContext* ctx) {
	(void)me;
    
	PblTm t;
	get_time(&t);
    
	unsigned int angle = (t.tm_min * 6) + (t.tm_sec / 10);
	gpath_rotate_to(&minute_hand_path, (TRIG_MAX_ANGLE / 360) * angle);
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	gpath_draw_filled(ctx, &minute_hand_path);
}


static void dow_update_proc(Layer *layer, GContext *ctx) {
  PblTm t;
   get_time(&t);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

	int days = (t.tm_wday ? t.tm_wday - 1 : 6) % 7;
	
  int x = (int)(20.5 * days);

	graphics_fill_rect(ctx, GRect(x, 65, 18, 5), 3, GCornersAll );

}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "VerySimple");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);


  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(8, 68, 144-8, 168-68));

  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROB_CONDENSED_21)));
  layer_add_child(&window.layer, &text_date_layer.layer);


  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(7, 92, 144-7, 168-92));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &text_time_layer.layer);

	
  layer_init(&line_layer, window.layer.frame);
  line_layer.update_proc = &line_layer_update_callback;
  layer_add_child(&window.layer, &line_layer);
	
    initLayerPathAndCenter(&hour_display_layer, &hour_hand_path, &HOUR_HAND_POINTS, &hour_display_layer_update_callback);
	initLayerPathAndCenter(&minute_display_layer, &minute_hand_path, &MINUTE_HAND_POINTS, &minute_display_layer_update_callback);
	
   dow_layer.update_proc = &dow_update_proc;
   layer_add_child(&window.layer, &dow_layer);

	hour_display_layer.frame.origin.x = (144/2) - (hour_display_layer.frame.size.w/2) ;
	minute_display_layer.frame.origin.x = (144/2) - (minute_display_layer.frame.size.w/2) ;
  layer_init(&center_display_layer, GRect(0, 0, 144, 70));

//		 center_display_layer.frame.origin.x = (144/2) - (center_display_layer.frame.size.w/2) ;
  center_display_layer.update_proc = &center_display_layer_update_callback;
  layer_add_child(&window.layer, &center_display_layer);
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)t;
  (void)ctx;

  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "00 Xxxxxxxxx";

  char *time_format;


  // TODO: Only update the date when it's changed.
  string_format_time(date_text, sizeof(date_text), "%e %B", t->tick_time);
  text_layer_set_text(&text_date_layer, date_text);


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(&text_time_layer, time_text);
	if(t->tick_time->tm_min%2==0) {
		layer_mark_dirty(&hour_display_layer);
	}
	layer_mark_dirty(&minute_display_layer);
	
	// Vibrate Every Five Minutes
  //#if HOUR_VIBRATION
         
    if ((t->tick_time->tm_min % 5 == 0 ) && (t->tick_time->tm_sec==0))
    {
        //vibes_enqueue_custom_pattern(hour_pattern);
		vibes_double_pulse();	
    }

  //#endif
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
