/*
 * main.c - r3pong - 3-dimensional pong implementation using GLUT
 *
 * Copyright (C) 2010, Stefan Schramm <mail@stefanschramm.net>
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

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "r3pong.h"

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

#define FIELDWIDTH 3.0
#define FIELDHEIGHT 3.0
#define FIELDDISTANCE 4.0
#define TIMERTIMEOUT 5

player players[2];

struct {
	GLfloat x, y, z;
	GLfloat vx, vy, vz;
	GLfloat size;
} ball;

int screen_width, screen_height;

void render_paddle(player *p) {

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
	glColor3f(0.5, 0.5, 0.5);
	glVertex3f(-FIELDDISTANCE/2, -FIELDHEIGHT/2, FIELDWIDTH/2);
	glVertex3f(-FIELDDISTANCE/2, -FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, -FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, -FIELDHEIGHT/2, FIELDWIDTH/2);
	glEnd();
	// back
	glBegin(GL_QUADS);
	glColor3f(0.7, 0.7, 0.7);
	glVertex3f(-FIELDDISTANCE/2, -FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(-FIELDDISTANCE/2, FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, FIELDHEIGHT/2, -FIELDWIDTH/2);
	glVertex3f(FIELDDISTANCE/2, -FIELDHEIGHT/2, -FIELDWIDTH/2);
	glEnd();
	// top
	glBegin(GL_QUADS);
	glColor3f(0.5, 0.5, 0.5);
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
	render_paddle(&players[0]);
	render_paddle(&players[1]);

	// print score
	glColor3f(1, 1, 1);
	// print text just a bit (0.1) in front of the back wall
	glRasterPos3f(0, FIELDHEIGHT/2 - 0.3, -FIELDWIDTH/2 + 0.1);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 0x30 | (players[0].score % 10));
	// TODO:
	// - how to use a larger font?
	// - count and display real score (for now just the last digit of player[0]'s score is shown)

	glPopMatrix();
	glutSwapBuffers();
}

void keyboard_callback(unsigned char key, int x, int y) {

	switch(key) {
		case 'q':
			exit(EXIT_SUCCESS);
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

	players[1].z = (x - screen_width/2) * ((FIELDWIDTH - players[1].size) / screen_width) + players[1].size / 2;
	players[1].y = (-y + screen_height/2) * ((FIELDHEIGHT - players[1].size) / screen_height) - players[1].size / 2;
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
		0.0, 0.0, 5.0,
		0.0, 0.0, -1.0,
		0.0f, 1.0f, 0.0f);
}

void timer_callback(int value) {

	ball.x += ball.vx;
	ball.y += ball.vy;
	ball.z += ball.vz;

	// left player paddle - pseudo-"ai"
	players[0].y = ball.y - players[0].size / 2;
	players[0].z = ball.z + players[0].size / 2;

	// check collisions
	if (ball.x >= FIELDDISTANCE / 2) {
		// right wall
		// TODO: take ball size into account
		if (ball.y >= players[1].y && ball.y <= players[1].y + players[1].size &&
			ball.z <= players[1].z && ball.z >= players[1].z - players[1].size
		) {
			ball.vx *= -1;
		}
		else {
			players[0].score++;
			ball.x = 0;
			ball.y = 0;
			ball.z = 0;
		}
	}
	else if (ball.x <= - FIELDDISTANCE / 2) {
		// left wall
		// TODO: other player / "ai" here
		ball.vx *= -1;
	}
	if (ball.z <= -FIELDWIDTH / 2 || ball.z >= FIELDWIDTH / 2) {
		// back or front wall
		ball.vz *= -1;
	}
	if (ball.y <= -FIELDHEIGHT / 2 || ball.y >= FIELDHEIGHT / 2) {
		// bottom or top wall
		ball.vy *= -1;
	}
	render_scene();
	// TODO: make timer callback time and ball movement adjust on fps (rendering time)?
	glutTimerFunc(TIMERTIMEOUT, timer_callback, 0);
}

int main(int argc, char **argv) {

	// init ball
	ball.x = 0;
	ball.y = 0;
	ball.z = 0;
	ball.size = 0.1;
	// TODO: random direction on start
	ball.vx = 0.018;
	ball.vy = 0.01;
	ball.vz = -0.01;

	// init players
	players[0].player = -1;
	players[0].y = 0;
	players[0].z = 0;
	players[0].size = 1;
	players[0].score = 0;
	players[1] = players[0];
	players[1].player = 1;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT);
	glutCreateWindow("r3pong");
	glutDisplayFunc(render_scene);
	glutKeyboardFunc(keyboard_callback);
	glutPassiveMotionFunc(mouse_callback);
	glutReshapeFunc(reshape_callback);
	glEnable(GL_DEPTH_TEST);
	timer_callback(0);
	glutMainLoop();

	// TODO:
	// - count player score
	// - pseudo-ai: left paddle z+y coordinates == ball position
	// - multiplayer mode (server/client)
}
