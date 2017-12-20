#include "main.h"
#include "shader.h"
#include "model.h"
#include "light.h"
#include "eye.h"

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

vector<Scene*> globe;
vector<Light*> light;

Eye *eye;
Mouse *mouse;
Keyboard *key;
extern Shader *elementShader;
extern Shader *texShader;
extern Shader *shadowShader;

void renderGlobe() {
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(45.f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = glm::lookAt(eye->pos, eye->pos + eye->dir, glm::vec3(0.0, 1.0, 0.0));
	elementShader->use();
	elementShader->setMat4("u_projection", projection);
	elementShader->setMat4("u_view", view);
	elementShader->setVec3("u_eyePos", eye->pos);
	for (int i = 0; i < light.size(); i++) {
		char tmp[64];
		sprintf(tmp, "u_lightsMatrix[%d]", i);
		elementShader->setMat4(tmp, light[i]->lightMatrix);
		sprintf(tmp, "u_lightInfo[%d].u_lightPos", i);
		elementShader->setVec3(tmp, light[i]->pos);
		sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
		elementShader->setVec3(tmp, light[i]->diffuse);
		sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
		elementShader->setVec3(tmp, light[i]->ambient);
		sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
		elementShader->setFloat(tmp, light[i]->specular);
	}
	texShader->use();
	texShader->setMat4("u_projection", projection);
	texShader->setMat4("u_view", view);
	texShader->setVec3("u_eyePos", eye->pos);
	for (int i = 0; i < light.size(); i++) {
		char tmp[64];
		sprintf(tmp, "u_lightsMatrix[%d]", i);
		texShader->setMat4(tmp, light[i]->lightMatrix);
		sprintf(tmp, "u_lightInfo[%d].u_lightPos", i);
		texShader->setVec3(tmp, light[i]->pos);
		sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
		texShader->setVec3(tmp, light[i]->diffuse);
		sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
		texShader->setVec3(tmp, light[i]->ambient);
		sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
		texShader->setFloat(tmp, light[i]->specular);
	}
	int ln = light.size();
	for (int i = 0; i < ln; i++) {
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D, light[i]->textureHandle);
	}
	for (auto s : globe) {
		s->show();
	}
}

static void GLFInit() {
	glewInit();
	glEnable(GL_DEPTH_TEST);
	elementShader = new Shader("shader//element.vert", "shader//element.frag");
	texShader = new Shader("shader//tex.vert", "shader//tex.frag");
	shadowShader = new Shader("shader//shadow.vert", "shader//shadow.frag");

	eye = new Eye(glm::vec3(0.0f, 0.0f, 8.0f));
	mouse = new Mouse();
	key = new Keyboard();
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
}
static void GLFDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	elementShader->use();
	elementShader->setInt("u_lightNum", light.size());
	texShader->use();
	texShader->setInt("u_lightNum", light.size());
	for (auto &l : light) {
		l->genShadow();
	}
	renderGlobe();

	glutSwapBuffers();
}
static void GLFReshape(int w, int h) {
	SCR_WIDTH = w;
	SCR_HEIGHT = h;
}
static void GLFMouse(int x, int y) {
	mouse->tmp.x = float(x);
	mouse->tmp.y = float(y);

	glutPostRedisplay();
}
static void GLFDrag(int x, int y) {
	if (mouse->state[GLUT_LEFT_BUTTON] == GLUT_DOWN)
		eye->rotate(float(x - mouse->pre.x) / 200, float(y - mouse->pre.y) / 200);
	if (mouse->state[GLUT_RIGHT_BUTTON] == GLUT_DOWN)
		eye->move(float(x - mouse->pre.x) / 2, float(y - mouse->pre.y) / 2);

	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	glutPostRedisplay();
}
static void GLFClick(int button, int state, int x, int y) {
	mouse->state[button] = state;
	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN) {
		mouse->state[GLUT_LEFT_BUTTON] = GLUT_DOWN;
	}

	glutPostRedisplay();
}
static void GLFWheel(int wheel, int dir, int x, int y) {
	if (dir > 0)eye->zoom(.8f);
	if (dir < 0)eye->zoom(1.2f);

	glutPostRedisplay();
}
static void GLFSpecialDown(int key, int x, int y) {
	if (key == GLF_KEY_CTRL)::key->ctrl = true;
	if (key == GLF_KEY_SHIFT)::key->shift = true;
	if (key == GLF_KEY_ALT)::key->alt = true;
}
static void GLFSpecialUp(int key, int x, int y) {
	if (key == GLF_KEY_CTRL)::key->ctrl = false;
	if (key == GLF_KEY_SHIFT)::key->shift = false;
	if (key == GLF_KEY_ALT)::key->alt = false;
}
static void GLFKeyDown(unsigned char cAscii, int x, int y) {

}
static void GLFKeyUp(unsigned char cAscii, int x, int y) {

}

void build() {
	static Scene tmp;
	tmp.elements.push_back(*Element(true, glm::vec3(1.f), glm::vec3(1.2f)).load("source//castle.obj"));
	tmp.elements.push_back(*Element().load("source//ground.obj"));
	tmp.textures.push_back(*Texture().load("source//pic.obj"));
	globe.push_back(&tmp);
	light.push_back(new Light(0, glm::vec3(-24.f, 32.f, 18.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), .6f));
	light.push_back(new Light(1, glm::vec3(24.f, 20.f, -18.f), glm::vec3(.3f, .3f, .3f), glm::vec3(.1f, .1f, .1f), .4f));
	light.push_back(new Light(2, glm::vec3(24.f, 32, 10.f), glm::vec3(.2f, .2f, .2f), glm::vec3(.0f, .0f, .0f), .4f));
}
int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("OpenGL Frame --SG Creator");
	GLFInit();

	glutDisplayFunc(GLFDisplay);
	glutReshapeFunc(GLFReshape);
	glutPassiveMotionFunc(GLFMouse);
	glutMotionFunc(GLFDrag);
	glutMouseFunc(GLFClick);
	glutMouseWheelFunc(GLFWheel);
	glutKeyboardFunc(GLFKeyDown);
	glutKeyboardUpFunc(GLFKeyUp);
	glutSpecialFunc(GLFSpecialDown);
	glutSpecialUpFunc(GLFSpecialUp);

	build();
	glutMainLoop();
	return 0;
}
