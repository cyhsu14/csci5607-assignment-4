CC = g++
GLAD = glad/glad.c
LIB = /Library/Frameworks
FF = -framework

all:
	read -p "file name: " file;
	echo "$${file}"
	# $(CC) $(file) $(GLAD) -F$(LIB) $(FF) SDL2 $(FF) OpenGL -o hw
clean:
	rm hw

## g++ maze.cpp -x c glad/glad.c -g -F/Library/Frameworks -framework SDL2 -framework OpenGL -o m
