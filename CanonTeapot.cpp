#include "RGBpixmap.h"
RGBpixmap pix[6];
#include "camera.h"
#include <ctime>
#include <string>
Camera cam;
Vector3 pos, target;
Vector3 eyevector;
#include<iostream>
#include<cstdlib>
#include <gl/glut.h>
using namespace std; 
int currentsphere = 0;
float yaw_range = 0.0,pitch_range = 0.0;
int teapot;
float rotX = 0.0, rotY = 0.0;  GLfloat canonX, canonY, canonZ;
float transx = 0.0, transy = 0.0, transz = 0.0, spheredistance;
float sphere_curx, sphere_cury, sphere_curz;
Vector3 sphere;
bool collision[3][5]; bool collision_fly_out[3][5]; 
bool endofgame;
float Teapot[3][5][2];
int spacebar_pressed;
GLfloat cylinder_matrix[16], sphere_matrix[10][16];
float x[3][5], y[3][5], z[3][5]; //Array to store X,Y,Z coordinate positions of Teapot in row1,row2,row3 respectively.
GLfloat yRotn[3][5]; // Rotation Angles for Tea pots

GLfloat sphereX[10], sphereY[10], sphereZ[10];

void *font = GLUT_BITMAP_TIMES_ROMAN_24;
char const *pchar;
char  *teapotremaining = "Teapots Remaining : ";
char *text_toappear;
int direction_flying;

void printBitmapString(void *f, const char* s)
{
	while (*s) {
		glutBitmapCharacter(f, *s);
		s++;
	}
}
void write_string(const char *s)
{
	glDisable(GL_LIGHTING);
	glColor3d(1, 0, 1); // font color
	glRasterPos3f(0, 0, 0); // world origin
	printBitmapString(font, s);

	glEnable(GL_LIGHTING);
}

void myInit(void)
{


	Point3 cylinder_pos = cam.geteye();//set canon near eye from 0.05 distance over y an 0.01 distance over z
	canonX = cylinder_pos.x; canonY = cylinder_pos.y - 0.05; canonZ = cylinder_pos.z - 0.1; spheredistance = 0.15;
	teapot = 15;//count
	spacebar_pressed = 0;
	endofgame = false; direction_flying = 0;
	glClearColor(1.0, 1.0, 1.0, 0.0);       // set white background color
	x[0][0] = 0.1; x[0][1] = 0.3; x[0][2] = 0.5; x[0][3] = 0.7; x[0][4] = 0.9;//Initial X coordinate positions for Teapots in Row1
	x[1][0] = 0.1; x[1][1] = 0.3; x[1][2] = 0.5; x[1][3] = 0.7; x[1][4] = 0.9;//Initial X coordinate positions for Teapots in Row2
	x[2][0] = 0.1; x[2][1] = 0.3; x[2][2] = 0.5; x[2][3] = 0.7; x[2][4] = 0.9;//Initial X coordinate positions for Teapots in Row3
	for (int i = 0; i < 5; i++) { y[0][i] = 0.38; y[1][i] = 0.58; y[2][i] = 0.78; } //Initial Y coordinate for Teapots row wise
	for (int i = 0; i < 3; i++)for (int j = 0; j < 5; j++) { z[i][j] = 0.5; }
	for (int i = 0; i < 3; i++)for (int j = 0; j < 5; j++) { collision[i][j] = false; collision_fly_out[i][j] = false; }
	yRotn[0][0] = 30; yRotn[0][1] = 120; yRotn[0][2] = 290; yRotn[0][3] = 60; yRotn[0][4] = 150;//Initial rotation angle for Teapots in Row1 
	yRotn[1][0] = 45; yRotn[1][1] = 180; yRotn[1][2] = 335; yRotn[1][3] = 95; yRotn[1][4] = 225; //Initial rotation angle for Teapots in Row2 
	yRotn[2][0] = 20; yRotn[2][1] = 250; yRotn[2][2] = 110; yRotn[2][3] = 45; yRotn[2][4] = 160; //Initial rotation angle for Teapots in Row3 
	pix[1].readBMPFile("mandrill.bmp");  // make pixmap from image
	pix[1].setTexture(2002);		// create texture


}


