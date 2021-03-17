#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sokoban.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLfloat vertices[][3] = { {-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };

GLfloat normals[][3] = { {-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
	{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
	{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };


typedef struct cubo* Cubo;
struct cubo {
	double x, y, z; //initial coord
	double x1, y1, z1; //final pos
	double dx, dy;
	char c;	//* # @ $
};


Cubo** aCubos;
int timeUpdate = 10;


double rotX = -60, rotY = 1;
NiveisTab niveis;
int iNivel = 0;
int startGame = 0, playerlost = 0, levelUp = 0;
int movesMade, boxLeft;
double timer = 0;
int i, j;
const int font = (int)GLUT_BITMAP_TIMES_ROMAN_24;


void copyToaCubos();

void iText(double x, double y, char* str, void* font) {
	glRasterPos3d(x, y, 0);
	int i;
	for (i = 0; str[i]; i++) {
		glutBitmapCharacter(font, str[i]);
	}
}
void polygon(int a, int b, int c, int d)
{

	/* draw a polygon via list of vertices */
	glBegin(GL_POLYGON);
	//		glColor3fv(colors[a]);
	glNormal3fv(normals[a]);
	glVertex3fv(vertices[a]);
	//		glColor3fv(colors[b]);
	glNormal3fv(normals[b]);
	glVertex3fv(vertices[b]);
	//		glColor3fv(colors[c]);
	glNormal3fv(normals[c]);
	glVertex3fv(vertices[c]);
	//		glColor3fv(colors[d]);
	glNormal3fv(normals[d]);
	glVertex3fv(vertices[d]);
	glEnd();
}

void lineloop(int a, int b, int c, int d)
{

	/* draw a lineloop via list of vertices */
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(vertices[a]);
	glVertex3fv(vertices[b]);
	glVertex3fv(vertices[c]);
	glVertex3fv(vertices[d]);
	glEnd();
}

void colorcube(void)
{

	/* map vertices to faces */

	polygon(0, 3, 2, 1);
	polygon(2, 3, 7, 6);
	polygon(0, 4, 7, 3);
	polygon(1, 2, 6, 5);
	polygon(4, 5, 6, 7);
	polygon(0, 1, 5, 4);

	lineloop(0, 3, 2, 1);
	lineloop(2, 3, 7, 6);
	lineloop(0, 4, 7, 3);
	lineloop(1, 2, 6, 5);
	lineloop(4, 5, 6, 7);
	lineloop(0, 1, 5, 4);
}

void updateCubo(Cubo c) {

	if (c->x != c->x1) {
		if (c->x1 > c->x) {
			c->x += c->dx;
			c->x = ceil(c->x * 10) / 10.0;
		}
		else if (c->x1 < c->x) {
			c->x -= c->dx;
			c->x = floor(c->x * 10) / 10.0;
		}
	}
	if (c->y != c->y1) {
		if (c->y1 > c->y) {
			c->y += c->dy;
			c->y = ceil(c->y * 10) / 10.0;
		}
		else {
			c->y -= c->dy;
			c->y = floor(c->y * 10) / 10.0;
		}
	}
}

void desenhaTabuleiro() { //main map

	int colunas = numColunas(niveis, iNivel);
	int linhas = numLinhas(niveis, iNivel);


	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			glPushMatrix();
			glTranslatef(j * 2.0, i * 2.0, 0.0);
			glScalef(0.75, 0.75, 0.25);
			if (getCelula(niveis, iNivel, i, j) == '.' || getCelula(niveis, iNivel, i, j) == '*') {
				glColor3f(1.0, 1.0, 0.0);	//yello squares
			}
			else {
				glColor3f(0.15, 0.15, 0.15); //base table (gray)
			}
			colorcube();
			glPopMatrix();
		}
	}

	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			glPushMatrix();
			glTranslatef(j * 2.0, i * 2.0, 1.0);
			glScalef(0.75, 0.75, 0.75);
			if (getCelula(niveis, iNivel, i, j) == '#') {
				glColor3f(1.0, 0.0, 0.0); //walls
				colorcube();
			}
			glPopMatrix();
		}
	}

	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			glPushMatrix();
			glTranslatef(aCubos[i][j]->x, aCubos[i][j]->y, aCubos[i][j]->z);
			glScalef(0.75, 0.75, 0.75);
			if (aCubos[i][j]->c == '@') {
				glColor3f(0.0, 1.0, 0.0); //green
				colorcube();
			}
			else if (aCubos[i][j]->c == '$') {
				glColor3f(0.0, 0.0, 1.0); //blue
				colorcube();
			}
			glPopMatrix();
		}
	}
}

