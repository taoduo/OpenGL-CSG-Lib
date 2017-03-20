/**
 * Duo Tao
 * light.c
 * Directional Light.
 */

typedef struct lightLight lightLight;
struct lightLight {
	GLdouble color[3];
	GLdouble rotation[3][3];
};

void lightSetRotation(lightLight *light, GLdouble rot[3][3]) {
	vecCopy(9, (GLdouble *)rot, (GLdouble *)(light->rotation));
}

void lightSetColor(lightLight *light, GLdouble rgb[3]) {
	vecCopy(3, rgb, light->color);
}

/* Sets the light's direction. */
void lightShineFrom(lightLight *light, GLdouble phi, GLdouble theta) {
	GLdouble negZ[3], y[3];
	GLdouble yStd[3] = {0.0, 1.0, 0.0}, negZStd[3] = {0.0, 0.0, -1.0};
	vec3Spherical(1.0, phi, theta, negZ);
	vec3Spherical(1.0, M_PI / 2.0 - phi, theta + M_PI, y);
	mat33BasisRotation(yStd, negZStd, y, negZ, light->rotation);
}

/* The '...Loc' arguments are shader locations. This function loads those 
locations with the light's settings. cosLoc gets the cosine of half of the 
spot light angle. */
void lightRender(lightLight *light, GLint colorLoc, GLint dirLoc) {
	GLfloat vec[3];
	vecOpenGL(3, light->color, vec);
	glUniform3fv(colorLoc, 1, vec);
	vec[0] = -light->rotation[0][2];
	vec[1] = -light->rotation[1][2];
	vec[2] = -light->rotation[2][2];
	glUniform3fv(dirLoc, 1, vec);
}


