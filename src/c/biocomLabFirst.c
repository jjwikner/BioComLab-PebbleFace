#include <pebble.h>
// #include <biocomPebbleDefinitions.h>

// #include <unistd.h>

// Originating from the hello world-concept to illustrate and find conceptual limits with the pebble equipment.
// 
// JJ Wikner 
// BioCom Lab
//

#define MSG_LENGTH 48

static Window *s_window;
static TextLayer *s_text_layer;
// static GRect bounds;
char message[MSG_LENGTH];
static BitmapLayer *s_bitmap_layer;
static GBitmap *s_bitmap;


static void deinit(void) {
	// Destroy the text layer
	text_layer_destroy(s_text_layer);	
	// Destroy the window
	accel_tap_service_unsubscribe();
	//	window_single_click_unsubscribe();
	
	// Destroy the BitmapLayer
	bitmap_layer_destroy(s_bitmap_layer);

	window_destroy(s_window);
}
	
static void biocomSelectSingleClickHandler(ClickRecognizerRef recognizer, void *context) {
	// Detect that a single click has just occured. 
	// Notice that the binders is independent of the button clicked.
	accel_tap_service_unsubscribe();
	deinit();
}
char *itoa(int num)
{
  static char buff[20] = {};
  int i = 0, temp_num = num, length = 0;
  char *string = buff;
  
  if(num >= 0) { //See NOTE
    // count how many characters in the number
    while(temp_num) {
      temp_num /= 10;
      length++;
    }
    // assign the number to the buffer starting at the end of the 
    // number and going to the begining since we are doing the
    // integer to character conversion on the last number in the
    // sequence
    for(i = 0; i < length; i++) {
      buff[(length-1)-i] = '0' + (num % 10);
      num /= 10;
    }
    buff[i] = '\0'; // can't forget the null byte to properly end our string
  }
  else {
    return "unum";
  }
  
  return string;
}

static void biocomAccelTapHandler(AccelAxisType axis, int32_t direction) {
  	// A tap event occured

	int x, y, z = 0; 
	int p = 0;
	AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };
	accel_service_peek(&accel);
	x = accel.x;
	y = accel.y;
	z = accel.z;
	p = x*x + y*y + z*z;
	// char message[20];	
	if(direction == 1) {
		// memset(&message[0], 0, sizeof(message));
		// snprintf(message, 48, "Shaky! \n Down \n %d \n %d \n %d",  x, y, z ) ;		
		//snprintf(message, 42, "Shaky! \n Down \n %d \n %d \n %d",  x, y, z ) ;
		//message = "hello world this is fun stuff, right?";
		snprintf(message, MSG_LENGTH, "Shaky! Dn \n%d %d %d\n%d", x, y, z, p);
		// biocomPutHomeString(  strcat( "",   message)  );
		// something goes wrong with the pointers here
		// message = strcat( strcat( strcat("Shaky! Dn ", itoa(x)), itoa(y)), itoa(z) );
		text_layer_set_text(s_text_layer,  message);
			   
	}
	else {
		//text_layer_destroy(s_text_layer);			
		//snprintf(message, 40, "Shaky! \n Up \n%d\n%d\n%d",  x, y, z ) ;	
		//biocomPutHomeString(  strcat("", message ));
		//snprintf(message, 32, "Shaky! Up \n %d %d %d",  x, y, z ) ;
		// biocomPutHomeString(  strcat( "",   message)  );
		snprintf(message, MSG_LENGTH, "Shaky! Up \n%d %d %d\n%d", x, y, z, p);
		text_layer_set_text(s_text_layer, message);
		
	}

//	uint32_t num_samples = 3;  // Number of samples per batch/callback	
	// Subscribe to batched data events
	// accel_data_service_subscribe(num_samples, accel_data_handler);	
}

static void biocomUpSingleClickHandler(ClickRecognizerRef recognizer, void *context) {
	Layer *window_layer = window_get_root_layer(s_window);  	
	GRect bounds = layer_get_bounds(window_layer);

	
	// A single click has just occured
	// In this case, create a buzzing sound to illustrate this
	accel_tap_service_unsubscribe();
	vibes_long_pulse();
	accel_tap_service_subscribe(biocomAccelTapHandler);
	
	// Load the image
	s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FILE1);

	// Create a BitmapLayer
	s_bitmap_layer = bitmap_layer_create(bounds);

	// Set the bitmap and compositing mode
	bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
	bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);

	// Add to the Window
	layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
	
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
	accel_tap_service_unsubscribe();
	vibes_enqueue_custom_pattern(pat);
	accel_tap_service_subscribe(biocomAccelTapHandler);
	// Destroy the BitmapLayer
	gbitmap_destroy(s_bitmap);
	bitmap_layer_destroy(s_bitmap_layer);

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
	accel_tap_service_subscribe(biocomAccelTapHandler);
	biocomPutHomeString("\nBioCom Lab\nWristband\n");
	// psleep(500);
	
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
