debug:
	gcc source/main.c -Wextra -Wall -fsanitize=address -o ../voxelize.elf -g -lSDL2 -lGL -lGLEW -lm

release:
	gcc source/main.c -Wextra -Wall -o ../voxelize.elf -g -lSDL2 -lGL -lGLEW -lm
