debug:
	gcc source/main.c -Wextra -Wall -fsanitize=address -o binary/voxelize_debug -g -lSDL2 -lGL -lGLEW -lm

release:
	gcc source/main.c -Wextra -Wall -o binary/voxelize_release -g -lSDL2 -lGL -lGLEW -lm
