//Multi-Object, Multi-Texture Example
//Stephen J. Guy, 2018

//This example demonstartes:
// Loading multiple models (a cube and a knot)
// Using multiple textures (wood and brick)
// Instancing (the teapot is drawn in two locatons)
// Continuous keyboard input - arrows (moves knot up/down/left/right continueous on being held)
// Keyboard modifiers - shift (up/down arrows move knot in/out of screen when shift is pressed)
// Single key events - pressing 'c' changes color of a random teapot
// Mixing textures and colors for models
// Phong lighting
// Binding multiple textures to one shader


//Mac OS build: g++ multiObjectTest.cpp -x c glad/glad.c -g -F/Library/Frameworks -framework SDL2 -framework OpenGL -o MultiObjTest
//Linux build:  g++ multiObjectTest.cpp -x c glad/glad.c -g -lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/ -o MultiObjTest

#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <vector>
typedef vector <pair<int,int> > ModelInfo; // starting point, num of vertices
ModelInfo mi;


int screenWidth = 800;
int screenHeight = 600;
float timePast = 0;

//SJG: Store the object coordinates
//You should have a representation for the state of each object
float colR=1, colG=1, colB=1;
ModelInfo doors;
ModelInfo keys;
bool isCollected[5] = {false};
bool isOpened[5] = {false};
glm::vec3 key_info = glm::vec3(0.f, 0.f, -2); // x,y: location, z: texture
bool isSucess = false;
float t = 0;

typedef vector<glm::vec3> vecList;

// bool DEBUG_ON = true;
bool DEBUG_ON = false;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

//srand(time(NULL));
float rand01(){
	return rand()/(float)RAND_MAX;
}

void drawGeometry(int shaderProgram, ModelInfo mi, int w, int h, char* map[]);

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

	//Ask SDL to get a recent version of OpenGL (3.2 or greater)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//Load OpenGL extentions with GLAD
	if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));
	}
	else {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}
