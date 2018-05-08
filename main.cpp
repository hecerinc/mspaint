#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
using namespace std;

typedef struct Color {
	int r, g, b;
	float alpha;
	Color() {
		r = g = b = 255;
		alpha = 1.0;
	}
	Color(int r, int g, int b) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->alpha = 1.0;
	}
	Color(int r, int g, int b, float a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->alpha = a;
	}
} Color;

typedef struct Square {
	int tl[2];
	int br[2];
	Color color;
	int origPos[4];
	Square(int x1, int y1, int x2, int y2) {
		tl[0] = x1;
		tl[1] = y1;
		br[0] = x2;
		br[1] = y2;
		origPos[0] = origPos[1] = origPos[2] = origPos[3] = -1;
		this->color = Color(255,0,0); // red
	}
	void fixOrigPos() {
		origPos[0] = tl[0];
		origPos[1] = tl[1];
		origPos[2] = br[0];
		origPos[3] = br[1];
	}
} Square;


typedef struct Mouse {
	int xpressed;
	int ypressed;
	int x; // current position
	int y; // current position
	int selected; // selected Object
	Mouse() {
		xpressed = ypressed = x = y = 0;
		selected = -1;
	}
} Mouse;

enum Tool {SELECT, CREATE, SCALE, DELETE};

Tool currentTool = DELETE;

Mouse mouse;

vector<Square> objects;


float red = 1.0;
float green = 1.0;
float blue = 1.0;
const int width = 1000;
const int height = 800;
int init_pos_x = 0;
int init_pos_y = 0;
bool mouseDidMove = false;


unsigned char scaleMatrix[width][height];


void drawRectAux(Square &s) {
	glColor3f(s.color.r/255.0, s.color.g/255.0, s.color.b/255.0);
	glBegin(GL_QUADS);
		glVertex2i(s.tl[0], s.tl[1]);
		glVertex2i(s.br[0], s.tl[1]);
		glVertex2i(s.br[0], s.br[1]);
		glVertex2i(s.tl[0], s.br[1]);
	glEnd();
}

void drawRectangle(int x, int y) { 
	glColor3f(1,0,0);

	// printf("---------------------------\n");
	// printf("init pos: (%d, %d)\n", init_pos_x, init_pos_y);
	glBegin(GL_QUADS);
		glVertex2i(init_pos_x, init_pos_y);
		glVertex2i(x, init_pos_y);
//		glColor3f(0,0,1);
		glVertex2i(x, y);
		glVertex2i(init_pos_x, y);
	glEnd();
}
bool checkBoundingBox(size_t index, int x, int y) {
	const Square * a = &objects[index];
	int min_x = min(a->tl[0], a->br[0]);
	int min_y = min(a->tl[1], a->br[1]);
	int max_x = max(a->tl[0], a->br[0]);
	int max_y = max(a->tl[1], a->br[1]);

	return x >= min_x && x <= max_x && y <= max_y && y >= min_y;
}
void setSelectedObject(int x, int y) {
	const size_t len = objects.size();
	for (int i = len-1; i >= 0; i--) {
		if(checkBoundingBox(i, x, y)) {
			mouse.selected = i;
			break;
		}
	}
}
void deleteSquare(int index) {
	objects.erase(objects.begin() + index);
}
void handleMouseClick(int button, int state, int x, int y) {
	if(button != GLUT_LEFT_BUTTON)
		return;
	if(state == GLUT_DOWN) {
		printf("button pressed at (%d, %d)\n", x, y);	
		int mod = glutGetModifiers();
		setSelectedObject(x, y);
		if(mod == GLUT_ACTIVE_CTRL) {
			if(mouse.selected > -1) {
				objects[mouse.selected].color = {0, 0, 255};
				glutPostRedisplay();
			}
		}
		else {
			// only store the initial point
			init_pos_x = x;
			init_pos_y = y;
			mouse.xpressed = x;
			mouse.ypressed = y;
			if(mouse.selected > -1) {
				objects[mouse.selected].fixOrigPos();
			}

		}

	}
	else {
		printf("button released at (%d, %d)\n", x, y);	
		// Store the new square in our list of objects
		if(currentTool == CREATE && mouseDidMove) {
			Square s(init_pos_x, init_pos_y, x, y);
			objects.push_back(s);
		}
		if(currentTool == DELETE && !mouseDidMove && mouse.selected > -1) {
			deleteSquare(mouse.selected);
			glutPostRedisplay();
		}
		mouseDidMove = false;
		mouse.selected = -1;
		printf("SQUARES: %d\n", objects.size());

//		drawRectangle(x,y);
//		glutSwapBuffers();
//		glutPostRedisplay();

	} 
}

