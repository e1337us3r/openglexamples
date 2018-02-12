//Written by Sinan Sakaoglu, A free falling ball simulation. Features 2d graphing, freefall physics and some special glut features
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define  TIMER_PERIOD  16 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define DOT_NUM 17
#define WDOT_NUM 40

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
float hitDistance;
typedef struct 
{
	float x=0, y=-350;
	
}points_t; //structure for storing 2D points
typedef struct 
{
	int startX, endX;
		float a, b;
}lines_t; //structure for storing 2D lines
lines_t linez[16];
lines_t lineP;
points_t gpoint[DOT_NUM]; //store randomized green points
points_t wpoint[WDOT_NUM]; //store randomized white points
points_t ball;
points_t hit;

int lineIndex; //index of the line that intersects with the ball
float speed=0; //speed of the ball

//
// to draw circle, center at (x,y)
//  radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}


// display text with variables.
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

//create randomized green points ranging from x=-400 to x=400
void gpoints()
{
	int a = 0;
	
	for (int i = -400;  i <= 401;  i+=50,a++)
	{
		gpoint[a].y = -(rand() % 200*1.0);
		gpoint[a].x = i*1.0;		
	}
}
//Connect the dots on the graph
void condots()
{
	glLineWidth(3);
	glColor3f(1, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < DOT_NUM; i++)
	{
		glVertex2f(gpoint[i].x, gpoint[i].y);
	}
	glEnd();
}
//Record info's of lines between random green dots
void findLines()
{
	for (int i = 0; i < 17; i++)
	{
		linez[i].startX = gpoint[i].x;
		linez[i].endX = gpoint[i+1].x;
		linez[i].a = ((gpoint[i + 1].y - gpoint[i].y) / (gpoint[i + 1].x - gpoint[i].x)*1.0);
		linez[i].b = (gpoint[i].y*1.0 - (linez[i].a*gpoint[i].x));
	}
}
//Record info's of white lines directly under green dots
void wpoints()
{
	
	int wp=0,z=0;
	for (int i = -380; i < 401; i += 20,wp++)
	{	
		wpoint[wp].x = i;
		if (i >= linez[z].endX)
			z++;
		wpoint[wp].y= ((wpoint[wp].x*linez[z].a) + linez[z].b);		
	}
}
//find intersecting line
void findLineIndex()
{
	lineIndex = 0;
	while (ball.x > linez[lineIndex].startX)
		lineIndex++;
	lineIndex--;
}
//find the perpendicular line info at intersection
void findPline()
{ 
	lineP.a = linez[lineIndex].a * (-1);
	lineP.b = (ball.y*1.0 - (lineP.a*ball.x));
}

void hitDist()
{
	
	
	//find intersection point
	hit.x = (linez[lineIndex].b - lineP.b) / (lineP.a - linez[lineIndex].a);
	hit.y = lineP.a*hit.x + lineP.b;
	//find where is the intersection relative to the line and calculate distance between ball and int. point.

	if (fabs(linez[lineIndex].a)<0.2)
	{
		hitDistance = (sqrtf(powf((ball.y - (ball.x*linez[lineIndex].a + linez[lineIndex].b)), 2)));
	}
	else if (ball.x >= linez[lineIndex].endX)
	{
		hitDistance = (sqrtf(powf((ball.x - linez[lineIndex].endX), 2) + powf((ball.y - (linez[lineIndex].endX*linez[lineIndex].a + linez[lineIndex].b)), 2)));
		
	}
	else if (ball.x <= linez[lineIndex].startX)
	{
		hitDistance = (sqrtf(powf((ball.x- linez[lineIndex].startX),2)+powf((ball.y- (linez[lineIndex].startX*linez[lineIndex].a+ linez[lineIndex].b)),2)));
		
	}
	else
	{
		
		hitDistance = (sqrtf(powf((ball.x - hit.x), 2) + powf((ball.y - hit.y), 2)));
		
	}
}


void display()
{

	//
	// clear window to black
	//
	//glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//draw background
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex2f(-800, 600);
	glVertex2f(800, 600);

	glColor3f(0, 0, 1);
	glVertex2f(800, -600);
	glVertex2f(-800, -600);
	glEnd();



	
	condots();
	int i;
	glColor3f(1, 1, 1);
	vprint(-390, 270, GLUT_BITMAP_9_BY_15, "SINAN SAKAOGLU");
	vprint(-390, 250, GLUT_BITMAP_9_BY_15, "21602739");
	vprint(-390, 230, GLUT_BITMAP_9_BY_15, "HW #2");
	glLineWidth(1);
	//draw white lines
	for (i = 0; i < WDOT_NUM; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(wpoint[i].x, -300);
		glVertex2f(wpoint[i].x, wpoint[i].y);
		glEnd();
	}
	//draw white dots
	for (i = 0; i < WDOT_NUM; i++)
	{
		circle(wpoint[i].x, wpoint[i].y, 3);
		
	}
	glColor3f(0, 1, 0);
	//draw green dots
	for (i = 0; i < DOT_NUM; i++)
	{
		circle(gpoint[i].x, gpoint[i].y, 5);
	}

	//draw the ball
	circle(ball.x, ball.y, 25);
	glColor3f(1, 1, 1);
	circle_wire(ball.x, ball.y, 25);
	
	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{	
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	if (key == GLUT_KEY_F11)
	{
		gpoints();   // reset the points
		findLines(); //reset line information
		wpoints();   //randomize new points
		ball.y = -350;  //move ball off screen
		
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}


//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}


//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	//Translate the coords. so that 0,0 is middle of screen.
	ball.x = x - winWidth / 2;
	ball.y = winHeight / 2-y;

	
	speed = 0;
	findLineIndex(); // find which of the lines will the ball fall 
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function	
	glutPostRedisplay();
}

void onMove(int x, int y) {
	// Write your codes here.
	
	

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	findPline();
	hitDist();
	if (hitDistance>30.0)
	{
		speed += 0.5;
		ball.y -= speed;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gpoints();
	findLines();
	wpoints();
	
	

}


void main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Hitbox Simulation");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();
	

	glutMainLoop();
	
}