//wall 
void wall(double thickness)
{

	// draw thin wall with top = xz plane and with corner at origin
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);//translation done second
	glScaled(1.0, thickness, 1.0); //scaling done first
	glutSolidCube(1.0); //draw a solid cube 
	glPopMatrix();
}
void setCanonTransformation()
{
	//cout << "inside canon";
	glTranslatef(canonX, canonY, canonZ);
	glRotatef(yaw_range, 0.0, 0.1, 0.0);
	glRotatef(pitch_range, 1.0, 0.0, 0.0);
	glRotatef(180.0, 0.0, 1.0, 0.0);
}

void set_ball()//Ball positions at canon position
{

	glPushMatrix();
	glLoadIdentity();
	setCanonTransformation();
	glGetFloatv(GL_MODELVIEW_MATRIX, sphere_matrix[currentsphere]);
	glPopMatrix();
	sphereX[currentsphere] = sphere_matrix[currentsphere][8] * spheredistance + sphere_matrix[currentsphere][12];
	sphereY[currentsphere] = sphere_matrix[currentsphere][9] * spheredistance + sphere_matrix[currentsphere][13];
	sphereZ[currentsphere] = sphere_matrix[currentsphere][10] * spheredistance + sphere_matrix[currentsphere][14];
	//cout << "\nCurrent Sphere" << sphereX[currentsphere] << " " << sphereY[currentsphere] << " " << sphereZ[currentsphere];
}
void draw_ball()
{
	GLfloat color[4];
	color[0] = 0.5f, color[1] = 0.5f, color[2] = 0.5f, color[3] = 0.0f;//Black
	glMaterialfv(GL_FRONT, GL_DIFFUSE, color);

	glPushMatrix();
	//cout << sphereX[currentsphere] << " " << sphereY[currentsphere] << " " << sphereZ[currentsphere] << " ";
	glTranslated(sphereX[currentsphere], sphereY[currentsphere], sphereZ[currentsphere]);
	glutSolidSphere(0.02, 50, 50);//Solid Sphere //make the sphere size smaller 
	glPopMatrix();

}
void CollisionDetection()//find distance and calaculate radii 
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			//cout << " " << sphereX[currentsphere] << " " << sphereY[currentsphere] << " " << sphereZ[currentsphere]<<"\n";
			//cout << x[i][j] << "  " << y[i][j] << "  " << z[i][j];
			float res = sqrt(pow(sphereX[currentsphere] - x[i][j], 2.0) + pow(sphereY[currentsphere] - y[i][j], 2.0) + pow(sphereZ[currentsphere] - 0.5, 2.0));
			float sum_of_radii = 0.05 + 0.02;
			//cout << "\nDistance between them is" << res<<" "<<sum_of_radii<<"\n";
			if (res <= sum_of_radii)
			{
				collision[i][j] = true;
				//cout << "\nSphere" << i << " " << j << "Collision Detected";
			}
		}
	}
}
void setroom() {
	//Construction of Walls.
	GLfloat mat_diffuse[4];
	mat_diffuse[0] = 1.00f, mat_diffuse[1] = 0.00f, mat_diffuse[2] = 0.0f, mat_diffuse[3] = 0.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse); //set color by diffusing them
															  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
															  //wall(0.02); // wall #1: in xz-plane (TOP WALL)
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 2002); 	// Top wall: mandrill
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 1.0); glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0, 0); glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();
	wall(0.02);                  // wall #1: in xz-plane (TOP WALL)

								 //bottom wall
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, 2002); 	// Bottom wall: mandrill
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glRotated(180.0, 1.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0, 1.0); glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0, 0); glVertex3f(1.0f, 0.0f, 1.0f);
	glEnd();

	glTranslated(0.0, 1.0, 0.0);

	wall(0.02);                  // wall #2: in dist  xz-plane (BOTTOM WALL)
								 //glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	//left wall
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, 2002); 	// leftface: mandrill
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0, 1.0); glVertex3f(0.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0, 1.0); glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0, 0); glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRotated(90.0, 0.0, 0.0, 1.0);
	wall(0.02); // wall #3: in yz-plane (LEFT WALL)	
	glPopMatrix();
	//RIGHT WALL
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, 2002); 	// RIGHT face: mandrill
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0, 1.0); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0); glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
	glTranslated(1.0, 0.0, 0.0);    //translation done second
	glRotated(90.0, 0.0, 0.0, 1.0); //Rotation done first
	glRotated(180.0, 0.0, 0.0, 1.0);
	wall(0.02);                  // wall #4: dist in yz-plane (RIGHT WALL)
								 //	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, 2002); 	// CENTER wall: mandrill
	glBegin(GL_QUADS);

	glTexCoord2f(-1.0, -1.0); glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(-1.0, 2.0); glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(2.0, 2.0); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(2.0, -1.0); glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
	glRotated(-90.0, 1.0, 0.0, 0.0);
	wall(0.02);                  // wall #5: in xy-plane (CENTER WALL)

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);//glDisable(GL_TEXTURE_2D);
}
//display Flying teapot
void displayFlyingTeapot(void)
{
	GLUquadric* qobj;
	qobj = gluNewQuadric();
	//set properties of the surface material
	GLfloat mat_ambient[] = { 0.0f,0.0f,0.0f, 1.0f }; //define ambience  values
	GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };//define diffuse values
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };//define specular values
	GLfloat mat_shininess[] = { 50.0f };//define shininess value
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);//set Ambience for front portion of material
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);//Set Diffuse for front portion of material
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);//Set Specular for front postion of material
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);//Set Shinniness for front position of material

	glEnable(GL_COLOR_MATERIAL); //Enable Material Color
	glColorMaterial(GL_FRONT, GL_DIFFUSE); //Set Material Color with Diffuse
	glColorMaterial(GL_FRONT, GL_SPECULAR);//Set Material Color with Specular


										   // set the light source properties
	GLfloat lightIntensity0[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //Intensity of Light 1
	GLfloat light_position0[] = { 1.0F, -5.0f, 0.8f, 1.0f }; //Position of Light 1
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0); //set Light 1 position
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity0);  //set Light 1 intensity

	GLfloat lightIntensity1[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //Intensity of Light 2
	GLfloat light_position1[] = { 0.0f, 5.0f, 0.8f, 1.0f }; //Position of Light 2
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);     //set Light 1 position
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightIntensity1);      //set Light 2 intensity

	std::string s = std::to_string(teapot);//teapot remaining count
	pchar = s.c_str();//convert integer to string
					  //cout << pchar;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen

	setroom();
	if (collision_fly_out[0][0] != true)
	{
		mat_diffuse[0] = 0.85f, mat_diffuse[1] = 0.53f, mat_diffuse[2] = 0.1f, mat_diffuse[3] = 0.0f;//Copper 
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse); //diffuse color for each teapot
		glPushMatrix();
		glTranslated(x[0][0], y[0][0], z[0][0]); //Moves leftwards with translation in X coordinates.
		glRotated(yRotn[0][0], 0, 1, 0);   //Rotation along its up vector(y axis)
		glutSolidTeapot(0.05);        // draw the teapot row1 col1
		glPopMatrix();
	}

	if (collision_fly_out[0][1] != true)
	{
		mat_diffuse[0] = 0.0f, mat_diffuse[1] = 0.0f, mat_diffuse[2] = 1.0f, mat_diffuse[3] = 0.0f;//Blue
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[0][1], y[0][1], z[0][1]);
		glRotated(yRotn[0][1], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot col2
		glPopMatrix();
	}

	if (collision_fly_out[0][2] != true)
	{
		mat_diffuse[0] = 1.0f, mat_diffuse[1] = 1.0f, mat_diffuse[2] = 0.0f, mat_diffuse[3] = 0.0f;//Yelllow
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[0][2], y[0][2], z[0][2]);
		glRotated(yRotn[0][2], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row1 col3
		glPopMatrix();
	}

	if (collision_fly_out[0][3] != true)
	{
		mat_diffuse[0] = 0.71f, mat_diffuse[1] = 0.65f, mat_diffuse[2] = 0.26f, mat_diffuse[3] = 0.0f;//bronze
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[0][3], y[0][3], z[0][3]);
		glRotated(yRotn[0][3], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row1 col4
		glPopMatrix();
	}
	if (collision_fly_out[0][4] != true)
	{
		mat_diffuse[0] = 0.0f, mat_diffuse[1] = 1.0f, mat_diffuse[2] = 1.0f, mat_diffuse[3] = 0.0f;//Cyan
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[0][4], y[0][4], z[0][4]);
		glRotated(yRotn[0][4], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row1 col5
		glPopMatrix();
	}
	if (collision_fly_out[1][0] != true)
	{
		mat_diffuse[0] = 1.0f, mat_diffuse[1] = 0.50f, mat_diffuse[2] = 0.0f, mat_diffuse[3] = 0.0f;//orange
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		//2nd row
		glPushMatrix();
		glTranslated(x[1][0], y[1][0], z[1][0]);
		glRotated(yRotn[1][0], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row2 col1
		glPopMatrix();
	}
	if (collision_fly_out[1][1] != true)
	{
		mat_diffuse[0] = 1.5f, mat_diffuse[1] = 1.0f, mat_diffuse[2] = 0.96f, mat_diffuse[3] = 0.0f;//pink
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[1][1], y[1][1], z[1][1]);
		glRotated(yRotn[1][1], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row2 col2
		glPopMatrix();
	}
	if (collision_fly_out[1][2] != true)
	{
		mat_diffuse[0] = 0.73f, mat_diffuse[1] = 0.16f, mat_diffuse[2] = 0.96f, mat_diffuse[3] = 0.0f;//purple
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[1][2], y[1][2], z[1][2]);
		glRotated(yRotn[1][2], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row2 col3
		glPopMatrix();
	}
	if (collision_fly_out[1][3] != true)
	{
		mat_diffuse[0] = 1.0f, mat_diffuse[1] = 0.11f, mat_diffuse[2] = 0.19f, mat_diffuse[3] = 0.0f;//plum
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[1][3], y[1][3], z[1][3]);
		glRotated(yRotn[1][3], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row2 col4
		glPopMatrix();
	}
	if (collision_fly_out[1][4] != true)
	{
		mat_diffuse[0] = 0.5f, mat_diffuse[1] = 1.0f, mat_diffuse[2] = 0.0f, mat_diffuse[3] = 0.0f;//lime green
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[1][4], y[1][4], z[1][4]);
		glRotated(yRotn[1][4], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row2 col5
		glPopMatrix();

	}
	//3rd row
	if (collision_fly_out[2][0] != true)
	{
		mat_diffuse[0] = 0.1f, mat_diffuse[1] = 0.41f, mat_diffuse[2] = 0.55f, mat_diffuse[3] = 0.0f; //steel blue
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[2][0], y[2][0], z[2][0]);
		glRotated(yRotn[2][0], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row3 col1
		glPopMatrix();
	}
	if (collision_fly_out[2][1] != true)
	{
		mat_diffuse[0] = 1.0f, mat_diffuse[1] = 0.0f, mat_diffuse[2] = 1.0f, mat_diffuse[3] = 0.0f; //violet
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[2][1], y[2][1], z[2][1]);
		glRotated(yRotn[2][1], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row3 col2
		glPopMatrix();
	}
	if (collision_fly_out[2][2] != true)
	{
		mat_diffuse[0] = 0.25f, mat_diffuse[1] = 0.25f, mat_diffuse[2] = 0.43f, mat_diffuse[3] = 0.0f; //Corn flower blue
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[2][2], y[2][2], z[2][2]);
		glRotated(yRotn[2][2], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row3 col3
		glPopMatrix();
	}
	if (collision_fly_out[2][3] != true)
	{
		mat_diffuse[0] = 0.52f, mat_diffuse[1] = 1.0f, mat_diffuse[2] = 1.0f, mat_diffuse[3] = 0.0f; //light blue
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[2][3], y[2][3], z[2][3]);
		glRotated(yRotn[2][3], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row3 col4
		glPopMatrix();
	}
	if (collision_fly_out[2][4] != true)
	{
		mat_diffuse[0] = 0.0f, mat_diffuse[1] = 0.5f, mat_diffuse[2] = 0.0f, mat_diffuse[3] = 0.0f;//forest green
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		glTranslated(x[2][4], y[2][4], z[2][4]);
		glRotated(yRotn[2][4], 0, 1, 0);
		glutSolidTeapot(0.05);        // draw the teapot row3 col5
		glPopMatrix();
	}


	mat_diffuse[0] = 0.0f, mat_diffuse[1] = 0.0f, mat_diffuse[2] = 1.0f, mat_diffuse[3] = 0.0f;//Blue
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glPushMatrix();
	setCanonTransformation();
	GLUquadricObj * qobj1;
	qobj1 = gluNewQuadric();
	gluCylinder(qobj1, 0.01, 0.01, 0.15, 50, 50);//Solid Cylinder
	glPopMatrix();

	if (spacebar_pressed) {
		draw_ball();
	}
	if (endofgame != true) {
		glPushMatrix();
		glTranslated(0.1, 0.9, 0.3);
		write_string(teapotremaining);
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.7, 0.9, 0.3);
		write_string(pchar);//Number Of Teapots remaining.
		glPopMatrix();

	}
	if (endofgame) {
		glPushMatrix();
		glTranslated(0.1, 0.8, 0.3);
		write_string("Continue?(Y/N)");
		glPopMatrix();

	}
	glFlush();
	glutSwapBuffers();
}
void idleFunc(void) //When Idle this fucntion is called
{
	//glGetFloatv(GL_MODELVIEW_MATRIX, cylinder_matrix);

	for (int i = 0; i < 5; i++)
	{
		x[0][i] += 0.001; x[2][i] += 0.001;//Row1 and Row3 Teapots moves towards right
		x[1][i] -= 0.001;//Row 2 Teapots moves towards left

		if (x[0][i] >= 1.00) //Reposition the pot on reaching right edge of wall coordinate
		{
			x[0][i] = 0.0;  //reposition to  left beginning coordinate inside wall
		}
		if (x[1][i] <= 0.0) //Reposition the pot on reaching left edge of wall coordinate
		{
			x[1][i] = 1.0; //reposition to right end coordinate inside the wall 
		}
		if (x[2][i] >= 1.0) //Reposition the pot on reaching right edge of wall coordinate
		{
			x[2][i] = 0.0;  //reposition to left beginning coordinate inside wall
		}
	}
	for (int i = 0; i < 3; i++)for (int j = 0; j < 5; j++)
	{
		if (collision[i][j] == true)//teapot translation while collision detected
		{
			if (y[i][j] <= 1)
			{

				y[i][j] += 0.03;
				z[i][j] -= 0.01;

				//	cout <<"\nonly when collision detected and ball flies\n";
			}
			else {
				collision_fly_out[i][j] = true; collision[i][j] = false; spacebar_pressed = 0;
				//	cout << "current sphere value" << currentsphere << "\n";
				teapot -= 1;
				if (teapot == 0) {
					endofgame = "true";
				}
			}
		}
	}
	//the speed of spinning for each teapot
	for (int j = 0; j < 5; j++)
	{
		yRotn[0][j] += 1.25;//Rotates counter clockwise
		yRotn[2][j] += 1.25;//Rotates counter clockwise
		yRotn[1][j] -= 1.25;//Rotates clockwise
	}
	//SpacebaR:
	if (spacebar_pressed)
	{//ball movement 
	 //	cout << "idle part of space bar";
		if (yaw_range == 0) { sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 5) { sphereX[currentsphere] -= 0.001; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 10) { sphereX[currentsphere] -= 0.0025; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 15) { sphereX[currentsphere] -= 0.004; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -5) { sphereX[currentsphere] += 0.001; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -10) { sphereX[currentsphere] += 0.0025; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -15) { sphereX[currentsphere] += 0.003; sphereZ[currentsphere] -= 0.01; }

		if (yaw_range == 0 && pitch_range == 5) { sphereY[currentsphere] -= 0.001; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 5 && pitch_range == 5) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] -= 0.001; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 10 && pitch_range == 5) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] -= 0.001; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 15 && pitch_range == 5) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] -= 0.001; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -5 && pitch_range == 5) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] -= 0.001;  sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -10 && pitch_range == 5) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] -= 0.001; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -15 && pitch_range == 5) { sphereX[currentsphere] += 0.001; sphereY[currentsphere] -= 0.001;  sphereZ[currentsphere] -= 0.01; }

		if (yaw_range == 0 && pitch_range == 10) { sphereY[currentsphere] += 0.0012; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 5 && pitch_range == 10) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] += 0.0012; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 10 && pitch_range == 10) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] += 0.0012; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 15 && pitch_range == 10) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] += 0.0012; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -5 && pitch_range == 10) { sphereX[currentsphere] += 0.001; sphereY[currentsphere] += 0.0012;  sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -10 && pitch_range == 10) { sphereX[currentsphere] += 0.001; sphereY[currentsphere] += 0.0012; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -15 && pitch_range == 10) { sphereX[currentsphere] += 0.001; sphereY[currentsphere] += 0.0012;  sphereZ[currentsphere] -= 0.01; }

		if (yaw_range == 0 && pitch_range == 15) { sphereY[currentsphere] += 0.003; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 5 && pitch_range == 15) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] += 0.003; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 10 && pitch_range == 15) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] += 0.003; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == 15 && pitch_range == 15) { sphereX[currentsphere] -= 0.001; sphereY[currentsphere] += 0.003; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -5 && pitch_range == 15) { sphereX[currentsphere] += 0.001; sphereY[currentsphere] += 0.003;  sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -10 && pitch_range == 15) { sphereX[currentsphere] += 0.001; sphereY[currentsphere] += 0.003; sphereZ[currentsphere] -= 0.01; }
		if (yaw_range == -15 && pitch_range == 15) { sphereX[currentsphere] += 0.001; sphereY[currentsphere] += 0.003;  sphereZ[currentsphere] -= 0.01; }

		CollisionDetection();


	}

	displayFlyingTeapot();//calls back displayfunction
}

