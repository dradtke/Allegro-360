`xbox.c` is a simple example of how to use an Xbox 360 controller with the Allegro library. The font is provided to make running the program easier, because Allegro 5 doesn't provide a default font.

Make sure allegro is installed (including the modules allegro\_font and allegro\_ttf) and compile with:

    clang -o xbox xbox.c -lallegro -lallegro_font -lallegro_ttf

Running it will open up a blank allegro display with text values written on-screen showing the current state of the controller. NOTE: to avoid possible errors, make sure that the controller is plugged in before running and that it is the only controller plugged in. The program defaults to the first joystick (in allegro terms) found, and doesn't do any extra checking.
