//Made by Sinan Sakaoglu, a modulus game with a twist 
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON      1    // 0:disable timer, 1:enable timer

#define D2R 0.0174532
/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

int cpscore = 0, plscore = 0;    //init. scores for player and computer
int balpoint = 0;
double timex = 60;
float angle = 0;
int num1;
int num2;
int wans[4];
int ri;
bool answered = false;
int bpoint;
bool spacebar = false;
float degbet;
int tangle;
bool rightt = false;
int tempx;
bool start = false;

// structure for holding points
typedef struct points_t {

	int x, y;

}; 
//structure for holding answer button vars.
typedef struct answers_t {

	int x, y, state;

};

//structure for holding bullet variables
typedef struct {
	points_t pos;
	float angle;
	bool active;
} fire_t;

//structure for holding target variables
typedef struct {
	points_t center;
	float radius;
} target_t;


target_t balon = { 50,50,25.0 }; 
points_t mouse;
points_t aim = { 0,0 };
answers_t answersc[] = { { 25,275,0 },{ 100,275,0 },{ 175,275,0 },{ 250,275,0 } };
fire_t fire;

//
// to draw circle, center at (x,y)
// radius r
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
	for (int i = 0; i < 600; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
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
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}
//display text with a font
void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

//
// To display onto window using OpenGL commands
//

void fireball() {
	fire.angle = angle;
	fire.active = true;
}

float findAngle() {

	float an = atan2(balon.center.y, balon.center.x - 1.0) - atan2(0, 1.0);

	an = an * 360.0 / (2 * PI);

	if (an < 0) {
		an = an + 360;
	}
	return an;

}

void question() {

	glColor3f(1, 1, 1);


	vprint2(-275, 255, 0.3, "%d %% %d = ?", num1, num2);

	for (int i = 0; i < 4; i++)
	{
		switch (answersc[i].state) {

		case 0:
			glColor3f(1, 1, 0);//still
			break;
		case 1:
			glColor3f(0, 0, 1);//hover
			break;
		case 2:
			glColor3f(1, 0, 0);//wrong answer
			break;
		case 3:
			glColor3f(0, 1, 0);//right answer
			break;
		}
		circle(answersc[i].x, answersc[i].y, 20);
		glColor3f(0, 0, 0);


		vprint2(answersc[i].x, answersc[i].y, 0.1, "%d", wans[i]);

	}
}
void shuffle(int arr[]) {

	int y = rand() % 4;
	int temp;
	temp = arr[3];
	arr[3] = arr[y];
	arr[y] = temp;
}

bool contains(int arr[], int x) {

	for (int i = 0; i < 3; i++) {
		//printf("%d == %d\n", arr[i], x);
		if (arr[i] == x)
			return true;
	}
	return false;
}

//This function resets the game 
void randomize() {
	tempx = 0;
	rightt = false;
	fire.pos = { 0,0 };
	balpoint = rand() % 5 + 1;
	balon.center.x = rand() % 280;
	balon.center.y = rand() % 300;

	if (wans[0] < 5)
		balon.center.x *= -1;
	if (wans[2] > 5)
		balon.center.y *= -1;



	for (int i = 0; i < 4; i++) // reset answer buttons
	{
		answersc[i].state = 0;
	}

	answered = false;
	num1 = rand() % 10;//get 2 random and distinct numbers
	do
	{
		num2 = rand() % 10;
	} while (num2 == 0);

	wans[3] = num1 % num2;//correct answer
	int z;
	for (int i = 0; i < 3; i++)
	{
		wans[i] = -1;
	}
	for (int i = 0; i < 3; i++)//get 3 different random answers that are incorrect and distinct
	{

		do
		{
			z = rand() % 10;
		} while (z == wans[3] || contains(wans, z));
		wans[i] = z;
	}

	shuffle(wans);

	degbet = findAngle();  //find the true angle between the target and the aim
	tangle = degbet - angle;	//translate it to fit our problem
}
//checks if bullet hit the target(balloon)
bool testCollision(fire_t fr, target_t t) {
	float dx = t.center.x - fr.pos.x;
	float dy = t.center.y - fr.pos.y;
	float d = sqrt(dx*dx + dy*dy);
	return d <= t.radius + 5;
}
//draw balloon
void balloon() {

	glColor3f(wans[0] / 10.0, wans[1] / 10.0, wans[2] / 10.0);
	circle(balon.center.x, balon.center.y, balon.radius);

	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(balon.center.x, balon.center.y - balon.radius + 5);
	glVertex2f(balon.center.x - 15, balon.center.y - balon.radius - 55);
	glVertex2f(balon.center.x, balon.center.y - balon.radius + 5);
	glVertex2f(balon.center.x + 15, balon.center.y - balon.radius - 55);
	glEnd();


	glColor3f(1, 102 / 255.0, 0);
	glBegin(GL_QUADS);//basket
	glVertex2f(balon.center.x - 15, balon.center.y - balon.radius - 55);
	glVertex2f(balon.center.x - 15, balon.center.y - balon.radius - 85);
	glVertex2f(balon.center.x + 15, balon.center.y - balon.radius - 85);
	glVertex2f(balon.center.x + 15, balon.center.y - balon.radius - 55);
	glEnd();

	glColor3f(1, 1, 204 / 255.0);
	circle(balon.center.x, balon.center.y - balon.radius - 55, 4);


	glColor3f(1, 1, 1);
	vprint2(balon.center.x, balon.center.y - 16, 0.2, "%d", balpoint);


}



