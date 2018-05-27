#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <SDL.h>

typedef struct Window {
	SDL_Window* sdl_window;
} Window;

typedef struct int2 {
	int x, y;
} int2;

int2 CENTERED = { -1, -1 };

Window create_window(const char* title, int2 pos, int2 size)
{
	int x = pos.x < 0 ? SDL_WINDOWPOS_CENTERED : pos.x;
	int y = pos.y < 0 ? SDL_WINDOWPOS_CENTERED : pos.y;
	SDL_Window* sdl_window = SDL_CreateWindow(title, x, y, size.x, size.y, 0);
	return (Window) { sdl_window };
}

void destroy_window(Window window)
{
	SDL_DestroyWindow(window.sdl_window);
}

typedef struct Key {
	bool down;
	bool pressed;
	bool released;
} Key;

enum { 
	KEY_RETURN = SDL_SCANCODE_RETURN,
	NUM_KEYS = 256,
};

Key keys[NUM_KEYS];

void update(void)
{
	SDL_PumpEvents();
	const uint8_t* sdl_key_state = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < NUM_KEYS; i++)
	{
		bool down = sdl_key_state[i];
		bool was_down = keys[i].down;
		keys[i].down = down;
		keys[i].pressed = down & !was_down;
		keys[i].released = !down & was_down;
	}
}

bool init(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		return false;
	}

	return true;
}

typedef struct KeyState {
	uint8_t keys[256];
} KeyState;

int main(int argc, char** argv)
{
	if (!init())
	{
		perror("init failed");
		exit(1);
	}
	Window window = create_window("hello", CENTERED, (int2){ 800, 600 });
	for (;;)
	{
		update();

		if (keys[KEY_RETURN].pressed)
		{
			printf("Return pressed\n");
		}
		if (keys[KEY_RETURN].released)
		{
			printf("Return released\n");
		}
	}
	return 0;
}