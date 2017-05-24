#include <pebble.h>
// #include <biocomPebbleDefinitions.h>

// #include <unistd.h>

// Originating from the hello world-concept to illustrate and find conceptual limits with the pebble equipment.
// 
// JJ Wikner 
// BioCom Lab
//

static Window *s_window;
static TextLayer *s_text_layer;

static void deinit(void) {
	// Destroy the text layer
	text_layer_destroy(s_text_layer);	
	// Destroy the window
	window_destroy(s_window);
}

// Vibe pattern: ON for 200ms, OFF for 100ms, ON for 400ms:
static const uint32_t const segments[] = { 200, 100, 400 };
VibePattern pat = {
  .durations = segments,
  .num_segments = ARRAY_LENGTH(segments),
};


static void biocomSelectSingleClickHandler(ClickRecognizerRef recognizer, void *context) {
	// Detect that a single click has just occured. 
	// Notice that the binders is independent of the button clicked.
	deinit();
}

static void biocomUpSingleClickHandler(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	// In this case, create a buzzing sound to illustrate this
	vibes_long_pulse();
}

static void biocomDownSingleClickHandler(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	// In this case, create a buzzing sound to illustrate this
	// vibes_long_pulse();
	vibes_enqueue_custom_pattern(pat);
}

static void biocomClickConfigProvider(void *context) { // Rename biocom	
	// Subscribe to button click events here	
	// They are contextually bound to the home screen
	
	ButtonId bidSelect = BUTTON_ID_SELECT; // The select button
	window_single_click_subscribe(bidSelect, biocomSelectSingleClickHandler); // The trigger event
	
	ButtonId bidUp = BUTTON_ID_UP;
	window_single_click_subscribe(bidUp, biocomUpSingleClickHandler);

	ButtonId bidDn = BUTTON_ID_DOWN;
	window_single_click_subscribe(bidDn, biocomDownSingleClickHandler);
}

void biocomPutHomeString(char *windowString) {
	// Create a window and get information about the window
	s_window = window_create();
	window_set_click_config_provider(s_window, biocomClickConfigProvider);
	
	
	Layer *window_layer = window_get_root_layer(s_window);  
	GRect bounds = layer_get_bounds(window_layer);
	
  // Create a text layer and set the text
	s_text_layer = text_layer_create(bounds);
	text_layer_set_text(s_text_layer, windowString);
  
  // Set the font and text alignment
	text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

	// Add the text layer to the window
	layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_layer));
  
  	// Enable text flow and paging on the text layer, with a slight inset of 10, for round screens
  	text_layer_enable_screen_text_flow_and_paging(s_text_layer, 10);

	// Push the window, setting the window animation to 'true'
	window_stack_push(s_window, true);
	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  	// A tap event occured
	biocomPutHomeString("Shaky!");
	psleep(1);
	biocomPutHomeString("BioCom Lab - Wristband communicator");
	
}


static void init(void) {
	biocomPutHomeString("BioCom Lab - Wristband communicator");
	// psleep(500);
	accel_tap_service_subscribe(accel_tap_handler);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
