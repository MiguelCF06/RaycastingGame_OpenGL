#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 // one degree in radians

float playerX, playerY, playerDeltaX, playerDeltaY, playerAngle;
int mapX = 8, mapY = 8, mapSize = 64;

int map[] = 
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,2,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,1,0,0,1,1,
	1,1,1,1,1,1,1,1,
};

void drawMap2D()
{
	int x, y, xOffset, yOffset;
	
	for (y = 0; y < mapY; y++)
	{
		for (x = 0; x < mapX; x++)
		{
			if (map[y*mapX+x] == 1)
			{
				glColor3f(1, 1, 1);
			}
			else if (map[y*mapX+x] == 2)
			{
				glColor3f(1, 0, 0);
			}
			else
			{
				glColor3f(0, 0, 0);
			}
			xOffset = x * mapSize;
			yOffset = y * mapSize;
			glBegin(GL_QUADS);
			glVertex2i(xOffset + 1, yOffset + 1);
			glVertex2i(xOffset + 1, yOffset + mapSize - 1);
			glVertex2i(xOffset + mapSize - 1, yOffset + mapSize - 1);
			glVertex2i(xOffset + mapSize - 1, yOffset + 1);
			glEnd();
		}
	}
}

void drawPlayer()
{
	glColor3f(1, 0, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(playerX, playerY);
	glEnd();
	
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(playerX, playerY);
	glVertex2i(playerX + playerDeltaX * 5, playerY + playerDeltaY * 5);
	glEnd();
}

float distancePlayer_n_Ray(float ax, float ay, float bx, float by, float angle)
{
	return ( sqrt((bx - ax)*(bx - ax) + (by - ay)*(by - ay)) );
}

void drawRays2D()
{
	int ray, mx, my, mapPosition, deepOfField;
	float rayX, rayY, rayAngle, xOffset, yOffset, disT;
	
	rayAngle = playerAngle - DR * 30;
	if (rayAngle < 0)
	{
		rayAngle += 2 * PI;	
	}
	if (rayAngle > 2 * PI)
	{
		rayAngle -= 2 * PI;
	}
	for (ray = 0; ray < 60; ray++)
	{
		// Check all the horizontal lines
		deepOfField = 0;
		float distanceHorizontal = 1000000, hx = playerX, hy = playerY;
		float aTan = -1/tan(rayAngle); // Inverse negative tangent of the angle
		if (rayAngle > PI) // Looking up
		{
			rayY = (((int)playerY>>6)<<6) - 0.0001;
			rayX = (playerY - rayY) * aTan + playerX;
			yOffset = -64;
			xOffset = -yOffset * aTan;
		}
		if (rayAngle < PI) // Looking down	
		{
			rayY = (((int)playerY>>6)<<6) + 64;
			rayX = (playerY - rayY) * aTan + playerX;
			yOffset = 64;
			xOffset = -yOffset * aTan;
		}
		if (rayAngle == 0 || rayAngle == PI)
		{
			rayX = playerX;
			rayY = playerY;
			deepOfField = 8;
		}
		int mv = 0, mh = 0;
		while (deepOfField < 8)
		{
			mx = (int) (rayX)>>6;
			my = (int) (rayY)>>6;
			mapPosition = my * mapX + mx;
			if (mapPosition > 0 && mapPosition < mapX*mapY && map[mapPosition] > 0) // Hit wall
			{
				mh = map[mapPosition];
				hx = rayX;
				hy = rayY;
				distanceHorizontal = distancePlayer_n_Ray(playerX, playerY, hx, hy, rayAngle);
				deepOfField = 8;
			}
			else // next horizontal line
			{
				rayX += xOffset;
				rayY += yOffset;
				deepOfField += 1;
			}
		}
		// Check Vertical Line
		deepOfField = 0;
		float distanceVertical = 1000000, vx = playerX, vy = playerY;
		float nTan = -tan(rayAngle);
		if (rayAngle > P2 && rayAngle < P3)
		{
			rayX = (((int)playerX>>6)<< 6) - 0.0001;
			rayY = (playerX - rayX) * nTan + playerY;
			xOffset = -64;
			yOffset = -xOffset * nTan;
		}
		if (rayAngle < P2 || rayAngle > P3)
		{
			rayX = (((int)playerX>>6)<<6) + 64;
			rayY = (playerX - rayX) * nTan + playerY;
			xOffset = 64;
			yOffset = -xOffset * nTan;
		}
		if (rayAngle == 0 || rayAngle == PI)
		{
			rayX = playerX;
			rayY = playerY;
			deepOfField = 8;
		}
		while (deepOfField < 8)
		{
			mx = (int) (rayX)>>6;
			my = (int) (rayY)>>6;
			mapPosition = my * mapX + mx;
			if (mapPosition > 0 && mapPosition < mapX*mapY && map[mapPosition] > 0) // Hit wall
			{
				mv = map[mapPosition];
				vx = rayX;
				vy = rayY;
				distanceVertical = distancePlayer_n_Ray(playerX, playerY, vx, vy, rayAngle);
				deepOfField = 8;
			}
			else // next line
			{
				rayX += xOffset;
				rayY += yOffset;
				deepOfField += 1;
			}
		}
		if (distanceVertical < distanceHorizontal)
		{
			rayX = vx;
			rayY = vy;
			disT = distanceVertical;
			glColor3f(0, 0, 0.9);
			if (mv == 2)
			{
				glColor3f(0.9, 0, 0);
			}
		}
		if (distanceHorizontal < distanceVertical)
		{
			rayX = hx;
			rayY = hy;
			disT = distanceHorizontal;
			glColor3f(0, 0, 0.7);
			if (mh == 2)
			{
				glColor3f(0.7, 0, 0);
			}
		}
		glLineWidth(4);
		glBegin(GL_LINES);
		glVertex2i(playerX, playerY);
		glVertex2i(rayX, rayY);
		glEnd();
		
		// Draw the 3D walls
		float newAngle = playerAngle - rayAngle; // Fix fisheye
		if (newAngle < 0)
		{
			newAngle += 2 * PI;
		}
		if (newAngle > 2 * PI)
		{
			newAngle -= 2 * PI;
		}
		disT = disT * cos(newAngle);			// Fix fisheye
		
		
		float lineHeight = (mapSize * 320) / disT;
		float lineOffset = 160 - lineHeight / 2 + 80;
		/*
		if (lineHeight > 320)
		{
			lineHeight = 320;
		}
		*/
		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(ray * 8 + 530, lineOffset);
		glVertex2i(ray * 8 + 530, lineHeight + lineOffset);
		glEnd();
		
		rayAngle += DR;
		if (rayAngle < 0)
		{
			rayAngle += 2 * PI;	
		}
		if (rayAngle > 2 * PI)
		{
			rayAngle -= 2 * PI;
		}
	}	
}

void movePlayer(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		playerAngle -= 0.1;
		if (playerAngle < 0)
		{
			playerAngle += 2 * PI;
		}
		playerDeltaX = cos(playerAngle) * 5; // Multiplied by 5 because the value is so small
		playerDeltaY = sin(playerAngle) * 5;
	}
	if (key == 'd')
	{
		playerAngle += 0.1;
		if (playerAngle > 2 * PI)
		{
			playerAngle -= 2 * PI;
		}
		playerDeltaX = cos(playerAngle) * 5; // Multiplied by 5 because the value is so small
		playerDeltaY = sin(playerAngle) * 5;
	}
	if (key == 'w')
	{
		playerX += playerDeltaX;
		playerY += playerDeltaY;
	}
	if (key == 's')
	{
		playerX -= playerDeltaX;
		playerY -= playerDeltaY;
	}
	glutPostRedisplay();		
}

void display()
{
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	drawMap2D();
 	drawRays2D();
 	drawPlayer();
 	glutSwapBuffers();
}

void init()
{
	glClearColor(0.1, 0.9, 0.1, 0.0);
	gluOrtho2D(0, 1024, 512, 0);
	playerX = 100;
	playerY = 100;
	playerDeltaX = cos(playerAngle) * 5;
	playerDeltaY = sin(playerAngle) * 5;
}

int main(int argc, char** argv)
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
 glutInitWindowSize(1024,512);
 glutCreateWindow("Maze Project");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(movePlayer);
 glutMainLoop();
 return 0;
}