static void Key(unsigned char key, int x, int y)
{
	switch (key) {
	case 32:
		currentsphere = (currentsphere + 1) % 10; spacebar_pressed = 1;
		set_ball(); displayFlyingTeapot(); break;
	case 'y':if (endofgame) {
		myInit(); displayFlyingTeapot();
	} break;
	case 'n':if (endofgame) {
		exit(0);
	}break;
	case 'q':
		exit(0);
	}
}

void timer(int extra)
{
	glutPostRedisplay();
	glutTimerFunc(12000, timer, 0);
}

static void SpecialKey(int key, int x, int y)
{
	switch (key) {
		// controls for camera

	case GLUT_KEY_UP:	if (pitch_range <15) {
		///cout << "After up";
		pitch_range += 5;
	}break;//3 times is enough 
	case GLUT_KEY_DOWN:	if (pitch_range >0) {

		pitch_range -= 5;

	} break; //1 time is enough.

	case GLUT_KEY_LEFT:if (yaw_range< 15)
	{
		//cout << "After left";
		yaw_range += 5;


	} break;
	case GLUT_KEY_RIGHT: if (yaw_range > -15)
	{

		//cout << "After right";
		yaw_range -= 5;

	} break;//limit to our control.
	}

	glutPostRedisplay(); // draw it again
}



