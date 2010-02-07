/*
 * r3pong
 *
 * Copyright (C) 2009, Stefan Schramm <mail@stefanschramm.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <GL/glut.h> 
#include <stdio.h>
#include <stdlib.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

#define FIELDWIDTH 3.0
#define FIELDHEIGHT 3.0
#define FIELDDISTANCE 4.0

typedef struct _paddle {
	int player; // -1: left player; 1: right player
	GLfloat y;
	GLfloat z;
	GLfloat size;
} paddle;

struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat size;
} ball;

paddle paddles[2];
int screen_width, screen_height;

void draw_paddle(paddle* p) {
	glBegin(GL_QUADS);
	glColor3f(1, 0, 0);
	glVertex3f(p->player * FIELDDISTANCE/2, p->y, p->z);
	glVertex3f(p->player * FIELDDISTANCE/2, p->y+p->size, p->z);
	glVertex3f(p->player * FIELDDISTANCE/2, p->y+p->size, p->z-p->size);
	glVertex3f(p->player * FIELDDISTANCE/2, p->y, p->z-p->size);
	glEnd();
}

void render_scene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	// draw walls:
	// bottom
	glBegin(GL_QUADS);
	glColor3f(0.9, 0.9, 0.9);
	glVertex3f(-FIELDDISTANCE/2, -FIELDHEIGHT/2, FIELDWIDTH/2);
	glVertex3f(-FIELDDISTANCE/2, -FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, -FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, -FIELDHEIGHT/2, FIELDWIDTH/2);
	glEnd();
	// back
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex3f(-FIELDDISTANCE/2, -FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(-FIELDDISTANCE/2, FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, -FIELDHEIGHT/2, -FIELDWIDTH/2);
	glEnd();
	// top
	glBegin(GL_QUADS);
	glColor3f(0.9, 0.9, 0.9);
	glVertex3f(-FIELDDISTANCE/2, FIELDHEIGHT/2, FIELDWIDTH/2);
	glVertex3f(-FIELDDISTANCE/2, FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, FIELDHEIGHT/2, FIELDWIDTH/2);
	glEnd();

	// draw ball
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex3f(ball.x - ball.size, ball.y - ball.size, ball.z);
	glVertex3f(ball.x - ball.size, ball.y + ball.size, ball.z);
	glVertex3f(ball.x + ball.size, ball.y + ball.size, ball.z);
	glVertex3f(ball.x + ball.size, ball.y - ball.size, ball.z);
	glEnd();

	// draw paddles
	draw_paddle(&paddles[0]);
	draw_paddle(&paddles[1]);

	glPopMatrix();
	glutSwapBuffers();
}

void keyboard_callback(unsigned char key, int x, int y)
{
	switch(key) {
		case 'q':
			exit(0);
		case '+':
			glTranslatef(0, 0, 1);
			break;
		case '-':
			glTranslatef(0, 0, -1);
			break;
	}
	render_scene();
}


void mouse_callback(int x, int y) {
	paddles[1].z = (x - screen_width/2) * ((FIELDWIDTH - paddles[1].size) / screen_width) + paddles[1].size / 2;
	paddles[1].y = (-y + screen_height/2) * ((FIELDHEIGHT - paddles[1].size) / screen_height) - paddles[1].size / 2;
	render_scene();
}

void reshape_callback(int w, int h) {

	if(h == 0)
		h = 1;

	screen_width = w;
	screen_height = h;

	float ratio = 1.0 * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	
	gluPerspective(40,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0.0,0.0,5.0,
		0.0,0.0,-1.0,
		0.0f,1.0f,0.0f);
}

int main(int argc, char **argv) {

	// init ball
	ball.x = 0;
	ball.y = 0;
	ball.z = 0;
	ball.size = 0.1;

	// init paddles
	paddles[0].player = -1;
	paddles[0].y = 0;
	paddles[0].z = 0;
	paddles[0].size = 1;
	paddles[1] = paddles[0];
	paddles[1].player = 1;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT);
	glutCreateWindow("r3pong");
	glutDisplayFunc(render_scene);
	glutPassiveMotionFunc(mouse_callback);
	glutReshapeFunc(reshape_callback);
	glutKeyboardFunc(keyboard_callback);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();

	// TODO:
	// - implement ball movement (idleFunc?) and events
	// - pseudo-ai: left paddle z+y coordinates == ball position
	// - multiplayer mode (server/client)

}