// Handle when the mouse is moving and a button is clicked
void handleMouseMove(int x, int y) {
	mouseDidMove = true;
	// printf("Mouse position: (%d, %d)\n",x, y); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	if(currentTool == CREATE) {
		for(vector<Square>::iterator it = objects.begin(); it != objects.end(); ++it) {
			drawRectAux(*it);
		}
		drawRectangle(x, y);
		glutSwapBuffers();
	}
	else if(currentTool == SELECT) {
		// Translate
		if(mouse.selected > -1) {
			int dx = x - mouse.xpressed;
			int dy = y - mouse.ypressed;
			Square * const selectedObj = &objects[mouse.selected];

			// TODO: maybe shift this to translate() func
			selectedObj->tl[0] = selectedObj->origPos[0] + dx;
			selectedObj->tl[1] = selectedObj->origPos[1] + dy;
			selectedObj->br[0] = selectedObj->origPos[2] + dx;
			selectedObj->br[1] = selectedObj->origPos[3] + dy;

			for(vector<Square>::iterator it = objects.begin(); it != objects.end(); ++it) {
				drawRectAux(*it);
			}
			glutSwapBuffers();
		}
		mouse.x = x;
		mouse.y = y;
	}

	
//	glutPostRedisplay();	
}

void fillScaleMatrix() {
	for(vector<Square>::iterator it = objects.begin(); it != objects.end(); ++it) {
		// Top left
		int min_x = min(it->tl[0], it->br[0]);
		int min_y = min(it->tl[1], it->br[1]);
		int max_x = max(it->tl[0], it->br[0]);
		int max_y = max(it->tl[1], it->br[1]);

		// TODO: Check that the figure is large enough
		// In clockwise fashion
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				scaleMatrix[min_x+i][min_y+j] = 1; // top left
				scaleMatrix[max_x-j][min_y+i] = 2; // top right
				scaleMatrix[max_x-j][max_y-i] = 3; // bottom right
				scaleMatrix[min_x+j][max_y-i] = 4; // bottom right
			}
		}




	}
}
void handleMousePassive(int x, int y) {
	if(currentTool != SCALE)
		return;
	switch(scaleMatrix[x][y]) {
		case 1:
			glutSetCursor(GLUT_CURSOR_TOP_LEFT_CORNER);
			break;
		case 2:
			glutSetCursor(GLUT_CURSOR_TOP_RIGHT_CORNER);
			break;
		case 3:
			glutSetCursor(GLUT_CURSOR_BOTTOM_RIGHT_CORNER);
			break;
		case 4:
			glutSetCursor(GLUT_CURSOR_BOTTOM_LEFT_CORNER);
			break;
		default: 
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

	}
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

	// glutSetCursor(GLUT_CURSOR_BOTTOM_LEFT_CORNER);
	// Add two squares
	objects.push_back(Square(20,20, 220, 220));
	objects.push_back(Square(20,320, 220, 520));
	objects.push_back(Square(750,600, 790, 650));
	objects[0].color = {0, 43, 54};
	objects[1].color = {0, 43, 54};
	objects[2].color = {0, 43, 54};

	if(currentTool == SCALE) {
		memset(scaleMatrix, 0, sizeof(scaleMatrix));
		fillScaleMatrix();
	}
	else if(currentTool == DELETE) {
		glutSetCursor(GLUT_CURSOR_DESTROY);
	}
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
	glutPassiveMotionFunc(handleMousePassive);
	glutMotionFunc(handleMouseMove);	
//	glutIdleFunc(renderScene);
	

	
	glutMainLoop();
	return 1;
	
}


