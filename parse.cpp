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

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
typedef vector<glm::vec3> vecList;

int main(){

    vecList vertices;
    vecList normals;
    vecList textures;
    string type;
    char tmp[20];
    int cnt = 0;

    ifstream file;
    file.open("iii.obj");
    while(file.good()){
        file>>type;
        if(type=="f") cnt++;
        else{
            getline(file, type);
        }
    }
    file.close();
    //
    file.open("iii.obj");
    ofstream out;
    out.open("test.txt");
    out << cnt*24 <<endl;
    while(file >> type){

        float x, y, z;

        if(type=="v") {
            file >> x >> y >> z;
            // if(abs(x)<.00000001)x=0;
            // if(abs(y)<.00000001)y=0;
            // if(abs(z)<.00000001)z=0;
            vertices.push_back(glm::vec3(x, y, z));
        }
        else if(type=="vn"){
            file >> x >> y >> z;
            // if(abs(x)<.00000001)x=0;
            // if(abs(y)<.00000001)y=0;
            // if(abs(z)<.00000001)z=0;
            normals.push_back(glm::vec3(x, y, z));
        }
        else if(type=="vt"){
            file >> x >> y; // >>z;
            // if(abs(x)<.00000001)x=0;
            // if(abs(y)<.00000001)y=0;
            // if(abs(z)<.00000001)z=0;
            textures.push_back(glm::vec3(x, y, 0.0f));
        }
        else if (type=="f"){
            for(int i=0; i<3; i++){
                file >> tmp;
                char * pch;
                cout << i<<":"<<tmp << endl;
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

    // file.open("test.txt");
    // out.open("test2.txt");
    // out << cnt*8 << endl;
    // while(file.good()){
    //     string s;
    //     out << getline(file,s)<<endl;
    // }
    // out.close();
    // file.close();

}
