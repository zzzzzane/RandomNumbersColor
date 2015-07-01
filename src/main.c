//standard header
#include "pebble.h"
#include "iota.h"

//app-specific data
Window *window; //obvious window is obvious
TextLayer *time_layer; //time layer
TextLayer *hour_layer; //time layer
TextLayer *date_layer; //time layer

/*what the window does*/
static void fill_window(struct tm* tick_time) {
	
  	static char full_string[125]; //125 characters
    static char hour_string[6];
    static char date_string[6];
    
    static char time_hours[] = "00";
 	  static char time_minutes[] = "00";
  	static char time_month[] = "00";
  	static char time_day[] = "00";
      
  int r; //init variables for number of lines and random numbers
    int x; //hour-x  location
    int y; //hour-y location
    int dx; //day location
    int dy; //day location
    int bx;
    int by;
  
 
    
	/*each time-component size*/
  /*handle 24 hour time setting */
  if (clock_is_24h_style()) {
  	strftime(time_hours, sizeof("00"), "%H", tick_time);
  } else {    
  	strftime(time_hours, sizeof("00"), "%I", tick_time);
  }
  	strftime(time_minutes, sizeof("00"), "%M", tick_time);
  	strftime(time_month, sizeof("00"), "%m", tick_time);
  	strftime(time_day, sizeof("00"), "%d", tick_time);
	
    strcpy(hour_string,time_hours);
    strcat(hour_string,".");
    strcat(hour_string,time_minutes);
  
    strcpy(date_string,time_month);
    strcat(date_string,".");
    strcat(date_string,time_day);

  	srand(time(NULL)); //'NULL' tick event

        
		  	r = rand() % 99 + 1; //random number, 1-99; this bypasses the whole check for zero

        if (r < 10) {
				  strcpy(full_string, "0");
          strcat(full_string, itoa(r));
        } else
          strcpy(full_string, itoa(r));
          
  	strcat(full_string, ".");
          
    //loop prints 35 number lines
  	for (int i = 2; i < 36; i++) {
		  	r = rand() % 99 + 1; //random number, 1-99; this bypasses the whole check for zero

        if (r < 10)
				  strcat(full_string, "0");
                       
			    strcat(full_string, itoa(r));
        
			  if ((i % 5) == 0)
          {
				  strcat(full_string, "\n");
			    }
          else
            {
				  strcat(full_string, ".");
          }
	  }
	


 //Set the hour layer
		  x = rand() % 4 ; //random number, 1-99; this bypasses the whole check for zero
		  y = rand() % 7 ; //random number, 1-99; this bypasses the whole check for zero
      dx = rand() % 4;
      dy = rand() % 7;
      bx = rand() % 4;
      by = rand() % 7;
  while  ((dx==x && dy==y ) || ((dx==x+1) && dy==y)) 
  {
    dx = rand() % 4;
      dy = rand() % 7;
  }

    while  (
            (bx==x && by==y) || 
           (  bx==x+1  && by==y ) ||
           ( bx==dx && by==dy ) || 
            (bx==dx+1 && by==dy)
          ) 
  {
    bx = rand() % 4;
      by = rand() % 7;
  }

  layer_set_frame(text_layer_get_layer(hour_layer),GRect((x*24) +14 , y*24,46 , 30));
  layer_set_frame(text_layer_get_layer(date_layer),GRect((dx*24) +14 , dy*24,46 , 30));
  
  text_layer_set_text(time_layer, full_string); //set the text
  text_layer_set_text(hour_layer, hour_string); //set the text
  text_layer_set_text(date_layer, date_string); //set the text

}
  
/*called once per minute*/
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
  
//  fill_window(tick_time);

 
  /*Random numbers change once per 10 minutes */ 
	if (((tick_time->tm_sec % 10) == 5))  
    fill_window(tick_time);
  
  }

/*handle the start-up of the app*/
static void do_init(void) {
	
	//create the watchface's base window
  	window = window_create();
  	window_stack_push(window, true);
  	window_set_background_color(window, GColorBlack);


  	//init the text layer used to show the time
  	time_layer = text_layer_create(GRect(0, 0, 144, 168)); //format: GRect(0, 0, width, height)
  	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  	text_layer_set_text_color(time_layer, GColorBlue);
  	text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  
    hour_layer = text_layer_create(GRect(0, 0, 40, 30)); //format: GRect(0, 0, width, height)
    text_layer_set_text_alignment(hour_layer, GTextAlignmentLeft);
  	text_layer_set_text_color(hour_layer, GColorCobaltBlue);
  	text_layer_set_background_color(hour_layer, GColorBlack);
    text_layer_set_font(hour_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

    date_layer = text_layer_create(GRect(0, 0, 40, 30)); //format: GRect(0, 0, width, height)
    text_layer_set_text_alignment(date_layer, GTextAlignmentLeft);
  	text_layer_set_text_color(date_layer, GColorBlueMoon);
  	text_layer_set_background_color(date_layer, GColorBlack);
    text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  
  	/*ensures time is displayed immediately (will break if 'NULL' tick event accessed);
  	(this is why it's a good idea to have a separate routine to do the update itself)*/
  	time_t now = time(NULL);
  	struct tm *current_time = localtime(&now);
  	fill_window(current_time);
  	handle_minute_tick(current_time, SECOND_UNIT);
  	tick_timer_service_subscribe(SECOND_UNIT, &handle_minute_tick);

  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(hour_layer));
  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
  	
}

/*deinits the layers*/
static void do_deinit(void) {
   
	text_layer_destroy(time_layer);
	text_layer_destroy(hour_layer);
	text_layer_destroy(date_layer);
	
  window_destroy(window);
}

/*the main "run" loop for our app*/
int main() {
	
  	do_init();
  	app_event_loop();
  	do_deinit();
}
