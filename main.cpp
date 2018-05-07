#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>


using namespace std;

typedef struct Square {
	int tl[2];
	int br[2];
	Square(int x1, int y1, int x2, int y2) {
		tl[0] = x1;
		tl[1] = y1;
		br[0] = x2;
		br[1] = y2;
	}
} Square;



vector<Square> objects;


float red = 1.0;
float green = 1.0;
float blue = 1.0;
int width = 1000;
int height = 800;
int init_pos_x = 0;
int init_pos_y = 0;

void drawRectAux(Square &s) {
	
	glBegin(GL_QUADS);
		glVertex2i(s.tl[0], s.tl[1]);
		glVertex2i(s.br[0], s.tl[1]);
		glVertex2i(s.br[0], s.br[1]);
		glVertex2i(s.tl[0], s.br[1]);
	glEnd();
}

void drawRectangle(int x, int y) { 
	glColor3f(1,0,0);		

	printf("---------------------------\n");
	printf("init pos: (%d, %d)\n", init_pos_x, init_pos_y);
	glBegin(GL_QUADS);
		glVertex2i(init_pos_x, init_pos_y);
		glVertex2i(x, init_pos_y);
//		glColor3f(0,0,1);
		glVertex2i(x, y);
		glVertex2i(init_pos_x, y);
	glEnd();
}
void handleMouseClick(int button, int state, int x, int y) {
	if(button != GLUT_LEFT_BUTTON)
		return;
	if(state == GLUT_DOWN) {
		printf("button pressed at (%d, %d)\n", x, y);	
		// only store the initial point
		init_pos_x = x;
		init_pos_y = y;
	}
	else {
		printf("button released at (%d, %d)\n", x, y);	
		// Store the new square in our list of objects
		Square s(init_pos_x, init_pos_y, x, y);
		objects.push_back(s);

//		drawRectangle(x,y);
//		glutSwapBuffers();
//		glutPostRedisplay();

	} 
}

// Handle when the mouse is moving and a button is clicked
void handleMouseMove(int x, int y) {
	printf("Mouse position: (%d, %d)\n",x, y); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(vector<Square>::iterator it = objects.begin(); it != objects.end(); ++it) {
		drawRectAux(*it);
	}

	drawRectangle(x, y);
	glutSwapBuffers();
	
//	glutPostRedisplay();	
}





void render() {
	glClearColor(red,green,blue,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations  
	glLoadIdentity();
//	drawRectangle(200, 200);
	for(vector<Square>::iterator it = objects.begin(); it != objects.end(); ++it) {
		drawRectAux(*it);
	}
//	gluLookAt(0,0,10.0f,0,0,0,0,1.0f,0);
	glutSwapBuffers();
}
void setup() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	// Allows setting coordinates in clockwise order
	glFrontFace(GL_CW);
}
int main(int argc, char ** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width,height);
	glutCreateWindow("MS Paint");
	setup();


	// register callbacks
	glutDisplayFunc(render);
	glutMouseFunc(handleMouseClick);
	glutMotionFunc(handleMouseMove);	
//	glutIdleFunc(renderScene);
	

	
	glutMainLoop();
	return 1;
	
}