// rgb()
    /* Load maze */

    doors.resize(5, make_pair(-1,-1));
    keys.resize(5, make_pair(-1,-1));
    // for(int i=0;i<5;i++)printf("%d %d\n", doors.at(i).first,doors.at(i).second);
    ifstream maze;
    int maze_w, maze_h;
    glm::vec3 cam_pos;
    glm::vec3 look_pt = glm::vec3(-0.5f, 0.f, -0.3f);
    int i,j;

    maze.open("level1.txt");
    maze >> maze_w >> maze_h;
    char **map = new char*[maze_h];//[maze_w]; //[maze_w];
    for(i = 0;i < maze_h; i++){
        map[i] = new char[maze_w];
        for (j = 0; j < maze_w; j++) {
            maze >> map[i][j];
            if(map[i][j]=='S') {
                cam_pos = glm::vec3(i,j,1.f);
            }
            else if(map[i][j]=='A') doors.at(0) = make_pair(i,j);
            else if(map[i][j]=='B') doors.at(1) = make_pair(i,j);
            else if(map[i][j]=='C') doors.at(2) = make_pair(i,j);
            else if(map[i][j]=='D') doors.at(3) = make_pair(i,j);
            else if(map[i][j]=='E') doors.at(4) = make_pair(i,j);
            else if(map[i][j]=='a') keys.at(0) = make_pair(i,j);
            else if(map[i][j]=='b') keys.at(1) = make_pair(i,j);
            else if(map[i][j]=='c') keys.at(2) = make_pair(i,j);
            else if(map[i][j]=='d') keys.at(3) = make_pair(i,j);
            else if(map[i][j]=='e') keys.at(4) = make_pair(i,j);

        }
    }

	//Here we will load two different model files

	//Load Model 1
	ifstream modelFile;
	modelFile.open("models/square.txt");
	int numLines = 0;
	modelFile >> numLines;
	float* model1 = new float[numLines];
	for (i = 0; i < numLines; i++){
		modelFile >> model1[i];
	}
	printf("%d\n",numLines);
	int numVertsSquare = numLines/8;
	modelFile.close();

	//Load Model 2
	modelFile.open("models/cube.txt");
	numLines = 0;
	modelFile >> numLines;
	float* model2 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model2[i];
	}
	printf("%d\n",numLines);
	int numVertsCube = numLines/8;
	modelFile.close();

    //Load Model 3
    modelFile.open("models/teapot.txt");
    numLines = 0;
    modelFile >> numLines;
    float* model3 = new float[numLines];
    for (int i = 0; i < numLines; i++) {
        modelFile >> model3[i];
    }
    printf("%d\n",numLines);
	int numVertsTeapot = numLines/8;
	modelFile.close();

    // Load Model 4
    modelFile.open("models/knot.txt");
    numLines = 0;
    modelFile >> numLines;
    float* model4 = new float[numLines];
    for (int i = 0; i < numLines; i++) {
        modelFile >> model4[i];
    }
    printf("%d\n",numLines);
	int numVertsKnot = numLines/8;
	modelFile.close();

    // Load Model 5
    //modelFile.open("models/sphere.txt");
    //*** Load obj file ***//
    vecList vertices;
    vecList normals;
    vecList textures;
    string type;
    char tmp[20];
    int cnt = 0;

    ifstream file;
    file.open("models/sphere.obj");
    while(file.good()){
        file>>type;
        if(type=="f") cnt++;
        else{
            getline(file, type);
        }
    }
    file.close();
    //
    file.open("models/sphere.obj");
    ofstream out;
    out.open("models/sphereobj.txt");
    out << cnt*24 <<endl;
    while(file >> type){
        float x, y, z;
        if(type=="v") {
            file >> x >> y >> z;
            vertices.push_back(glm::vec3(x, y, z));
        }
        else if(type=="vn"){
            file >> x >> y >> z;
            normals.push_back(glm::vec3(x, y, z));
        }
        else if(type=="vt"){
            file >> x >> y; // >>z;
            textures.push_back(glm::vec3(x, y, 0.0f));
        }
        else if (type=="f"){
            for(int i=0; i<3; i++){
                file >> tmp;
                char * pch;
                pch = strtok (tmp,"/");
                x = atoi(pch);
                pch = strtok (NULL, "/");
                y = atoi(pch);
                pch = strtok (NULL, "/");
                z = atoi(pch);

                out << vertices.at(x-1).x << endl << vertices.at(x-1).y << endl << vertices.at(x-1).z << endl;
                // out << vertices.at(y-1).x << endl << vertices.at(y-1).y << endl; // << textures.at(z)<<endl;
                // out << vertices.at(z-1).x << endl << vertices.at(z-1).y << endl << vertices.at(z-1).z << endl;
                out << textures.at(y-1).x << endl << textures.at(y-1).y << endl; // << textures.at(z)<<endl;
                out << normals.at(z-1).x << endl << normals.at(z-1).y << endl << normals.at(z-1).z << endl;
            }

        }
        else{ // take unneeded line
            getline(file, type);
        }

    }
    out.close();
    file.close();

    //*** Load obj file ***//



    modelFile.open("models/sphereobj.txt");
	numLines = 0;

    modelFile >> numLines;
    float* model5 = new float[numLines];
    for (int i = 0; i < numLines; i++) {
        modelFile >> model5[i];
    }
    printf("%d\n",numLines);
	int numVertsSphere = numLines/8;
	modelFile.close();


	//SJG: I load each model in a different array, then concatenate everything in one big array
	// This structure works, but there is room for improvement here. Eg., you should store the start
	// and end of each model a data structure or array somewhere.
	//Concatenate model arrays
	float* modelData = new float[(numVertsSquare+numVertsCube+numVertsTeapot+numVertsKnot+numVertsSphere)*8];
	copy(model1, model1+numVertsSquare*8, modelData);
	copy(model2, model2+numVertsCube*8, modelData+numVertsSquare*8);
    copy(model3, model3+numVertsTeapot*8, modelData+(numVertsSquare+numVertsCube)*8);
    copy(model4, model4+numVertsKnot*8, modelData+(numVertsSquare+numVertsCube+numVertsTeapot)*8);
    copy(model5, model5+numVertsSphere*8, modelData+(numVertsSquare+numVertsCube+numVertsTeapot+numVertsKnot)*8);
	int totalNumVerts = numVertsSquare+numVertsCube+numVertsTeapot+numVertsKnot+numVertsSphere;
	// int startVertTeapot = 0;  //The teapot is the first model in the VBO
	// int startVertKnot = numVertsTeapot; //The knot starts right after the taepot
    mi.push_back(make_pair(0, numVertsSquare));
    mi.push_back(make_pair(numVertsSquare, numVertsCube));
    mi.push_back(make_pair(numVertsSquare+numVertsCube, numVertsTeapot));
    mi.push_back(make_pair(numVertsSquare+numVertsCube+numVertsTeapot, numVertsKnot));
    mi.push_back(make_pair(numVertsSquare+numVertsCube+numVertsTeapot+numVertsKnot, numVertsSphere));


	//// Allocate Texture 0 (Wood) ///////
	SDL_Surface* surface = SDL_LoadBMP("wood.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex0;
    glGenTextures(1, &tex0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);

    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture

    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////


	//// Allocate Texture 1 (Brick) ///////
	SDL_Surface* surface1 = SDL_LoadBMP("brick.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex1;
    glGenTextures(1, &tex1);

    //Load the texture into memory
    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, tex1);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w,surface1->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface1->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture

    SDL_FreeSurface(surface1);
	//// End Allocate Texture ///////

    //// Allocate Texture 2 (Bump map for brick) ///////
	SDL_Surface* surface2 = SDL_LoadBMP("brick_NRM.bmp");
	if (surface2==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex2;
    glGenTextures(1, &tex2);

    //Load the texture into memory
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2);

    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface2->w,surface2->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface2->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture

    SDL_FreeSurface(surface2);
	//// End Allocate Texture ///////

    //// Allocate Texture 3 (Bump map for wood) ///////
	SDL_Surface* surface3 = SDL_LoadBMP("wood_NRM.bmp");
	if (surface3==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex3;
    glGenTextures(1, &tex3);

    //Load the texture into memory
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tex3);

    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface3->w,surface3->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface3->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture

    SDL_FreeSurface(surface3);
	//// End Allocate Texture ///////



	//Build a Vertex Array Object (VAO) to store mapping of shader attributse to VBO
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, totalNumVerts*8*sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

	int texturedShader = InitShader("textured-Vertex.glsl", "textured-Fragment.glsl");

	//Tell OpenGL how to set fragment shader input
	GLint posAttrib = glGetAttribLocation(texturedShader, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	  //Attribute, vals/attrib., type, isNormalized, stride, offset
	glEnableVertexAttribArray(posAttrib);

	//GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(colAttrib);

	GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

	GLint uniView = glGetUniformLocation(texturedShader, "view");
	GLint uniProj = glGetUniformLocation(texturedShader, "proj");

	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one


	glEnable(GL_DEPTH_TEST);


	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;
	bool quit = false;
    glm::vec3 next;
	while (!quit){
		while (SDL_PollEvent(&windowEvent)){  //inspect all events in the queue
			if (windowEvent.type == SDL_QUIT) quit = true;
			//List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
			//Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
			quit = true; //Exit event loop
    		if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
    			fullscreen = !fullscreen;
    			SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen
    		}
    		if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_a){ //If "a" is pressed
                if(look_pt == glm::vec3(-0.5f, 0.f, -0.3f) )
                    look_pt += glm::vec3(0.5f, -0.5f, 0.f);
                else if(look_pt == glm::vec3(0.f, -0.5f, -0.3f))
                    look_pt += glm::vec3(0.5f, 0.5f, 0.f);
                else if(look_pt == glm::vec3(0.5f, 0.f, -0.3f))
                    look_pt += glm::vec3(-0.5f, 0.5f, 0.f);
                else if(look_pt == glm::vec3(0.f, 0.5f, -0.3f))
                    look_pt += glm::vec3(-0.5f, -0.5f, 0.f);
                key_info = glm::vec3(cam_pos.x+look_pt.x,cam_pos.y+look_pt.y, key_info.z);


    		}
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_d){ //If "d" is pressed
                if(look_pt == glm::vec3(-0.5f, 0.f, -0.3f) )
                    look_pt += glm::vec3(0.5f, 0.5f, 0.f);
                else if(look_pt == glm::vec3(0.f, -0.5f, -0.3f))
                    look_pt += glm::vec3(-0.5f, 0.5f, 0.f);
                else if(look_pt == glm::vec3(0.5f, 0.f, -0.3f))
                    look_pt += glm::vec3(-0.5f, -0.5f, 0.f);
                else if(look_pt == glm::vec3(0.f, 0.5f, -0.3f))
                    look_pt += glm::vec3(0.5f, -0.5f, 0.f);
                key_info = glm::vec3(cam_pos.x+look_pt.x,cam_pos.y+look_pt.y, key_info.z);


    		}
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_w){ //If "w" is pressed
                next = cam_pos + glm::vec3(look_pt.x*2, look_pt.y*2, 0.f);
                if(next.x <= maze_h-1 && next.x >= 0 && next.y <= maze_w-1 && next.y >=0
                   && map[(int)next.x][(int)next.y]!='W'){
                    cam_pos = next;
                    key_info = glm::vec3(cam_pos.x+look_pt.x,cam_pos.y+look_pt.y, key_info.z);
                    if(map[(int)next.x][(int)next.y]=='G'){ // succeed!
                        cout << "CONGRATULATIONS! YOU HAVE ESCAPE FROM THE MAZE!!" << endl;
                        isSucess = true;
                        map[(int)next.x][(int)next.y] = '0';
                        key_info.z = 7;
                        for (int i=0; i<5; i++)isCollected[i] = true;
                    }
                    else
                        for(int i=0; i<5; i++){
                            if(map[(int)next.x][(int)next.y] == 'a'+i){ // collect keys
                                key_info = glm::vec3(cam_pos.x+look_pt.x,cam_pos.y+look_pt.y, i);
                                keys.at(i) = make_pair(-1, -1);
                                isCollected[i] = true;
                                map[(int)next.x][(int)next.y] = '0';
                            }
                            else if (map[(int)next.x][(int)next.y] == 'A'+i){ // open doors
                                if(isCollected[i]){
                                    doors.at(i) = make_pair(-1, -1);
                                    isOpened[i] = true;
                                    t = SDL_GetTicks()/1000.f;
                                    map[(int)next.x][(int)next.y] = i + 1 + '0';

                                }
                                else {
                                    cam_pos = cam_pos - glm::vec3(look_pt.x*2, look_pt.y*2, 0.f);
                                    key_info = glm::vec3(cam_pos.x+look_pt.x,cam_pos.y+look_pt.y, key_info.z);
                                }

                            }
                        }

                }
    		}
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_s){ //If "s" is pressed
                next = cam_pos - glm::vec3(look_pt.x*2, look_pt.y*2, 0.f);
                if(next.x <= maze_h-1 && next.x >= 0 && next.y <= maze_w-1 && next.y >=0
                   && map[(int)next.x][(int)next.y]!='W'){
                    cam_pos = next;
                    key_info = glm::vec3(cam_pos.x+look_pt.x,cam_pos.y+look_pt.y, key_info.z);
                    if(map[(int)next.x][(int)next.y] == 'G'){ // succeed!
                        cout << "CONGRATULATIONS! YOU HAVE ESCAPE FROM THE MAZE!!" << endl;
                        isSucess = true;
                        map[(int)next.x][(int)next.y] = '0';
                        key_info.z = 7;
                        for (int i=0; i<5; i++)isCollected[i] = true;
                    }
                    else
                        for(int i=0; i<5; i++){
                            if(map[(int)next.x][(int)next.y] == 'a'+i){
                                key_info = glm::vec3(cam_pos.x-look_pt.x,cam_pos.y-look_pt.y, i);
                                keys.at(i) = make_pair(-1, -1);
                                isCollected[i] = true;
                                map[(int)next.x][(int)next.y] = '0';
                            }
                            else if (map[(int)next.x][(int)next.y] == 'A'+i){ // open doors
                                if(isCollected[i]){
                                    doors.at(i) = make_pair(-1, -1);
                                    isOpened[i] = true;
                                    t = SDL_GetTicks()/1000.f;
                                    map[(int)next.x][(int)next.y] = i + 1 + '0';
                                }
                                else {
                                    cam_pos = cam_pos + glm::vec3(look_pt.x*2, look_pt.y*2, 0.f);
                                    key_info = glm::vec3(cam_pos.x+look_pt.x,cam_pos.y+look_pt.y, key_info.z);
                                }
                            }
                        }
                    // printf("%f %f %f \n", cam_pos.x,cam_pos.y,cam_pos.z);
                }
    		}

		}

		// Clear the screen to default color
		glClearColor(.2f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(texturedShader);


		timePast = SDL_GetTicks()/1000.f;

		// glm::mat4 view = glm::lookAt(
		// glm::vec3(5.f, 1.f, 0.5f),  //Cam Position
		// glm::vec3(1.0f, 1.0f, 0.0f),  //Look at point
		// glm::vec3(0.0f, 0.0f, 1.0f)); //Up

        glm::mat4 view = glm::lookAt(
            cam_pos,
            cam_pos+look_pt,
            glm::vec3(0.0f, 0.0f, 1.0f)); //Up

		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 proj = glm::perspective(3.14f/3, screenWidth / (float) screenHeight, 0.5f, 15.0f); //FOV, aspect, near, far
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);
		glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);
		glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

        glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, tex2);
		glUniform1i(glGetUniformLocation(texturedShader, "tex2"), 2);

        glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, tex3);
		glUniform1i(glGetUniformLocation(texturedShader, "tex3"), 3);


		glBindVertexArray(vao);
		drawGeometry(texturedShader, mi, maze_w, maze_h, map);

		SDL_GL_SwapWindow(window); //Double buffering
	}

	//Clean Up
	glDeleteProgram(texturedShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

void drawGeometry(int shaderProgram, ModelInfo mi, int w, int h, char* map[]){

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
	glm::vec3 colVec(colR,colG,colB);
	glUniform3fv(uniColor, 1, glm::value_ptr(colVec));

    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");

	//************
	//Draw models
	//This model is stored in the VBO starting a offest model1_start and with model1_numVerts num of verticies
	//*************
	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    glm::mat4 model;
    model = glm::mat4(); //Load intentity
    int i,j;
    for (i=0;i<h;i++){
        for(j=0;j<w;j++){
            // floor
            model = glm::mat4();
            model = glm::translate(model,glm::vec3(i,j,0));
            //pass model matrix to shader
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            //Set which texture to use (-1 = no texture, 0 = wood, 1 = brick)
            glUniform1i(uniTexID, 0);
            //Draw an instance of the model (at the position & orientation specified by the model matrix above)
            glDrawArrays(GL_TRIANGLES, mi.at(0).first, mi.at(0).second);

            //walls
            if(map[i][j]=='W'){
                model = glm::mat4();
                model = glm::translate(model,glm::vec3(i,j,0));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(uniTexID, 1);
                glDrawArrays(GL_TRIANGLES, mi.at(1).first, mi.at(1).second);
            }
            // success knot
            else if (map[i][j]=='G'){
                float time = SDL_GetTicks()/1000.f;
                model = glm::mat4();
                model = glm::translate(model,glm::vec3(i, j, 0.2f));
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
                model = glm::rotate(model,time * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
                model = glm::rotate(model,time * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(uniTexID, 7);
                glDrawArrays(GL_TRIANGLES, mi.at(3).first, mi.at(3).second);
            }
            // animation for door
            else if(map[i][j]!='0'){
                for (int k=0; k<5; k++){
                    if(map[i][j] == k + 1 + '0'){
                        float t2 = SDL_GetTicks()/1000.f;
                        model = glm::mat4();
                        model = glm::translate(model,glm::vec3(i, j, 0.5f-(t2-t)*.4));
                        model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
                        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                        glUniform1i(uniTexID, k+2);
                        glDrawArrays(GL_TRIANGLES, mi.at(4).first, mi.at(4).second);
                        if(t2-t > 2.5) map[i][j]='0';
                    }
                }

            }
        }
    }

	//************
	//Draw model #cube as door
    // A -> red, B -> yellow, C -> orange, D -> black, E -> white
	//*************
    for(i=0; i<5; i++){
        if(doors.at(i).first != -1){
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(doors.at(i).first,doors.at(i).second,0.5f));
            model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(uniTexID, i+2);
            glDrawArrays(GL_TRIANGLES, mi.at(4).first, mi.at(4).second);
        }
        if(keys.at(i).first != -1 ){
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(keys.at(i).first, keys.at(i).second, 0.2f));
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(uniTexID, i+2);
            glDrawArrays(GL_TRIANGLES, mi.at(2).first, mi.at(2).second);
        }

    }

    //************
	//Draw model #teapot in front of camera
    // A -> red, B -> yellow, C -> orange, D -> black, E -> white
	//*************
    if(key_info.z != -2){
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(key_info.x, key_info.y, 0.5f));
        if(!isSucess){
            model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(uniTexID, key_info.z+2);
            glDrawArrays(GL_TRIANGLES, mi.at(2).first, mi.at(2).second);
        }
        else{
            float time = SDL_GetTicks()/1000.f;
            model = glm::rotate(model,time * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
            model = glm::rotate(model,time * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(uniTexID, 7);
            glDrawArrays(GL_TRIANGLES, mi.at(3).first, mi.at(3).second);
        }

    }

}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile){
	FILE *fp;
	long length;
	char *buffer;

	// open the file containing the text of the shader code
	fp = fopen(shaderFile, "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("can't open shader source file %s\n", shaderFile);
		return NULL;
	}

	// determine the file size
	fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
	length = ftell(fp);  // return the value of the current position

	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];

	// read the appropriate number of bytes from the file
	fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
	fread(buffer, 1, length, fp); // read all of the bytes

	// append a NULL character to indicate the end of the string
	buffer[length] = '\0';

	// close the file
	fclose(fp);

	// return the string
	return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName){
	GLuint vertex_shader, fragment_shader;
	GLchar *vs_text, *fs_text;
	GLuint program;

	// check GLSL version
	printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
	fs_text = readShaderSource(fShaderFileName);

	// error check
	if (vs_text == NULL) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("Vertex Shader:\n=====================\n");
		printf("%s\n", vs_text);
		printf("=====================\n\n");
	}
	if (fs_text == NULL) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("\nFragment Shader:\n=====================\n");
		printf("%s\n", fs_text);
		printf("=====================\n\n");
	}

	// Load Vertex Shader
	const char *vv = vs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
	glCompileShader(vertex_shader); // Compile shaders

	// Check for errors
	GLint  compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Vertex shader failed to compile:\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Load Fragment Shader
	const char *ff = fs_text;
	glShaderSource(fragment_shader, 1, &ff, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);

	//Check for Errors
	if (!compiled) {
		printf("Fragment shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Link and set program to use
	glLinkProgram(program);

	return program;
}
