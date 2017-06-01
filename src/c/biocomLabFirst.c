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
// static GRect bounds;
	
static void deinit(void) {
	// Destroy the text layer
	text_layer_destroy(s_text_layer);	
	// Destroy the window
	accel_tap_service_unsubscribe();
//	window_single_click_unsubscribe();
	window_destroy(s_window);
}
	
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
	// Vibe pattern: ON for 200ms, OFF for 100ms, ON for 400ms:
	static const uint32_t const segments[] = { 200, 100, 400 };
	VibePattern pat = {
  	.durations = segments,
  	.num_segments = ARRAY_LENGTH(segments),
	};

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
	// APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  	// A tap event occured
	char* message = ""; 
	int x,y,z = 0;
	AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };
	accel_service_peek(&accel);
	x = accel.x;
	y = accel.y;
	z = accel.z;
	
	// char message[20];	
	if(direction == 1) {
		// memset(&message[0], 0, sizeof(message));
		// snprintf(message, 48, "Shaky! \n Down \n %d \n %d \n %d",  x, y, z ) ;		
		//snprintf(message, 42, "Shaky! \n Down \n %d \n %d \n %d",  x, y, z ) ;
		//message = "hello world this is fun stuff, right?";
		snprintf(message, 32, "Shaky! Down \n %d %d %d",  x, y, z ) ;
		// biocomPutHomeString(  strcat( "",   message)  );
		text_layer_set_text(s_text_layer, message);
		
	}
	else {
		//text_layer_destroy(s_text_layer);			
		//snprintf(message, 40, "Shaky! \n Up \n%d\n%d\n%d",  x, y, z ) ;	
		//biocomPutHomeString(  strcat("", message ));
		snprintf(message, 32, "Shaky! Up \n %d %d %d",  x, y, z ) ;
		// biocomPutHomeString(  strcat( "",   message)  );
		text_layer_set_text(s_text_layer, message);
		
	}

//	uint32_t num_samples = 3;  // Number of samples per batch/callback	
	// Subscribe to batched data events
	// accel_data_service_subscribe(num_samples, accel_data_handler);	
}

static void accel_data_handler(AccelData *data, uint32_t num_samples) {
  // Read sample 0's x, y, and z values
  int16_t x = data[0].x;
  int16_t y = data[0].y;
  int16_t z = data[0].z;
  // char str[10]; 
	
  // Determine if the sample occured during vibration, and when it occured
  bool did_vibrate = data[0].did_vibrate;
  uint64_t timestamp = data[0].timestamp;

  if(!did_vibrate) {
    // Print it out
    APP_LOG(APP_LOG_LEVEL_INFO, "t: %llu, x: %d, y: %d, z: %d", timestamp, x, y, z);
  } else {
    // Discard with a warning
    APP_LOG(APP_LOG_LEVEL_WARNING, "Vibration occured during collection");
  }
}


static void init(void) {
	s_window = window_create();
	window_set_click_config_provider(s_window, biocomClickConfigProvider);

	biocomPutHomeString("\nBioCom Lab\nWristband\n");
	// psleep(500);
	
	accel_tap_service_subscribe(accel_tap_handler);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