void desenhaMapaNivel(int iNivel) { //little map

	int colunas = numColunas(niveis, iNivel);
	int linhas = numLinhas(niveis, iNivel);
	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			glPushMatrix();
			glTranslatef(j * 2.0, i * 2.0, 0.0);
			glScalef(0.9, 0.9, 0.9);
			if (getCelula(niveis, iNivel, i, j) == '#') {
				glColor3f(1.0, 0.0, 0.0); //wall
				colorcube();
			}
			else if (getCelula(niveis, iNivel, i, j) == '@' || getCelula(niveis, iNivel, i, j) == '+') {
				glColor3f(0.0, 1.0, 0.0); //green
				colorcube();
			}
			else if (getCelula(niveis, iNivel, i, j) == '$' || getCelula(niveis, iNivel, i, j) == '*') {
				glColor3f(0.0, 0.0, 1.0); //blue
				colorcube();
			}
			else if (getCelula(niveis, iNivel, i, j) == '.') {
				glColor3f(1.0, 1.0, 0.0); //yellow
				colorcube();
			}
			glPopMatrix();
		}
	}
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (startGame == 1) {

		glColor3f(1.0, 1.0, 1.0);
		char temp[40];
		snprintf(temp, 40, "Play Time:  %d", (int)timer);
		iText(-2.5, 1.8, temp, font);
		snprintf(temp, 40, "Moves Made:  %d", movesMade);
		iText(-2.5, 1.6, temp, font);
		snprintf(temp, 40, "Boxes Left:  %d", boxLeft);
		iText(-2.5, 1.4, temp, font);


		glPushMatrix();
		glRotatef(rotX, rotY, 0.0, 1.0);
		glScalef(0.2, 0.2, 0.2);
		glTranslatef(-7, -5, 0.0);
		desenhaTabuleiro();	//draw main map
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-2.5, -1.5, 0.0);
		glScalef(0.07, 0.07, 0.07);
		desenhaMapaNivel(iNivel); //draw little map
		glPopMatrix();
	}
	else {
		if (playerlost == 0 && levelUp == 0) {
			glColor3f(1.0, 1.0, 1.0);
			iText(-1, 0, "Press 0 - 9  to Start Game", font);
		}

		if (playerlost == 1) {
			glColor3f(1.0, 1.0, 1.0);
			iText(-1.5, 0, "Player has lost . Press 0 - 9 to start game again", font);
		}

		if (levelUp == 1)
		{
			glColor3f(1.0, 1.0, 1.0);
			char temp[40];
			snprintf(temp, 40, "Play won with %d moves in %0.0f seconds.", movesMade,timer);
			iText(-1, 0 ,temp, font);
			glFlush();
			glutSwapBuffers();
			Sleep(2000);
			levelUp = 0;
			startGame = 1;
			movesMade = 0;
			timer = 0;
		}
	}

	glFlush();
	glutSwapBuffers();
}


void myReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h) {
		glOrtho(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
			2.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
	}
	else {
		glOrtho(-2.0 * (GLfloat)w / (GLfloat)h,
			2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, -10.0, 10.0);
	}
	glMatrixMode(GL_MODELVIEW);

}

