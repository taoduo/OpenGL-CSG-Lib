/**
 * Duo Tao
 * camera.c
 * Perpective Projection.
 */

typedef struct camCamera camCamera;
struct camCamera {
	/* Low-level interface. */
	GLdouble rotation[3][3];
	GLdouble translation[3];
	GLdouble projection[6];
	/* High-level interface. */
	GLdouble fovy, ratio, width, height;
	GLdouble distance;
	GLdouble phi, theta;
	GLdouble target[3];
};



/*** Low-level interface ***/

#define camPROJL 0
#define camPROJR 1
#define camPROJB 2
#define camPROJT 3
#define camPROJF 4
#define camPROJN 5

void camSetRotation(camCamera *cam, GLdouble rot[3][3]) {
	vecCopy(9, (GLdouble *)rot, (GLdouble *)(cam->rotation));
}

void camSetTranslation(camCamera *cam, GLdouble transl[3]) {
	vecCopy(3, transl, cam->translation);
}

void camSetProjection(camCamera *cam, GLdouble proj[6]) {
	vecCopy(6, proj, cam->projection);
}

void camSetOneProjection(camCamera *cam, GLuint i, GLdouble value) {
	cam->projection[i] = value;
}

/* Sets the camera's rotation and translation, in a manner suitable for third-
person viewing. */
void camLookAt(camCamera *cam, GLdouble target[3], GLdouble rho, GLdouble phi, 
		GLdouble theta) {
	GLdouble z[3], y[3], yStd[3] = {0.0, 1.0, 0.0}, zStd[3] = {0.0, 0.0, 1.0};
	vec3Spherical(1.0, phi, theta, z);
	vec3Spherical(1.0, M_PI / 2.0 - phi, theta + M_PI, y);
	mat33BasisRotation(yStd, zStd, y, z, cam->rotation);
	vecScale(3, rho, z, cam->translation);
	vecAdd(3, target, cam->translation, cam->translation);
}

/* Sets the camera's rotation and translation, in a manner suitable for first-
person viewing. */
void camLookFrom(camCamera *cam, GLdouble position[3], GLdouble phi, 
		GLdouble theta) {
	GLdouble negZ[3], y[3];
	GLdouble yStd[3] = {0.0, 1.0, 0.0}, negZStd[3] = {0.0, 0.0, -1.0};
	vec3Spherical(1.0, phi, theta, negZ);
	vec3Spherical(1.0, M_PI / 2.0 - phi, theta + M_PI, y);
	mat33BasisRotation(yStd, negZStd, y, negZ, cam->rotation);
	vecCopy(3, position, cam->translation);
}

/* Sets the six projection parameters, based on the 
width and height of the viewport and three other parameters.  */
void camSetFrustum(camCamera *cam, GLdouble fovy, 
		GLdouble focal, GLdouble ratio, GLdouble width, GLdouble height) {
	cam->projection[camPROJF] = -focal * ratio;
	cam->projection[camPROJN] = -focal / ratio;
	cam->projection[camPROJT] = -cam->projection[camPROJN] 
		* tan(fovy * 0.5);
	cam->projection[camPROJB] = -cam->projection[camPROJT];
	cam->projection[camPROJR] = cam->projection[camPROJT] * width / height;
	cam->projection[camPROJL] = -cam->projection[camPROJR];
}

/* viewingLoc is a shader location for a uniform 4x4 matrix. This function 
loads that location with the camera's inverse isometry and projection --- that 
is, P C^-1, in the notation of our software graphics engine. */
void camRender(camCamera *cam, GLint viewingLoc) {
	GLfloat viewing[4][4];
	double camInv[4][4], proj[4][4], projCamInv[4][4];
	mat44InverseIsometry(cam->rotation, cam->translation, camInv);
	mat44Perspective(cam->projection[camPROJL], cam->projection[camPROJR],
		cam->projection[camPROJB],cam->projection[camPROJT],cam->projection[camPROJF],
		cam->projection[camPROJN], proj);
	mat444Multiply(proj, camInv, projCamInv);
	mat44OpenGL(projCamInv, viewing);
	glUniformMatrix4fv(viewingLoc, 1, GL_FALSE, (GLfloat *)viewing);
}



/*** High-level interface ***/
void camSetControls(camCamera *cam, GLdouble fovy, 
		GLdouble ratio, GLdouble width, GLdouble height, GLdouble distance, 
		GLdouble phi, GLdouble theta, GLdouble target[3]) {
	cam->fovy = fovy;
	cam->ratio = ratio;
	cam->width = width;
	cam->height = height;
	cam->distance = distance;
	cam->phi = phi;
	cam->theta = theta;
	vecCopy(3, target, cam->target);
	camSetFrustum(cam, cam->fovy, cam->distance, 
		cam->ratio, cam->width, cam->height);
	camLookAt(cam, cam->target, cam->distance, cam->phi, cam->theta);
}

void camAddFovy(camCamera *cam, GLdouble delta) {
	cam->fovy += delta;
	camSetFrustum(cam, cam->fovy, cam->distance, 
		cam->ratio, cam->width, cam->height);
}

void camAddRatio(camCamera *cam, GLdouble delta) {
	cam->ratio += delta;
	camSetFrustum(cam, cam->fovy, cam->distance, 
		cam->ratio, cam->width, cam->height);
}

void camSetWidthHeight(camCamera *cam, GLdouble width, GLdouble height) {
	cam->width = width;
	cam->height = height;
	camSetFrustum(cam, cam->fovy, cam->distance, 
		cam->ratio, cam->width, cam->height);
}

void camAddDistance(camCamera *cam, GLdouble delta) {
	cam->distance += delta;
	camSetFrustum(cam, cam->fovy, cam->distance, 
		cam->ratio, cam->width, cam->height);
	camLookAt(cam, cam->target, cam->distance, cam->phi, cam->theta);
}

void camAddPhi(camCamera *cam, GLdouble delta) {
	cam->phi += delta;
	camLookAt(cam, cam->target, cam->distance, cam->phi, cam->theta);
}

void camAddTheta(camCamera *cam, GLdouble delta) {
	cam->theta += delta;
	camLookAt(cam, cam->target, cam->distance, cam->phi, cam->theta);
}

void camSetTarget(camCamera *cam, GLdouble target[3]) {
	vecCopy(3, target, cam->target);
	camLookAt(cam, cam->target, cam->distance, cam->phi, cam->theta);
}


