/* Rotating cube with color interpolation */

/* Demonstration of use of homogeneous coordinate
transformations and simple data structure for representing
cube from Chapter 4 */

/*Both normals and colors are assigned to the vertices */
/*Cube is centered at origin so (unnormalized) normals
are the same as the vertex values */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sokoban.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLfloat vertices[][3] = { { -1.0, -1.0, -1.0}, {1.0, -1.0, -1.0},
	{1.0, 1.0, -1.0}, { -1.0, 1.0, -1.0}, { -1.0, -1.0, 1.0},
	{1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, { -1.0, 1.0, 1.0}
};

typedef struct cubo* Cubo;
struct cubo {
	double x, y, z;
	double x1, y1, z1;
	double dx, dy;
	char c;		// The character the cube refers to ( * $ @ # )
};
/*
Here x,y,z and x1,y1,z1 are coordinates of the cube which is used for animation . When cubes are not moving then x==x1 and y==y1 .
When they start to move x1 and y1 are set to destination(Where the cube will move to) coordinates . And x , y are set to starting coordinates .
Then x,y are updated to x1,y1 with time in the updateCubo() function until x==x1 and y==y1 . Which  makes a smooth animation .
The change in x,y coordinate is denoted by dx,dy . The less it is , more smooth the animation is .
*/
/* In the program when we write if(aCubos[i][j]->x != aCubos[i][j]->x1 || aCubos[i][j]->y != aCubos[i][j]->y1) that means this cube is on the move . */

Cubo** aCubos;  // Array of cubes which position is changed during the game ( Blue ,Green)
int timeUpdate = 10;     // intervalo de tempo (ms) para o update


double rotX = -40, rotY = 20,rotZ=-40 ,zoom=.2,boardX=-7,boardY=-5,boardZ=0;	// For Camera rotation
NiveisTab niveis;
int iNivel = 0;		// Current level
int startGame = 0;
int movesMade, boxLeft;
int playerlost = 0;
double timer = 0;
int i, j;
const void* font = GLUT_BITMAP_TIMES_ROMAN_24;

void copyToaCubos();

void iText(double x, double y, char* str, void* font)
{
	glRasterPos3d(x, y, 0);
	int i;
	for (i = 0; str[i]; i++) {
		glutBitmapCharacter(font, str[i]); //,GLUT_BITMAP_8_BY_13, GLUT_BITMAP_TIMES_ROMAN_24
	}
}
void polygon(int a, int b, int c, int d)
{
	/* draw a polygon via list of vertices */
	glBegin(GL_POLYGON);
	glVertex3fv(vertices[a]);
	glVertex3fv(vertices[b]);
	glVertex3fv(vertices[c]);
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

void updateCubo(Cubo c)
{
	if (c->x != c->x1)
	{
		if (c->x1 > c->x)
		{
			c->x += c->dx;
			c->x = ceil(c->x * 10) / 10.0;
		}
		else
		{
			c->x -= c->dx;
			c->x = floor(c->x * 10) / 10.0;
		}
	}
	if (c->y != c->y1)
	{
		if (c->y1 > c->y)
		{
			c->y += c->dy;
			c->y = ceil(c->y * 10) / 10.0;
		}
		else
		{
			c->y -= c->dy;
			c->y = floor(c->y * 10) / 10.0;
		}
	}
}

// desenha um tabuleiro de n linhas e m colunas
// o canto inferior esquerdo do tabuleiro está no (0.0,0.0)
// cada cubo (posição do tabuleiro) tem tamanho 2x2x2
void desenhaTabuleiro()
{
	int colunas = numColunas(niveis, iNivel);
	int linhas = numLinhas(niveis, iNivel);


	/* Drawing the board */
	for (i = 0; i < linhas; i++)
	{
		for (j = 0; j < colunas; j++) {
			glPushMatrix();
			glTranslatef(j * 2.0, i * 2.0, 0.0);
			glScalef(0.75, 0.75, 0.25);
			if (getCelula(niveis, iNivel, i, j) == '.' || getCelula(niveis, iNivel, i, j) == '*' || getCelula(niveis, iNivel, i, j) == '+')
			{
				glColor3f(1.0, 1.0, 0.0);	// Drawing the yellow goal squares
			}
			else
			{
				glColor3f(0.15, 0.15, 0.15);
			}
			colorcube();
			glPopMatrix();
		}
	}

	/* Drawing Red walls */
	for (i = 0; i < linhas; i++)
		for (j = 0; j < colunas; j++)
		{
			glPushMatrix();
			glTranslatef(j * 2.0 , i * 2.0, 1.0);
			glScalef(0.75, 0.75, 0.75);
			if (getCelula(niveis, iNivel, i, j) == '#') {
				glColor3f(1.0, 0.0, 0.0);
				colorcube();
			}
			glPopMatrix();
		}

	/* Drawing the Green and blue cubes */
	for (i = 0; i < linhas; i++)
		for (j = 0; j < colunas; j++) {
			glPushMatrix();
			glTranslatef(aCubos[i][j]->x, aCubos[i][j]->y, aCubos[i][j]->z);
			glScalef(0.75, 0.75, 0.75);
			if (aCubos[i][j]->c == '@') {
				glColor3f(0.0, 1.0, 0.0);	// Green
				colorcube();
			}
			else if (aCubos[i][j]->c == '$') {
				glColor3f(0.0, 0.0, 1.0);	// Blue
				colorcube();
			}
			glPopMatrix();
		}
}

void desenhaMapaNivel(int iNivel)
{
	int colunas = numColunas(niveis, iNivel);
	int linhas = numLinhas(niveis, iNivel);
	for (i = 0; i < linhas; i++)
		for (j = 0; j < colunas; j++) {
			glPushMatrix();
			glTranslatef(j * 2.0, i * 2.0, -1.0);
			glScalef(0.9, 0.9, 0.9);
			if (getCelula(niveis, iNivel, i, j) == '#') {
				glColor3f(1.0, 0.0, 0.0);
				colorcube();
			}
			else if (getCelula(niveis, iNivel, i, j) == '@' ||
			         getCelula(niveis, iNivel, i, j) == '+') {
				glColor3f(0.0, 1.0, 0.0);
				colorcube();
			}
			else if (getCelula(niveis, iNivel, i, j) == '$' ||
			         getCelula(niveis, iNivel, i, j) == '*') {
				glColor3f(0.0, 0.0, 1.0);
				colorcube();
			}
			else if (getCelula(niveis, iNivel, i, j) == '.') {
				glColor3f(1.0, 1.0, 0.0);
				colorcube();
			}
			glPopMatrix();
		}
}

void display(void)
{
	/* display callback, clear frame buffer and z buffer,
	   rotate cube and draw, swap buffers */

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glShadeModel(GL_FLAT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (startGame == 1)
	{
		// Texts
		glColor3f(0.9, 0.9, 0.0);
		char temp[40];
		snprintf(temp, 40, "Play Time:  %d", (int)timer);
		iText(-2.5, 1.8, temp, font);
		snprintf(temp, 40, "Moves Made:  %d", movesMade);
		iText(-2.5, 1.6, temp, font);
		snprintf(temp, 40, "Boxes Left:  %d", boxLeft);
		iText(-2.5, 1.4, temp, font);


		glPushMatrix();
		glRotatef(rotX, 1.0, 0.0 , 0.0);  // Rotation in X
		glRotatef(rotY, 0.0, 1.0, 0.0);	  // Rotation in Y
		glRotatef(rotZ, 0.0, 0.0, 1.0);	  // Rotation in Z
		glScalef(zoom,zoom,zoom);	
		glTranslatef(boardX, boardY, 0);
		desenhaTabuleiro();		// Draw the game board

		glPopMatrix();

		glPushMatrix();
		glTranslatef(-2.5, -1.9, 0.0);
		glScalef(0.07, 0.07, 0.07);
		desenhaMapaNivel(iNivel);	// Draw the level map
		glPopMatrix();
	}
	else
	{
		if (playerlost == 0) {
			glColor3f(0.9, 0.9, 0.0);
			iText(-1, 0, "Press 0 - 9  to Start Game", font);
		}

		if (playerlost == 1) {
			glColor3f(0.9, 0.0, 0.0);
			iText(-1.5, 0, "Player has lost . Press 0 - 9 to start game again", font);
		}
	}

	glFlush();
	glutSwapBuffers();
}


void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
		        2.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-2.0 * (GLfloat)w / (GLfloat)h,
		        2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
}

void update(int v) // timer callback
{
	timer += .01;	// Increase playtime

	int colunas = numColunas(niveis, iNivel);
	int linhas = numLinhas(niveis, iNivel);

	/* Updating the cubes coordinates for moving animation */
	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++)
		{
			updateCubo(aCubos[i][j]);
		}
	}


	/* Counting how many boxes are left */
	boxLeft = 0;
	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			if (getCelula(niveis, iNivel, i, j) == '.')		// Checking for yellow square
			{
				if (aCubos[i][j]->c != '$')	   // Checking if there is blue cube over the yellow square . If not then increase the count of left boxes .
				{
					boxLeft++;
				}
				/* This case is for animation . Checking if the cube is on the move .
				If blue cube is over the yellow square partially , that means if it is still moving not raeched the yellow square yet , it will be also counted as a left box .*/
				else if (aCubos[i][j]->x != aCubos[i][j]->x1 || aCubos[i][j]->y != aCubos[i][j]->y1)
				{
					boxLeft++;
				}
			}
		}
	}

	/* Checking if all the blue cube is over the yellow squares . If yes , then we will proceed to upper level .*/
	for (i = 0; i < linhas; i++) {
		for (j = 0; j < colunas; j++) {
			if (getCelula(niveis, iNivel, i, j) == '.')
			{
				if (aCubos[i][j]->c != '$')
				{
					break;
				}
				else if (aCubos[i][j]->x != aCubos[i][j]->x1 || aCubos[i][j]->y != aCubos[i][j]->y1)
				{
					break;
				}
			}
		}
		if (j < colunas) break;
	}
	if (i == linhas)  // All the blue cube is over the yellow squares
	{
		Sleep(1000);
		timer = 0;
		movesMade = 0;
		iNivel++;
		copyToaCubos();
	}
	else
	{
		/* Checking if there is any move possible or not . If not then game is over . */
		for (i = 0; i < linhas; i++) {
			for (j = 0; j < colunas; j++) {

				if (aCubos[i][j]->c == '$' && aCubos[i][j]->x == aCubos[i][j]->x1 && aCubos[i][j]->y == aCubos[i][j]->y1)
				{
					/* To check if a blue box can be moved or not , we need to check the two adjacent side of it .
					If its two adjacent sides are walls , the it means its in a corner . Can't be move further . */
					if (aCubos[i - 1][j]->c != '#' && aCubos[i + 1][j]->c != '#')
					{
						break;
					}
					if (aCubos[i][j - 1]->c != '#' && aCubos[i][j + 1]->c != '#')
					{
						break;
					}
					if (aCubos[i + 1][j]->c != '#' && aCubos[i - 1][j]->c != '#')
					{
						break;
					}
					if (aCubos[i][j + 1]->c != '#' && aCubos[i][j - 1]->c != '#')
					{
						break;
					}
				}
				else if (aCubos[i][j]->c == '$')
				{
					break;
				}
			}
			if (j < colunas) break;
		}
		if (i == linhas)		// No move is possible . All the blue boxes are in corners .
		{
			Sleep(1000);
			playerlost = 1;
			startGame = 0;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(v, update, v);
}

void teclas(unsigned char key, int x, int y)
{
	if (startGame == 0)
	{
		if (key >= '0' && key <= '9')
		{
			// Starting game
			int i, j;
			iNivel = key - '0';
			startGame = 1;
			playerlost = 0;
			timer = 0;
			movesMade = 0;
			copyToaCubos();		// Copying all the cubes of current level to aCubos
		}
		else if (key == 'q')
		{
			exit(0);
		}
	}
	else
	{
		if (key == 's')
		{
			rotX += 1;
		}
		else if (key == 'w')
		{
			rotX -= 1;
		}
		else if (key == 'd')
		{
			rotY += 1;
		}
		else if (key == 'a')
		{
			rotY -= 1;
		}
		else if (key == 'z')
		{
			rotZ -= 1;
		}
		else if (key == 'x')
		{
			rotZ += 1;
		}
		else if (key == '+')
		{
			zoom += .01;
		}
		else if (key == '-')
		{
			zoom -= .01;
		}
		else if (key == 'l')
		{
			boardY += .1;
		}
		else if (key == 'j')
		{
			boardY -= .1;
		}
		else if (key == 'k')
		{
			boardX += .1;
		}
		else if (key == 'i')
		{
			boardX -= .1;
		}
		else if (key == 'q')
		{
			startGame = 0;
		}
	}
	glutPostRedisplay();
}

void teclasEspeciais(int key, int x, int y)
{
	if (startGame == 1)
	{
		int colunas = numColunas(niveis, iNivel);
		int linhas = numLinhas(niveis, iNivel);
		for (i = 0; i < numLinhas(niveis, iNivel); i++)
		{
			for (j = 0; j < numColunas(niveis, iNivel); j++) {
				if (aCubos[i][j]->c == '@')		// Searching for the green cube in the board to move
				{
					switch (key) {
					case GLUT_KEY_UP:	// Move upward
						if (aCubos[i][j - 1]->c != '#')		// Checking if there is wall in the up . If there is wall , then the green cube can't move uup .
						{
							if (aCubos[i][j - 1]->c == ' ' || aCubos[i][j - 1]->c == '.')    // If there is empty space or yellow square in up , then the cube can move upward .
							{
								aCubos[i][j]->c = ' ';	  // Clearing the current position of green cube
								aCubos[i][j - 1]->c = '@';	  // Moving it upward
								aCubos[i][j - 1]->x = aCubos[i][j]->x;	// Setting x to starting coordinate
								aCubos[i][j - 1]->x1 = (j - 1) * 2.0;	// Setting x1 to destination coordinate  for animation
								movesMade++;	// Increase count of moves made .
								goto X;
							}
							else if (aCubos[i][j - 1]->c == '$')	// If there is blue cube in the up , then we have to move both blue and green cube . Before that we have to check if the blue cube can be move upward , in the same way as we checked for the green cube previously .
							{
								if (aCubos[i][j - 2]->c != '#')
								{
									if (aCubos[i][j - 2]->c == ' ' || aCubos[i][j - 2]->c == '.')
									{
										aCubos[i][j - 2]->c = '$';
										aCubos[i][j - 2]->x = aCubos[i][j - 1]->x;
										aCubos[i][j - 2]->x1 = (j - 2) * 2.0;

										aCubos[i][j - 1]->c = '@';
										aCubos[i][j - 1]->x = aCubos[i][j]->x;
										aCubos[i][j - 1]->x1 = (j - 1) * 2.0;
										aCubos[i][j]->c = ' ';

										movesMade++;
										goto X;
									}
								}
							}
						}
						break;
					case GLUT_KEY_DOWN:		// Move downward
						if (aCubos[i][j + 1]->c != '#')
						{
							if (aCubos[i][j + 1]->c == ' ' || aCubos[i][j + 1]->c == '.')
							{
								aCubos[i][j]->c = ' ';
								aCubos[i][j + 1]->c = '@';
								aCubos[i][j + 1]->x = aCubos[i][j]->x;
								aCubos[i][j + 1]->x1 = (j + 1) * 2.0;

								movesMade++;
								goto X;
							}
							else if (aCubos[i][j + 1]->c == '$')
							{
								if (aCubos[i][j + 2]->c != '#')
								{
									if (aCubos[i][j + 2]->c == ' ' || aCubos[i][j + 2]->c == '.')
									{
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
					case GLUT_KEY_LEFT:		// Move left
						if (aCubos[i - 1][j]->c != '#')
						{
							if (aCubos[i - 1][j]->c == ' ' || aCubos[i - 1][j]->c == '.')
							{
								aCubos[i][j]->c = ' ';
								aCubos[i - 1][j]->c = '@';
								aCubos[i - 1][j]->y = aCubos[i][j]->y;
								aCubos[i - 1][j]->y1 = (i - 1) * 2.0;

								movesMade++;
								goto X;
							}
							else if (aCubos[i - 1][j]->c == '$')
							{
								if (aCubos[i - 2][j]->c != '#')
								{
									if (aCubos[i - 2][j]->c == ' ' || aCubos[i - 2][j]->c == '.')
									{
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
					case GLUT_KEY_RIGHT:	// Move right
						if (aCubos[i + 1][j]->c != '#')
						{
							if (aCubos[i + 1][j]->c == ' ' || aCubos[i + 1][j]->c == '.')
							{
								aCubos[i][j]->c = ' ';
								aCubos[i + 1][j]->c = '@';
								aCubos[i + 1][j]->y = aCubos[i][j]->y;
								aCubos[i + 1][j]->y1 = (i + 1) * 2.0;
								movesMade++;
								goto X;
							}
							else if (aCubos[i + 1][j]->c == '$')
							{
								if (aCubos[i + 2][j]->c != '#')
								{
									if (aCubos[i + 2][j]->c == ' ' || aCubos[i + 2][j]->c == '.')
									{
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
void copyToaCubos()
{
	boxLeft = 0;
	aCubos = malloc(sizeof(Cubo*) * numLinhas(niveis, iNivel));
	for (i = 0; i < numLinhas(niveis, iNivel); i++) {
		aCubos[i] = malloc(sizeof(Cubo) * numColunas(niveis, iNivel));
	}
	for (i = 0; i < numLinhas(niveis, iNivel); i++) {
		for (j = 0; j < numColunas(niveis, iNivel); j++) {
			aCubos[i][j] = criaCubo(i, j);
			if (aCubos[i][j]->c == '*') aCubos[i][j]->c = '$';
			if (aCubos[i][j]->c == '+') aCubos[i][j]->c = '@';
			if (aCubos[i][j]->c == '.')
			{
				boxLeft++;
			}
		}
	}
}
void myInit()
{
	niveis = readNiveisSokoban("sokoban.txt");		// Reading all the levels from the file .
	iNivel = 0;
	copyToaCubos();		// Copying all the cubes of current level to aCubos
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	/* need both double buffering and z buffer */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 700);
	glutCreateWindow("Sokoban");
	myInit();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(teclas);
	glutSpecialFunc(teclasEspeciais);
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	glutTimerFunc(timeUpdate, update, timeUpdate);
	glutFullScreen();
	glutMainLoop();
}