void update(int v) {

	timer += .01;	//play time

	int colunas = numColunas(niveis, iNivel);
	int linhas = numLinhas(niveis, iNivel);


	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			updateCubo(aCubos[i][j]);
		}
	}



	boxLeft = 0;
	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			if (getCelula(niveis, iNivel, i, j) == '.') {
				if (aCubos[i][j]->c != '$') {
					boxLeft++; //if there isn't blue cube over yellow square then increase the counter
				}
			}
		}
	}


	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			if (getCelula(niveis, iNivel, i, j) == '.') {
				if (aCubos[i][j]->c != '$') {
					break;
				}
			}
		}
		if (j < colunas) break;
	}
	if (i == linhas) { //next level
		Sleep(100);
		startGame = 0;
		levelUp = 1;
		iNivel++;
		copyToaCubos();
	}
	else {
		for (i = 0; i < numLinhas(niveis, iNivel); i++) { //possible moves
			for (j = 0; j < numColunas(niveis, iNivel); j++) {

				if (aCubos[i][j]->c == '$' && aCubos[i][j]->x == aCubos[i][j]->x1 && aCubos[i][j]->y == aCubos[i][j]->y1)
				{

					if (aCubos[i - 1][j]->c != '#' && aCubos[i + 1][j]->c != '#') { //left
						break;
					}
					if (aCubos[i][j - 1]->c != '#' && aCubos[i][j + 1]->c != '#') { //up
						break;
					}
					if (aCubos[i + 1][j]->c != '#' && aCubos[i - 1][j]->c != '#') { //right
						break;
					}
					if (aCubos[i][j + 1]->c != '#' && aCubos[i][j - 1]->c != '#') { //down
						break;
					}
				}
				else if (aCubos[i][j]->c == '$') {
					break;
				}
			}
			if (j < numColunas(niveis, iNivel)) break;
		}
		if (i == numLinhas(niveis, iNivel)) { //gameover
			Sleep(100);
			playerlost = 1;
			startGame = 0;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(v, update, v);
}

void teclas(unsigned char key, int x, int y) {



	if (startGame == 0) {
		if (key >= '0' && key <= '9') {
			iNivel = key - '0';
			startGame = 1;
			playerlost = 0;
			timer = 0;
			movesMade = 0;
			copyToaCubos();
		}
		else if (key == 'q') {
			exit(0);
		}
	}
	else {

		if (key == 'a') {
			rotX += 1;
		}
		else if (key == 'd') {
			rotX -= 1;
		}
		else if (key == 'w') {
			rotY += .05;
		}
		else if (key == 's') {
			rotY -= .05;
		}
		else if (key == 'r') {
			startGame = 0;
		}
	}
	glutPostRedisplay();
}

void teclasEspeciais(int key, int x, int y) {

	if (startGame == 1) {

		int colunas = numColunas(niveis, iNivel);
		int linhas = numLinhas(niveis, iNivel);
		for (i = 0; i < numLinhas(niveis, iNivel); i++) {
			for (j = 0; j < numColunas(niveis, iNivel); j++) {
				if (aCubos[i][j]->c == '@') { //green cube
					switch (key) {
					case GLUT_KEY_UP:
						if (aCubos[i][j - 1]->c != '#') { //if there is wall can't move
							if (aCubos[i][j - 1]->c == ' ' || aCubos[i][j - 1]->c == '.') { //if there is empty place or yellow square then can move

								aCubos[i][j]->c = ' ';
								aCubos[i][j - 1]->c = '@';	  //move up
								aCubos[i][j - 1]->x = aCubos[i][j]->x;	 //set x to starting coord
								aCubos[i][j - 1]->x1 = (j - 1) * 2.0;	 //set x1 to final pos
								movesMade++;
								goto X;
							}
							else if (aCubos[i][j - 1]->c == '$') { // if there is blue cube above need to move together
								if (aCubos[i][j - 2]->c != '#') { //check for wall
									if (aCubos[i][j - 2]->c == ' ' || aCubos[i][j - 2]->c == '.') {

										aCubos[i][j - 2]->c = '$'; //blue move up
										aCubos[i][j - 2]->x = aCubos[i][j - 1]->x; //set x to starting coord
										aCubos[i][j - 2]->x1 = (j - 2) * 2.0; //set x1 to final pos

										aCubos[i][j - 1]->c = '@';// green move up
										aCubos[i][j - 1]->x = aCubos[i][j]->x; //set x to starting coord
										aCubos[i][j - 1]->x1 = (j - 1) * 2.0; //set x1 to final pos
										aCubos[i][j]->c = ' '; //clear pos

										movesMade++;
										goto X;
									}
								}
							}
						}
						break;
					case GLUT_KEY_DOWN:
						if (aCubos[i][j + 1]->c != '#') {
							if (aCubos[i][j + 1]->c == ' ' || aCubos[i][j + 1]->c == '.') {

								aCubos[i][j]->c = ' ';
								aCubos[i][j + 1]->c = '@';
								aCubos[i][j + 1]->x = aCubos[i][j]->x;
								aCubos[i][j + 1]->x1 = (j + 1) * 2.0;

								movesMade++;
								goto X;
							}
							else if (aCubos[i][j + 1]->c == '$') {
								if (aCubos[i][j + 2]->c != '#') {
									if (aCubos[i][j + 2]->c == ' ' || aCubos[i][j + 2]->c == '.') {

										aCubos[i][j + 2]->c = '$';
										aCubos[i][j + 2]->x = aCubos[i][j + 1]->x;
										aCubos[i][j + 2]->x1 = (j + 2) * 2.0;

										aCubos[i][j + 1]->c = '@';
										aCubos[i][j + 1]->x = aCubos[i][j]->x;
										aCubos[i][j + 1]->x1 = (j + 1) * 2.0;
										aCubos[i][j]->c = ' ';

										movesMade++;
										goto X;
									}
								}
							}
						}
						break;
					case GLUT_KEY_LEFT:
						if (aCubos[i - 1][j]->c != '#') {
							if (aCubos[i - 1][j]->c == ' ' || aCubos[i - 1][j]->c == '.') {

								aCubos[i][j]->c = ' ';
								aCubos[i - 1][j]->c = '@';
								aCubos[i - 1][j]->y = aCubos[i][j]->y;
								aCubos[i - 1][j]->y1 = (i - 1) * 2.0;

								movesMade++;
								goto X;
							}
							else if (aCubos[i - 1][j]->c == '$') {
								if (aCubos[i - 2][j]->c != '#') {
									if (aCubos[i - 2][j]->c == ' ' || aCubos[i - 2][j]->c == '.') {

										aCubos[i - 2][j]->c = '$';
										aCubos[i - 2][j]->y = aCubos[i - 1][j]->y;
										aCubos[i - 2][j]->y1 = (i - 2) * 2.0;

										aCubos[i - 1][j]->c = '@';
										aCubos[i - 1][j]->y = aCubos[i][j]->y;
										aCubos[i - 1][j]->y1 = (i - 1) * 2.0;

										aCubos[i][j]->c = ' ';

										movesMade++;
										goto X;
									}
								}
							}
						}
						break;
					case GLUT_KEY_RIGHT:
						if (aCubos[i + 1][j]->c != '#') {
							if (aCubos[i + 1][j]->c == ' ' || aCubos[i + 1][j]->c == '.') {

								aCubos[i][j]->c = ' ';
								aCubos[i + 1][j]->c = '@';
								aCubos[i + 1][j]->y = aCubos[i][j]->y;
								aCubos[i + 1][j]->y1 = (i + 1) * 2.0;
								movesMade++;
								goto X;
							}
							else if (aCubos[i + 1][j]->c == '$') {
								if (aCubos[i + 2][j]->c != '#') {
									if (aCubos[i + 2][j]->c == ' ' || aCubos[i + 2][j]->c == '.') {

										aCubos[i + 2][j]->c = '$';
										aCubos[i + 2][j]->y = aCubos[i + 1][j]->y;
										aCubos[i + 2][j]->y1 = (i + 2) * 2.0;

										aCubos[i + 1][j]->c = '@';
										aCubos[i + 1][j]->y = aCubos[i][j]->y;
										aCubos[i + 1][j]->y1 = (i + 1) * 2.0;

										aCubos[i][j]->c = ' ';
										movesMade++;
										goto X;
									}
								}
							}
						}
						break;
					}
				}
			}
		}
	X:;
	}

	glutPostRedisplay();
}
Cubo criaCubo(int i, int j)
{
	Cubo c = (Cubo)malloc(sizeof(*c));
	c->c = getCelula(niveis, iNivel, i, j);
	c->x = c->x1 = j * 2.0;
	c->y = c->y1 = i * 2.0;
	c->z = c->z1 = 1.0;
	c->dx = c->dy = .10;
	return c;
}
void copyToaCubos() {

	boxLeft = 0;
	aCubos = malloc(sizeof(Cubo*) * numLinhas(niveis, iNivel));

	for (i = 0; i < numLinhas(niveis, iNivel); i++) {
		aCubos[i] = malloc(sizeof(Cubo) * numColunas(niveis, iNivel));
	}

	for (i = 0; i < numLinhas(niveis, iNivel); i++) {
		for (j = 0; j < numColunas(niveis, iNivel); j++) {
			aCubos[i][j] = criaCubo(i, j);
			if (aCubos[i][j]->c == '*') {
				aCubos[i][j]->c = '$';
			}
			if (aCubos[i][j]->c == '+') {
				aCubos[i][j]->c = '@';
			}
			if (aCubos[i][j]->c == '.') {
				boxLeft++;
			}
		}
	}
}
void myInit() {
	niveis = readNiveisSokoban("sokoban.txt");
	iNivel = 0;
	copyToaCubos();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 700);
	glutCreateWindow("colorcube");
	myInit();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(teclas);
	glutSpecialFunc(teclasEspeciais);
	glEnable(GL_DEPTH_TEST);
	glutTimerFunc(timeUpdate, update, timeUpdate);
	glutMainLoop();
}