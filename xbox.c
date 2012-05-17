#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/*
 * Struct representing the state of an Xbox 360 controller.
 */
typedef struct {
	float leftstick_x;
	float leftstick_y;
	float rightstick_x;
	float rightstick_y;
	float dpad_x;
	float dpad_y;
	float left_trigger;
	float right_trigger;
} XBOX_CONTROLLER;

XBOX_CONTROLLER *new_controller()
{
	XBOX_CONTROLLER *cont = (XBOX_CONTROLLER*)malloc(sizeof(XBOX_CONTROLLER));

	// zero out all values
	cont->leftstick_x = 0;
	cont->leftstick_y = 0;
	cont->rightstick_x = 0;
	cont->rightstick_y = 0;
	cont->dpad_x = 0;
	cont->dpad_y = 0;
	cont->left_trigger = 0;
	cont->right_trigger = 0;

	return cont;
}


/*
 * Update the controller's axis positions.
 */
void update_controller_axes(XBOX_CONTROLLER *cont, ALLEGRO_EVENT event)
{
	int axis = event.joystick.axis;
	float pos = event.joystick.pos;

	switch (event.joystick.stick) {
		case 0:
			if (axis == 0)
				cont->leftstick_x = pos;
			else if (axis == 1)
				cont->leftstick_y = pos;
			break;
		case 1:
			if (axis == 0)
				cont->left_trigger = (pos + 1.0) / 2.0;
			else if (axis == 1)
				cont->rightstick_x = pos;
			break;
		case 2:
			if (axis == 0)
				cont->rightstick_y = pos;
			else if (axis == 1)
				cont->right_trigger = (pos + 1.0) / 2.0;
			break;
		case 3:
			if (axis == 0)
				cont->dpad_x = pos;
			else if (axis == 1)
				cont->dpad_y = pos;
			break;
	}
}

/*
 * Main.
 */
int main(void)
{
	ALLEGRO_DISPLAY	*display = NULL;
	ALLEGRO_EVENT_QUEUE	*event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_JOYSTICK *joystick = NULL;
	ALLEGRO_FONT *font = NULL;
	XBOX_CONTROLLER *controller = new_controller();

	bool running = true;
	bool redraw = false;

	if (!al_init()) {
		fprintf(stderr, "Failed to initialize allegro.\n");
		return 1;
	}

	timer = al_create_timer(1.0 / 60);
	if (!timer) {
		fprintf(stderr, "Failed to create timer.\n");
		return 1;
	}

	if (!al_install_joystick()) {
		fprintf(stderr, "Failed to install joystick.\n");
		return 1;
	}

	joystick = al_get_joystick(0);
	if (!joystick) {
		fprintf(stderr, "Failed to find installed joystick.\n");
		return 1;
	}

	display = al_create_display(640, 480);
	if (!display) {
		fprintf(stderr, "Failed to create display.\n");
		return 1;
	}

	al_init_font_addon();
	al_init_ttf_addon();
	font = al_load_font("DroidSansMono.ttf", 14, 0);
	if (!font) {
		fprintf(stderr, "Failed to load font.\n");
		return 1;
	}

	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "Failed to create event queue.\n");
		return 1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_joystick_event_source());

	al_start_timer(timer);

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	while (running) {
		ALLEGRO_EVENT event;
		ALLEGRO_TIMEOUT	timeout;

		al_init_timeout(&timeout, 0.06);

		bool get_event = al_wait_for_event_until(event_queue, &event, &timeout);

		if (get_event) {
			switch (event.type) {
				case ALLEGRO_EVENT_TIMER:
					redraw = true;
					break;
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					running = false;
					break;
				case ALLEGRO_EVENT_JOYSTICK_AXIS:
					update_controller_axes(controller, event);
					break;
				default:
					fprintf(stderr, "Unsupported event received: %d\n", event.type);
					break;
			}
		}

		if (!running)
			break;

		if (redraw && al_is_event_queue_empty(event_queue)) {
			al_clear_to_color(al_map_rgb(0, 0, 0));

			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, "left stick: (%f,%f)", controller->leftstick_x, controller->leftstick_y);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 25, ALLEGRO_ALIGN_LEFT, "right stick: (%f,%f)", controller->rightstick_x, controller->rightstick_y);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 40, ALLEGRO_ALIGN_LEFT, "d-pad: (%f,%f)", controller->dpad_x, controller->dpad_y);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 75, ALLEGRO_ALIGN_LEFT, "left trigger: %f", controller->left_trigger);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 90, ALLEGRO_ALIGN_LEFT, "right trigger: %f", controller->right_trigger);

			al_flip_display();
			redraw = false;
		}
	}

	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
