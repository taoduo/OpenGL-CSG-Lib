/**
 * Duo Tao
 * main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>

#include "CSG.h"
#include "shader.c"
#include "camera.c"
#include "light.c"

GLuint program;
// locations
GLint attrLocs[3];
GLint viewingLoc;
GLint lightColLoc;
GLint lightDirLoc;
GLint camPosLoc;

// global structures
camCamera cam;
meshGLMesh mesh1;
lightLight light;

void handleError(int error, const char *description) {
	fprintf(stderr, "handleError: %d\n%s\n", error, description);
}

void handleResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
	camSetWidthHeight(&cam, width, height);
}

void handleKey(GLFWwindow *window, int key, int scancode, int action,
		int mods) {
	int shiftIsDown = mods & GLFW_MOD_SHIFT;
	int controlIsDown = mods & GLFW_MOD_CONTROL;
	int altOptionIsDown = mods & GLFW_MOD_ALT;
	int superCommandIsDown = mods & GLFW_MOD_SUPER;
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_O)
			camAddTheta(&cam, -0.1);
		else if (key == GLFW_KEY_P)
			camAddTheta(&cam, 0.1);
		else if (key == GLFW_KEY_I)
			camAddPhi(&cam, -0.1);
		else if (key == GLFW_KEY_K)
			camAddPhi(&cam, 0.1);
		else if (key == GLFW_KEY_U)
			camAddDistance(&cam, -0.1);
		else if (key == GLFW_KEY_J)
			camAddDistance(&cam, 0.1);
	}
}

void initializeLight() {
	GLdouble rgb[3] = {1.0, 1.0, 1.0};
	GLdouble phi = 0.0;
	GLdouble theta = 0.0;
	lightShineFrom(&light, phi, theta);
	lightSetColor(&light, rgb);
}

void destroyMesh(void) {
	meshGLDestroy(&mesh1);
}

/* Returns 0 on success, non-zero on failure. */
int initializeShaderProgram(void) {
	GLchar vertexCode[] = "\
	    uniform mat4 viewing;\
	    attribute vec3 position;\
	    attribute vec3 normal;\
	    varying vec3 fragPos;\
	    varying vec3 normalDir;\
	    void main() {\
	        vec4 worldPos = vec4(position, 1.0);\
	        gl_Position = viewing * worldPos;\
	        fragPos = vec3(worldPos);\
	        normalDir = normal;\
	    }";
	GLchar fragmentCode[] = "\
	    uniform vec3 camPos;\
	    uniform vec3 lightCol;\
	    uniform vec3 lightDir;\
	    varying vec3 fragPos;\
	    varying vec3 normalDir;\
	    void main() {\
	    	vec3 specular = vec3(1.0, 1.0, 1.0);\
	        vec3 surfCol = vec3(0.5, 0.5, 0.5);\
	        vec3 norDir = normalize(normalDir);\
	        vec3 litDir = vec3(0.0, 0.0, 1.0);\
	        vec3 camDir = normalize(camPos - fragPos);\
	        vec3 refDir = 2.0 * dot(litDir, norDir) * norDir - litDir;\
	        float diffInt = dot(norDir, litDir);\
	        float specInt = dot(refDir, camDir);\
	        if (diffInt <= 0.0 || specInt <= 0.0)\
	            specInt = 0.0;\
	        float ambInt = 0.5;\
	        if (diffInt <= ambInt)\
	            diffInt = ambInt;\
	        vec3 diffLight = diffInt * lightCol * surfCol;\
	        float shininess = 64.0;\
	        vec3 specLight = pow(specInt, shininess) * lightCol * specular;\
	    	gl_FragColor = vec4(diffLight + specLight, 1.0);\
		}";
	program = makeProgram(vertexCode, fragmentCode);
	if (program != 0) {
		glUseProgram(program);
		attrLocs[0] = glGetAttribLocation(program, "position");
		attrLocs[2] = glGetAttribLocation(program, "normal");
		viewingLoc = glGetUniformLocation(program, "viewing");
		camPosLoc = glGetUniformLocation(program, "camPos");
		lightColLoc = glGetUniformLocation(program, "lightCol");
		lightDirLoc = glGetUniformLocation(program, "lightDir");
	}
	return (program == 0);
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	// viewing
	camRender(&cam, viewingLoc);
	GLfloat vec[3];
	vecOpenGL(3, cam.translation, vec);
	glUniform3fv(camPosLoc, 1, vec);
	// light
	lightRender(&light, lightColLoc, lightDirLoc);
	// rendering
	GLuint attrDims[3] = {3, 2, 3};
 	meshGLRender(&mesh1, 3, attrDims, attrLocs);
}

int initializeMesh() {
	// init every mesh
	meshMesh meshA;
	meshMesh meshB;
	meshMesh meshC;
	meshMesh meshD;
	if (meshInitializeBox(&meshB, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5) != 0 || meshInitializeCapsule(&meshA, 0.5, 2.0, 16, 32)
		|| 	meshInitializeBox(&meshC, -0.3, 0.3, -0.3, 0.3, -0.3, 0.3) || meshInitializeSphere(&meshD, 0.3, 16, 32)) {
	 	return 1;
	}
	GLdouble translation[3] = {0.5, 0.5, 0.5};
	GLdouble translation1[3] = {0.2, 0.2, 0.3};
	meshTranslate(&meshA, translation);
	meshTranslate(&meshD, translation1);
	// boolean operations: ((capsule intersect box) union another box) subtract sphere
	meshMesh mesh;
	CSGIntersection(&meshA, &meshB, &mesh);
	meshDestroy(&meshA);
	meshDestroy(&meshB);
	CSGUnion(&mesh, &meshC, &mesh);
	meshDestroy(&meshC);
	CSGSubtraction(&mesh, &meshD, &mesh);
	meshDestroy(&meshD);
	// openGL the produced mesh
	meshGLInitialize(&mesh1, &mesh);
	meshDestroy(&mesh);
	return 0;
}

int main(void) {
	setbuf(stdout, NULL); // for debug
    glfwSetErrorCallback(handleError);
    if (glfwInit() == 0)
        return 1;
    GLFWwindow *window;
    window = glfwCreateWindow(512, 512, "Scene Graph", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 2;
    }
    glfwSetWindowSizeCallback(window, handleResize);
    glfwSetKeyCallback(window, handleKey);
    glfwMakeContextCurrent(window);
    fprintf(stderr, "main: OpenGL %s, GLSL %s.\n", glGetString(GL_VERSION), 
    	glGetString(GL_SHADING_LANGUAGE_VERSION));
    glEnable(GL_DEPTH_TEST);
    glDepthRange(1.0, 0.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Initialize mesh
    if (initializeMesh() != 0) {
    	glfwDestroyWindow(window);
        glfwTerminate();
    	return 3;
    }
    if (initializeShaderProgram() != 0) {
    	glfwDestroyWindow(window);
        glfwTerminate();
    	return 4;
    }
    // set the cam
    GLdouble target[3] = {0.0, 0.0, 0.0};
	camSetControls(&cam, M_PI / 6.0, 10.0, 512.0, 512.0, 10.0, 
		0.0, 0.0, target);
	// set the light
	initializeLight();
	// render
    while (glfwWindowShouldClose(window) == 0) {
    	render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    // destroy
    destroyMesh();
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


