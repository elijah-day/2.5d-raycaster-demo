#include <stdbool.h>
#include <dirent.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#define PI 3.14

#define ROTATION_SPEED PI / 2048
#define MOVEMENT_SPEED 0.01

#define VIEWLINE_LENGTH 8
#define WALL_SIZE 4

#define RAY_COUNT 64
#define RAY_ANGLE (PI / ((3) * RAY_COUNT))

#define RENDER_SCALE 4
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

typedef struct Wall
{
	int x, y;
}
Wall;

typedef struct Entity
{
	float x, y, theta;
}
Entity;

int main(int argc, char *argv[])
{
	bool running_state = true;
	
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init error\n");
		running_state = false;
	}
	
	SDL_Window *window = SDL_CreateWindow
	(
		"",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	
	if(window == NULL)
	{
		printf("SDL_CreateWindow error\n");	
		running_state = false;
	}
	
	SDL_Renderer *renderer = SDL_CreateRenderer
	(
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	);
	
	if(renderer == NULL)
	{
		printf("SDL_CreateRenderer error\n");
		running_state = false;
	}
	
	int wall_count = 0;
	
	bool room[16][16] =
	{
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	};
	
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 16; j++)
		{
			if(room[i][j] == true) wall_count++;
		}
	}
	
	Wall walls[wall_count];

	int wcounter = 0;
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 16; j++)
		{
			if(room[i][j] == true)
			{
				walls[wcounter].y = i * WALL_SIZE;
				walls[wcounter].x = j * WALL_SIZE;
				wcounter++;
			}
		}
	}
	
	printf("%d\n", wall_count);
	
	Entity player;
	player.x= 16;
	player.y = 16;
	player.theta = 0;
	
	bool minimap_toggle = true;
	int win_w = 0;
	int win_h = 0;
	
	int r, g, b;
	
	int wall_line_scale = 96;
	
	int brightness = 2;
	
	bool sdl_keys[322];
	memset(sdl_keys, false, 322);
	
	SDL_Rect wlrect;
	
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	SDL_Event event;
	while(running_state)
	{
		float ppx = player.x;
		float ppy = player.y;
	
		SDL_GetWindowSize(window, &win_w, &win_h);
		
		while(SDL_PollEvent(&event)) switch(event.type)
		{
			case SDL_QUIT:
				running_state = false;
				break;
				
			case SDL_KEYDOWN:
				sdl_keys[event.key.keysym.scancode] = true;
				
				switch(event.key.keysym.scancode)
				{
					case SDL_SCANCODE_M:
						minimap_toggle = !minimap_toggle;
						break;
				}
				
				break;
			
			case SDL_KEYUP:
				sdl_keys[event.key.keysym.scancode] = false;
				break;
		}
		
		if(sdl_keys[SDL_SCANCODE_ESCAPE]) running_state = 0;
		
		if(sdl_keys[SDL_SCANCODE_LEFT]) player.theta += ROTATION_SPEED;
		if(sdl_keys[SDL_SCANCODE_RIGHT]) player.theta -= ROTATION_SPEED;
		
		if(sdl_keys[SDL_SCANCODE_W])
		{
			player.x += MOVEMENT_SPEED * cos(player.theta);
			player.y -= MOVEMENT_SPEED * sin(player.theta);
		}
		if(sdl_keys[SDL_SCANCODE_A])
		{
			player.x -= MOVEMENT_SPEED * cos(player.theta - PI / 2);
			player.y += MOVEMENT_SPEED * sin(player.theta - PI / 2);
		}
		if(sdl_keys[SDL_SCANCODE_S])
		{
			player.x -= MOVEMENT_SPEED * cos(player.theta);
			player.y += MOVEMENT_SPEED * sin(player.theta);
		}
		if(sdl_keys[SDL_SCANCODE_D])
		{
			player.x -= MOVEMENT_SPEED * cos(player.theta + PI / 2);
			player.y += MOVEMENT_SPEED * sin(player.theta + PI / 2);
		}
		
		if(sdl_keys[SDL_SCANCODE_O]) wall_line_scale -= 1;
		if(sdl_keys[SDL_SCANCODE_P]) wall_line_scale += 1;
		
		
		
		/*Draw BG*/
		SDL_SetRenderDrawColor(renderer, 54, 54, 22, 255);
		SDL_RenderFillRect(renderer, NULL);
		
		/*Draw Floor*/
		wlrect.x = 0;
		wlrect.y = win_h / 2;
		wlrect.w = win_w;
		wlrect.h = win_h / 2;
		SDL_SetRenderDrawColor(renderer, 64, 64, 32, 255);
		SDL_RenderFillRect(renderer, &wlrect);
		
		/*Raycasting*/
		for(int k = 0; k < RAY_COUNT; k++)
		{	
			float rx = player.x;
			float ry = player.y;
			float rlength = 0;
			float rtheta = player.theta;
			bool ray_hit_state = false;
		
			while(!ray_hit_state && rlength < VIEWLINE_LENGTH)
			{
				rtheta = player.theta + (RAY_COUNT / 2 - k) * RAY_ANGLE;
			
				rx += cos(rtheta);
				ry -= sin(rtheta);
				
				for(int i = 0; i < wall_count; i++)
				{
					if
					(
						rx > (float)(walls[i].x) && rx < (float)(walls[i].x) + (float)WALL_SIZE &&
						ry > (float)(walls[i].y) && ry < (float)(walls[i].y) + (float)WALL_SIZE
					)
					{
						ray_hit_state = true;
						/*
						printf("RAY HIT!!!\n");
						*/
						break;
					}
				}
				
				rlength += 0.1;
			}
			
			r = (int)(192.0 / rlength);
			g = (int)(192.0 / rlength);
			b = (int)(96.0 / rlength);
			
			if(r > 191) r = 191;
			if(g > 191) g = 191;
			if(b > 191) b = 191;
			
			SDL_SetRenderDrawColor
			(
				renderer,
				r,
				g,
				b,
				255
			);
			
			if(rlength < VIEWLINE_LENGTH)
			{
				int wall_line_width = (int)((float)win_w / (float)RAY_COUNT);
			
				for(int i = 0; i < wall_line_width; i++) SDL_RenderDrawLine
				(
					renderer,
					win_w  - (RAY_COUNT - k) * wall_line_width + i,
					win_h / 2 + RENDER_SCALE * wall_line_scale / rlength,
					win_w - (RAY_COUNT - k) * wall_line_width + i,
					win_h / 2 - RENDER_SCALE * wall_line_scale / rlength
				);
			}
			
			if(minimap_toggle) SDL_RenderDrawLine
			(
				renderer,
				RENDER_SCALE * (int)player.x,
				RENDER_SCALE * (int)player.y,
				RENDER_SCALE * (int)player.x + rlength * cos(rtheta) * RENDER_SCALE,
				RENDER_SCALE * (int)player.y - rlength * sin(rtheta) * RENDER_SCALE
			);
		}
		
		if(minimap_toggle)
		{
			SDL_Rect wall_rect;
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			for(int i = 0; i < wall_count; i++)
			{
				wall_rect.x = walls[i].x * RENDER_SCALE;
				wall_rect.y = walls[i].y * RENDER_SCALE;
				wall_rect.w = WALL_SIZE * RENDER_SCALE;
				wall_rect.h = WALL_SIZE * RENDER_SCALE;
				
				SDL_RenderFillRect(renderer, &wall_rect);
			}
		}
		
		SDL_RenderPresent(renderer);
		
		/*
		printf("player.theta=%f\n", player.theta);
		*/
		
		if(player.theta > 2 * PI + PI / 48) player.theta = 0;
		if(player.theta < 0) player.theta = 2 * PI;
		
		
		/*
		printf("\n");
		*/
	}
	
	SDL_SetWindowFullscreen(window, 0);
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}
