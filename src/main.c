#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "xc.h"

int main(void)
{
	ALLEGRO_DISPLAY	*display = NULL;
	ALLEGRO_EVENT_QUEUE	*event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font = NULL;

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

	xc_install();
	XC_STATE *controller = xc_get_state(0);
	if (!controller) {
		fprintf(stderr, "Failed to grab xbox controller state.\n");
		return 1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, xc_get_event_source());

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
				case XC_EVENT_AXIS:
				case XC_EVENT_BUTTON_DOWN:
				case XC_EVENT_BUTTON_UP:
					xc_update(event);
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

			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, "left stick: (%f,%f)", controller->left_stick_x, controller->left_stick_y);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 25, ALLEGRO_ALIGN_LEFT, "right stick: (%f,%f)", controller->right_stick_x, controller->right_stick_y);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 40, ALLEGRO_ALIGN_LEFT, "d-pad: (%f,%f)", controller->dpad_x, controller->dpad_y);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 75, ALLEGRO_ALIGN_LEFT, "left trigger: %f", controller->left_trigger);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 90, ALLEGRO_ALIGN_LEFT, "right trigger: %f", controller->right_trigger);

			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 10, ALLEGRO_ALIGN_LEFT, "A: %d", controller->button_a);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 25, ALLEGRO_ALIGN_LEFT, "B: %d", controller->button_b);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 40, ALLEGRO_ALIGN_LEFT, "X: %d", controller->button_x);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 55, ALLEGRO_ALIGN_LEFT, "Y: %d", controller->button_y);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 70, ALLEGRO_ALIGN_LEFT, "Left Stick: %d", controller->button_left_stick);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 85, ALLEGRO_ALIGN_LEFT, "Right Stick: %d", controller->button_right_stick);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 100, ALLEGRO_ALIGN_LEFT, "Left Shoulder: %d", controller->button_left_shoulder);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 115, ALLEGRO_ALIGN_LEFT, "Right Shoulder: %d", controller->button_right_shoulder);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 130, ALLEGRO_ALIGN_LEFT, "Back: %d", controller->button_back);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 145, ALLEGRO_ALIGN_LEFT, "Start: %d", controller->button_start);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 450, 160, ALLEGRO_ALIGN_LEFT, "Xbox: %d", controller->button_xbox);

			al_flip_display();
			redraw = false;
		}
	}

	xc_free_state(controller);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
