#include <pebble.h>
  
#define FULL_SCREEN GRect(0, 0, 144, 168)
#define TEXT_COLOR GColorWhite
#define BG_COLOR GColorBlack
#define TIME_COLOR GColorRed

Window *my_window;
Layer *graph_layer;
TextLayer *hour_number_layer[13], *minute_number_layer[13];
uint8_t hours, minutes;
GFont s_time_font;
char hour_strings[13][3] = {" 0", "  ", " 2", "  ", " 4", "  ", " 6", "  ", " 8", "  ", "10", "  ", "12"};
char minute_strings[13][3] = {" 0", "  ", "10", "  ", "20", "  ", "30", "  ", "40", "  ", "50", "  ", "60"};

void updateGraph(struct Layer *layer, GContext *ctx);
void tick_handler(struct tm *tick_time, TimeUnits units_changed);

const char* get_string_for_hour(int i) {
  return hour_strings[i];
}

const char* get_string_for_minute(int i) {
  return minute_strings[i];
}

void handle_init(void) {
  my_window = window_create();
  graph_layer = layer_create(FULL_SCREEN);
  layer_set_update_proc(graph_layer, updateGraph);
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FORCED_SQUARE_10));
  
  for (int i = 0; i < 13; i++) {
    TextLayer *temp = hour_number_layer[i] = text_layer_create(GRect(0, 23 + 9 * i, 30, 30));
    layer_add_child(graph_layer, text_layer_get_layer(temp));
    text_layer_set_background_color(temp, GColorClear);
    text_layer_set_text_color(temp, TEXT_COLOR);
    text_layer_set_text(temp, "0");
    text_layer_set_font(temp, s_time_font);
    text_layer_set_text_alignment(temp, GTextAlignmentLeft);
  }
  
  for (int i = 0; i < 13; i++) {
    TextLayer *temp = minute_number_layer[i] = text_layer_create(GRect(15 - (i < 2 ? 1 : 0) + 9 * i, 9, 30, 30));
    layer_add_child(graph_layer, text_layer_get_layer(temp));
    text_layer_set_background_color(temp, GColorClear);
    text_layer_set_text_color(temp, TEXT_COLOR);
    text_layer_set_text(temp, "0");
    text_layer_set_font(temp, s_time_font);
    text_layer_set_text_alignment(temp, GTextAlignmentLeft);
  }
  
  window_stack_push(my_window, true);
  layer_add_child(window_get_root_layer(my_window), graph_layer);
  tick_timer_service_subscribe(MINUTE_UNIT | HOUR_UNIT, tick_handler);
  
  for (int i = 0; i < 13; i++)
    text_layer_set_text(hour_number_layer[i], get_string_for_hour(12 - i));
  for (int i = 0; i < 13; i++)
    text_layer_set_text(minute_number_layer[i], get_string_for_minute(i));
  
  time_t temp = time(0);
  struct tm *tick_time = localtime(&temp);
  tick_handler(tick_time, SECOND_UNIT);
}

void updateGraph(struct Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, BG_COLOR);
  graphics_fill_rect(ctx, FULL_SCREEN, 0, GCornerNone);
  
  float hourpos = 12 - (hours + minutes / 60.0f);
  float minpos = (minutes) / 5.0f;
  graphics_context_set_stroke_color(ctx, TIME_COLOR);
  graphics_draw_line(ctx, GPoint((int)(18 + 9 * minpos), 22), GPoint((int)(18 + 9 * minpos), 146));
  graphics_draw_line(ctx, GPoint(10, (int)(30 + 9 * hourpos)), GPoint(134, (int)(30 + 9 * hourpos)));
  graphics_draw_circle(ctx, GPoint((int)(18 + 9 * minpos), (int)(30 + 9 * hourpos)), 2);
  
  graphics_context_set_stroke_color(ctx, TEXT_COLOR);
  
  for (int i = 0; i < 13; i++)
    for (int j = 0; j < 13; j++)
    graphics_draw_pixel(ctx, GPoint(18 + 9 * i, 30 + 9 * j));
  
  graphics_draw_rect(ctx, GRect(9, 21, 126, 126));
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  hours = (tick_time->tm_hour) % 12;
  minutes = tick_time->tm_min;
  layer_mark_dirty(graph_layer);
}

void handle_deinit(void) {
  window_destroy(my_window);
  layer_destroy(graph_layer);
  for (int i = 0; i < 13; i++) {
    text_layer_destroy(hour_number_layer[i]);
    text_layer_destroy(minute_number_layer[i]);
  }
  fonts_unload_custom_font(s_time_font);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