void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (start)
	{


		//time, computer and player score
		glColor3f(1, 1, 1);
		vprint(-275, -275, GLUT_BITMAP_9_BY_15, "Time: %.2f", timex);
		timex -= 0.01;//decrease time
					  // if (time<0) time = 60.0;//reset time
		vprint(-100, -275, GLUT_BITMAP_9_BY_15, "Computer =  %d", cpscore);

		vprint(125, -275, GLUT_BITMAP_9_BY_15, "Player =  %d", plscore);

		//Gradient background
		glBegin(GL_QUADS);
		glVertex2f(-300, 250);
		glVertex2f(300, 250);
		glColor3f(0.7, 0.7, 1);//blue
		glVertex2f(300, -250);
		glVertex2f(-300, -250);
		glEnd();

		glColor3f(0.5, 0.5, 0.5);//gray line
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(-300, 0);
		glVertex2f(300, 0);
		glEnd();

		circle(0, 0, 30);
		glColor3f(0.3, 0.3, 0.3);//ball outline
		circle_wire(0, 0, 30);

		if (angle > 360) {
			angle = 0;
		}
		else if (angle < 0) { angle = 360; }



		//printf("fire angle = %.f, degbet = %.f, f+d = %.f \n", fire.angle, degbet, fire.angle + degbet);
		if ((int)degbet != (int)angle)
		{
			if (tangle < 0)
			{
				angle--;
			}
			else angle++;
		}
		else { fireball(); }


		glColor3f(1, 1, 1);//fire line
		aim.x = 25 * cos((int)(angle) % 360 * D2R);
		aim.y = 25 * sin((int)(angle) % 360 * D2R);
		glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(aim.x, aim.y);
		glEnd();
		//display angle in yellow
		glColor3f(1, 1, 0);
		vprint(0, -10, GLUT_BITMAP_9_BY_15, "%.f", angle);


		question();
		balloon();

		if (fire.active) //bullet
		{


			glBegin(GL_TRIANGLES);
			glColor3f(1, 0, 0);
			glVertex2f(fire.pos.x, fire.pos.y + 10);

			glColor3f(0, 1, 0);
			glVertex2f(fire.pos.x - 10, fire.pos.y - 10);

			glColor3f(0, 0, 1);
			glVertex2f(fire.pos.x + 10, fire.pos.y - 10);


			glEnd();


			//circle(fire.pos.x, fire.pos.y, 5);
		}



		if (rightt)//if answer is correct, float the balloon away from screen
		{
			balon.center.y += 2;
			balon.center.x = 8 * cos(abs(balon.center.y) % 360 * D2R) + tempx;
		}
	}
	else
		vprint(-150, 0, GLUT_BITMAP_9_BY_15, "Press any key to start");
	if (timex < 0.1)//when timer ends, finish the game
	{
		start = false;
	}
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
	cpscore = 0; //press any key to start sequence
	plscore = 0;
	timex = 60;
	randomize();
	start = true;

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
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
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
	// Write your codes here.
	for (int i = 0; i < 4; i++)  //whenever a mouse click registers, chek if it was on any answer buttons
	{
		if (answersc[i].state == 1 && !answered)//check if the button is clicked and if we already answered
		{
			if (wans[i] == num1%num2)//check if it is the correct answer
			{
				answersc[i].state = 3;
				tempx = balon.center.x;
				rightt = true;
			}
			else {
				answersc[i].state = 2;
				rightt = false;
			}
			answered = true;
		}
	}


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
	// glutPostRedisplay() ;
}

void onMove(int x, int y) {
	// Write your codes here.
	mouse.x = x - winWidth / 2;
	mouse.y = winHeight / 2 - y;

	for (int i = 0; i < 4; i++)
	{
		if (answersc[i].state != 2 && answersc[i].state != 3)
		{
			answersc[i].state = 0;
		}

	}
	for (int i = 0; i < 4; i++)
	{
		if ((mouse.x<answersc[i].x + 20 && mouse.x>answersc[i].x - 20) && (mouse.y<answersc[i].y + 20 && mouse.y>answersc[i].y - 20) && (answersc[i].state != 2 && answersc[i].state != 3))
		{
			answersc[i].state = 1;
		}
	}




	//printf("%d %d \n", mouse.x, mouse.y);

	// to refresh the window it calls display() function
	//glutPostRedisplay() ;
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (fire.active)//if bullet is fired
	{

		fire.pos.x += 10 * cos(fire.angle * D2R);//move bullet towards balloon's initial pos
		fire.pos.y += 10 * sin(fire.angle * D2R);

		if (fire.pos.x > 300 || fire.pos.x < -300 || fire.pos.y>300 || fire.pos.y < -300) {//if bullet is out of screen stop
			fire.active = false;


		}

		if (testCollision(fire, balon)) {//if balloon is hit add point to pc and create a new question
			fire.active = false;
			//resetTarget();

			cpscore += balpoint;
			randomize();
		}

	}
	if (balon.center.y > 310)
	{
		fire.active = false;
		plscore += balpoint;
		randomize();
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand((unsigned int)time(NULL));
	randomize();
}


void main(int argc, char *argv[]) {


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Template File");

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