//<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void main(int argc, char **argv)
{
	glutInit(&argc, argv);// initialize the toolkit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//Set the Display Buffer
	glutInitWindowSize(640, 480);//Set the size of the Window
	glutInitWindowPosition(50, 50);//set the position for Window
	glutCreateWindow("Flying  Teapots");
	cam.setShape(23.0f, 64.0f / 48.0f, 0.01f, 5.0f);
	Point3 eye(0.5, 0.5, 3.0);
	Point3 look(0.5, 0.5, 0.5);
	Vector3 up(0, 1, 0);
	cam.set(eye, look, up); // make the initial camera
	cam.seteye(eye);//set eye value

	myInit();
	glEnable(GL_DEPTH_TEST);
	Vector3 eyevector(eye.x, eye.y, eye.z);
	glutKeyboardFunc(Key);
	glutSpecialFunc(SpecialKey);
	glutTimerFunc(0, timer, 0);
	glutDisplayFunc(displayFlyingTeapot);//Calls the display funtion
	glEnable(GL_LIGHTING); // Enable the light source
	glEnable(GL_LIGHT0); glEnable(GL_LIGHT1);//Light0 and Light1 sources are enabled
	glShadeModel(GL_SMOOTH);//For Smooth Shading
	glEnable(GL_DEPTH_TEST); // For hidden surface removal
	glEnable(GL_NORMALIZE); // Normalize vectors for proper shading
	glEnable(GL_COLOR_MATERIAL); //Enable the Material Coloring
	glutIdleFunc(idleFunc);//Calls the Idle Function 
	glViewport(0, 0, 640, 480);//Viewport Coordinates
	glutMainLoop();//Go into a perpetual loop
}


