#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

//#include "glew.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "glut.h"

// Sphere-drawing function
#include "osusphere.cpp"
#include "distortedsphere.cpp"

// OBJ
#include "loadobjfile.cpp"


//	Author:			Cheng Ying Wu
//  Course:         CS 450
//  Project:        Final Project
//
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.

// title of these windows:

const char *WINDOWTITLE = { "CS 450 Final Project -- Cheng Ying Wu" };
const char *GLUITITLE   = { "User Interface Window" };

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };

// the escape key:

#define ESCAPE		0x1b

// initial window size:

const int INIT_WINDOW_SIZE = { 1000 };

// size of the 3d box:

const float BOXSIZE = { 1.f };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = { 3 };
const int SCROLL_WHEEL_DOWN = { 4 };

// equivalent mouse movement when we click a the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = { 5. };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };

// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };



// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
GLuint	BoxList;				// object display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

// OSU Torus
GLuint  OSUTorus;               // Torus: Use the code provided (A torus-drawing function)

// Control points & lines & views
int		PointsOn;		        // != 0 means to turn the control points on
int		LinesOn;		        // != 0 means to turn the control lines on
int     Views;                  // various integers mean different views from distinct positions


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoShadowMenu();
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
unsigned char *	BmpToTexture( char *, int *, int * );
void			HsvRgb( float[3], float [3] );
int				ReadInt( FILE * );
short			ReadShort( FILE * );

void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);

// new added functions for lighting
float*  Array3( float, float, float );
float*  MulArray3( float, float );
void    SetMaterial( float, float, float, float );
void    SetPointLight( int, float, float, float, float, float, float ); 
void    SetSpotLight( int, float, float, float, float, float, float, float, float, float );

 // declare for lights
bool    Light0On, Light1On, Light2On, Light3On, Light4On, Light5On; 	
bool    Frozen;
float   White[ ] = { 1.,1.,1.,1. };


// Make some global variables for textures
int     level = 0;
int     ncomps = 3;
int     border = 0;
GLuint  Tex0, Tex1, Tex2, Tex3, Tex4, Tex5, Tex6;


// OBJ files
GLuint  CAR;		    // car
GLuint  CAT;            // cat
GLuint  SHIP;           // spaceship
GLuint  DEER;           // deer
GLuint  DINO;           // dino


// Added structure for Bézier curves
struct Point
{
    float x0, y0, z0;       // initial coordinates
    float x,  y,  z;        // animated coordinates
};

struct Curve
{
    float r, g, b;
    Point p0, p1, p2, p3;
};

#define NUMCURVES  12
Curve Curves[NUMCURVES];    // if you are creating a pattern of curves

// Added functions for Bézier curves & views
void    Bezier( Curve, int );
void	DoPointMenu( int );
void	DoLineMenu( int );
void	DoViewsMenu( int );


// For animation
#define PI 3.14159
#define RAD 14.f
#define RADIUS 22.f
#define MOONRAD 8.f


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display structures that will not change:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never returns
	// this line is here to make the compiler happy:

	return 0;
}


// Bezier Curve Drawing Function
void
Bezier( Curve curve, int numPoints )
{
	glColor3f( curve.r, curve.g, curve.b );
	glBegin( GL_LINE_STRIP );
		for( int it = 0; it <= numPoints; it++ )
		{
			float t = (float)it / (float)numPoints;
			float omt = 1.f - t;
			float x = omt*omt*omt*curve.p0.x + 3.f*t*omt*omt*curve.p1.x + 3.f*t*t*omt*curve.p2.x + t*t*t*curve.p3.x;
			float y = omt*omt*omt*curve.p0.y + 3.f*t*omt*omt*curve.p1.y + 3.f*t*t*omt*curve.p2.y + t*t*t*curve.p3.y;
			float z = omt*omt*omt*curve.p0.z + 3.f*t*omt*omt*curve.p1.z + 3.f*t*t*omt*curve.p2.z + t*t*t*curve.p3.z;
			glVertex3f( x, y, z );
		}
	glEnd( );
}


// lighting helper function (provided)

// utility to create an array from 3 separate values:

float*
Array3( float a, float b, float c ) 
{
	static float array[4];
	
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;

}

// utility to create an array from a multiplier and an array:

float*
MulArray3( float factor, float array0[3] )
{
	static float array[4];

	array[0] = factor * array0[0]; 
	array[1] = factor * array0[1]; 
	array[2] = factor * array0[2]; 
	array[3] = 1.;
	return array;

}

void
SetMaterial( float r, float g, float b, float shininess ) 
{
	glMaterialfv( GL_BACK, GL_EMISSION, Array3( 0., 0., 0. ) ); 
	glMaterialfv( GL_BACK, GL_AMBIENT, MulArray3( .4f, White ) ); 
	glMaterialfv( GL_BACK, GL_DIFFUSE, MulArray3( 1., White ) ); 
	glMaterialfv( GL_BACK, GL_SPECULAR, Array3( 0., 0., 0. ) ); 
	glMaterialf ( GL_BACK, GL_SHININESS, 2.f );

	glMaterialfv( GL_FRONT, GL_EMISSION, Array3( 0., 0., 0. ) ); 
	glMaterialfv( GL_FRONT, GL_AMBIENT, Array3( r, g, b ) ); 
	glMaterialfv( GL_FRONT, GL_DIFFUSE, Array3( r, g, b ) ); 
	glMaterialfv( GL_FRONT, GL_SPECULAR, MulArray3( .8f, White ) ); 
	glMaterialf ( GL_FRONT, GL_SHININESS, shininess );
}

void
SetPointLight( int ilight, float x, float y, float z, float r, float g, float b ) 
{
	glLightfv( ilight, GL_POSITION, Array3( x, y, z ) ); 
	glLightfv( ilight, GL_AMBIENT, Array3( 0., 0., 0. ) ); 
	glLightfv( ilight, GL_DIFFUSE, Array3( r, g, b ) ); 
	glLightfv( ilight, GL_SPECULAR, Array3( r, g, b ) ); 
	glLightf ( ilight, GL_CONSTANT_ATTENUATION, 1. ); 
	glLightf ( ilight, GL_LINEAR_ATTENUATION, 0. ); 
	glLightf ( ilight, GL_QUADRATIC_ATTENUATION, 0. ); 
	glEnable ( ilight );
}

void
SetSpotLight( int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b ) 
{
	glLightfv( ilight, GL_POSITION, Array3( x, y, z ) );
	glLightfv( ilight, GL_SPOT_DIRECTION, Array3(xdir,ydir,zdir) ); glLightf( ilight, GL_SPOT_EXPONENT, 1. );
	glLightf( ilight, GL_SPOT_CUTOFF, 45. );
	glLightfv( ilight, GL_AMBIENT, Array3( 0., 0., 0. ) );
	glLightfv( ilight, GL_DIFFUSE, Array3( r, g, b ) );
	glLightfv( ilight, GL_SPECULAR, Array3( r, g, b ) );
	glLightf ( ilight, GL_CONSTANT_ATTENUATION, 1. );
	glLightf ( ilight, GL_LINEAR_ATTENUATION, 0. );
	glLightf ( ilight, GL_QUADRATIC_ATTENUATION, 0. ); glEnable( ilight );
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it


void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// Use the code provided
	int ms = glutGet( GLUT_ELAPSED_TIME );
	ms %= MS_PER_CYCLE;
	Time = (float)ms / (float)MS_PER_CYCLE;		// [0.,1.)

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


	// Time variable
	float theta = (float)(2. * M_PI) * Time;

	// Views Setting
    if( Views == 0 )
    {
        // Outside View
    	// set the eye position, look-at position, and up-vector:

		gluLookAt( 15., 5., 26.,     0., 0., 0.,     0., 1., 0. );


		// rotate the scene:

		glRotatef( (GLfloat)Yrot, 0., 1., 0. );
		glRotatef( (GLfloat)Xrot, 1., 0., 0. );


		// uniformly scale the scene:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
		glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

    }
    else if( Views == 1 )
    {	
    	// View from Car (watching straight)
    	gluLookAt( 0., 1., 15.*sin(Time*2*PI) - 7.5,    0., 0., -28.,     0., 1., 0. );
    }
    else if( Views == 2 )
    {	
    	// View from Spaceship (watch the origin)
    	gluLookAt( (float)(15.5 * cos(theta)), 2., (float)(15.5 * sin(theta)),    0., 0., 0.,     0., 1., 0. );
    }
    else if( Views == 3 )
    {	
    	// View from Earth (watch sun)
    	gluLookAt( (float)(RADIUS * cos(theta)), 4.2, (float)(RADIUS * sin(theta)) + 25.,    0., 0., 25.,     0., 1., 0. );
    }
    else if( Views == 4 )
    {	
    	// Cat Star's Inside View
    	gluLookAt( 0., 3.0, -31.,     0., 0., 0.,     0., 1., 0. );
    }
    else if( Views == 5 )
    {	
    	// View from Green Star (watch cat star)
    	gluLookAt( (float)(RAD * sin(theta)), 3.2, (float)(RAD * cos(theta)) - 28.,    0., 0., -28.,     0., 1., 0. );
    }
    else if( Views == 6 )
    {
    	// Green Star's Inside View
    	gluLookAt( (float)(RAD * sin(theta)), 2.5, (float)(RAD * cos(theta)) - 28.,    0., 0., -28.,     0., 1., 0. );
    }
    else if( Views == 7 )
    {
    	// View on Sun
    	gluLookAt( 0., 30., 25.,     0., 0., 10.,     0., 1., 0. );
    }
    else
    {
    	// Sun's Inside View
    	gluLookAt( 1.5, 2.5, 30.,     0., 0., 0.,     0., 1., 0. );
    }
    

	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}


	// For control points & lines

	if( PointsOn != 0 )
	{
		glColor3f( 1., 1., 1. );
		glPointSize( 5.0 );
		for ( int i = 0; i < NUMCURVES; i++)
		{
			glBegin( GL_POINTS );
			glVertex3f( Curves[i].p0.x, Curves[i].p0.y, Curves[i].p0.z ); 
		    glVertex3f( Curves[i].p1.x, Curves[i].p1.y, Curves[i].p1.z ); 
		    glVertex3f( Curves[i].p2.x, Curves[i].p2.y, Curves[i].p2.z ); 
		    glVertex3f( Curves[i].p3.x, Curves[i].p3.y, Curves[i].p3.z ); 
			glEnd( );
	
		} 
		
	}

	if( LinesOn != 0 )
	{
	    glColor3f( 1., 1., 1. ); 
	    glLineWidth( 1. );
	    for ( int i = 0; i < NUMCURVES; i++)
		{
			glBegin( GL_LINE_STRIP );
			glVertex3f( Curves[i].p0.x, Curves[i].p0.y, Curves[i].p0.z ); 
		    glVertex3f( Curves[i].p1.x, Curves[i].p1.y, Curves[i].p1.z ); 
		    glVertex3f( Curves[i].p2.x, Curves[i].p2.y, Curves[i].p2.z ); 
		    glVertex3f( Curves[i].p3.x, Curves[i].p3.y, Curves[i].p3.z ); 
			glEnd( );
		} 
	}



	// since we are using glScalef( ), be sure normals get unitized:

	glEnable( GL_NORMALIZE );


	// draw the Bézier Curves

    // Curve 1
    Curves[0].r = 1.;
    Curves[0].g = 0.;
    Curves[0].b = 0.;

    Curves[0].p0.x = 1.;
    Curves[0].p0.y = 0.;
    Curves[0].p0.z = 0.;

    Curves[0].p1.x = 1.5 + 2 * Time;
    Curves[0].p1.y = -1. - 2 * Time;
    Curves[0].p1.z = 0.;

    Curves[0].p2.x = 1. + 2 * Time;
    Curves[0].p2.y = -1.5 - 2 * Time;
    Curves[0].p2.z = 0.;

    Curves[0].p3.x = 0.;
    Curves[0].p3.y = -1.;
    Curves[0].p3.z = 0.;

	glPushMatrix();
	glLineWidth( 4. );
	Bezier( Curves[0], 20 );
	glPopMatrix();

	// Curve 2
    Curves[1].r = 1.;
    Curves[1].g = 0.5;
    Curves[1].b = 0.;

    Curves[1].p0.x = 1.;
    Curves[1].p0.y = 0.;
    Curves[1].p0.z = 0.;

    Curves[1].p1.x = 1.5 + 2 * Time;
    Curves[1].p1.y = 1. + 2 * Time;
    Curves[1].p1.z = 0.;

    Curves[1].p2.x = 1. + 2 * Time;
    Curves[1].p2.y = 1.5 + 2 * Time;
    Curves[1].p2.z = 0.;

    Curves[1].p3.x = 0.;
    Curves[1].p3.y = 1.;
    Curves[1].p3.z = 0.;

	glPushMatrix();
	glLineWidth( 4. );
	Bezier( Curves[1], 20 );
	glPopMatrix();

	// Curve 3
    Curves[2].r = 0.9;
    Curves[2].g = 0.;
    Curves[2].b = 0.7;

    Curves[2].p0.x = 0.;
    Curves[2].p0.y = 1.;
    Curves[2].p0.z = 0.;

    Curves[2].p1.x = -1. - 2 * Time;
    Curves[2].p1.y = 1.5 + 2 * Time;
    Curves[2].p1.z = 0.;

    Curves[2].p2.x = -1.5 - 2 * Time;
    Curves[2].p2.y = 1. + 2 * Time;
    Curves[2].p2.z = 0.;

    Curves[2].p3.x = -1.;
    Curves[2].p3.y = 0.;
    Curves[2].p3.z = 0.;

	glPushMatrix();
	glLineWidth( 4. );
	Bezier( Curves[2], 20 );
	glPopMatrix();

	// Curve 4
    Curves[3].r = 0.9;
    Curves[3].g = 0.5;
    Curves[3].b = 1.;

    Curves[3].p0.x = -1.;
    Curves[3].p0.y = 0.;
    Curves[3].p0.z = 0.;

    Curves[3].p1.x = -1.5 - 2 * Time;
    Curves[3].p1.y = -1. - 2 * Time;
    Curves[3].p1.z = 0.;

    Curves[3].p2.x = -1. - 2 * Time;
    Curves[3].p2.y = -1.5 - 2 * Time;
    Curves[3].p2.z = 0.;

    Curves[3].p3.x = 0.;
    Curves[3].p3.y = -1.;
    Curves[3].p3.z = 0.;

	glPushMatrix();
	glLineWidth( 4. );
	Bezier( Curves[3], 20 );
	glPopMatrix();

	// Curve 5
    Curves[4].r = 0.4;
    Curves[4].g = 0.7;
    Curves[4].b = 0.;

    Curves[4].p0.x = 1.;
    Curves[4].p0.y = 0.;
    Curves[4].p0.z = -6.;

    Curves[4].p1.x = 1.5 + Time;
    Curves[4].p1.y = -1. - Time;
    Curves[4].p1.z = -6.;

    Curves[4].p2.x = 1. + Time;
    Curves[4].p2.y = -1.5 - Time;
    Curves[4].p2.z = -6.;

    Curves[4].p3.x = 0.;
    Curves[4].p3.y = -1.;
    Curves[4].p3.z = -6.;

	glPushMatrix();
	glLineWidth( 4. );
	Bezier( Curves[4], 20 );
	glPopMatrix();

	// Curve 6
    Curves[5].r = 0.;
    Curves[5].g = 0.;
    Curves[5].b = 1.;

    Curves[5].p0.x = 1.;
    Curves[5].p0.y = 0.;
    Curves[5].p0.z = -6.;

    Curves[5].p1.x = 1.5 + Time;
    Curves[5].p1.y = 1. + Time;
    Curves[5].p1.z = -6.;

    Curves[5].p2.x = 1. + Time;
    Curves[5].p2.y = 1.5 + Time;
    Curves[5].p2.z = -6.;

    Curves[5].p3.x = 0.;
    Curves[5].p3.y = 1.;
    Curves[5].p3.z = -6.;

	glPushMatrix();
	glLineWidth( 4. );
	Bezier( Curves[5], 20 );
	glPopMatrix();

	// Curve 7
    Curves[6].r = 0.4;
    Curves[6].g = 0.1;
    Curves[6].b = 1.;

    Curves[6].p0.x = 0.;
    Curves[6].p0.y = 1.;
    Curves[6].p0.z = -6.;

    Curves[6].p1.x = -1. - Time;
    Curves[6].p1.y = 1.5 + Time;
    Curves[6].p1.z = -6.;

    Curves[6].p2.x = -1.5 - Time;
    Curves[6].p2.y = 1. + Time;
    Curves[6].p2.z = -6.;

    Curves[6].p3.x = -1.;
    Curves[6].p3.y = 0.;
    Curves[6].p3.z = -6.;

	glPushMatrix();
	glLineWidth( 4. );
	Bezier( Curves[6], 20 );
	glPopMatrix();

	// Curve 8
    Curves[7].r = 0.5;
    Curves[7].g = 0.7;
    Curves[7].b = 1.;

    Curves[7].p0.x = -1.;
    Curves[7].p0.y = 0.;
    Curves[7].p0.z = -6.;

    Curves[7].p1.x = -1.5 - Time;
    Curves[7].p1.y = -1. - Time;
    Curves[7].p1.z = -6.;

    Curves[7].p2.x = -1. - Time;
    Curves[7].p2.y = -1.5 - Time;
    Curves[7].p2.z = -6.;

    Curves[7].p3.x = 0.;
    Curves[7].p3.y = -1.;
    Curves[7].p3.z = -6.;

	glPushMatrix();
	glLineWidth( 4. );
	Bezier( Curves[7], 20 );
	glPopMatrix();


	glPushMatrix();
	glLineWidth( 4. );
	// Curve 9
    Curves[8].r = 0.8;
    Curves[8].g = 1.;
    Curves[8].b = 0.2;

    Curves[8].p0.x = -Time;
    Curves[8].p0.y =  Time;
    Curves[8].p0.z = -3.3;

    Curves[8].p1.x = -3.;
    Curves[8].p1.y =  3.;
    Curves[8].p1.z = 0.3 * sin( 2*PI*Time) - 3.;

    Curves[8].p2.x = 3.;
    Curves[8].p2.y = 3.;
    Curves[8].p2.z = 0.3 * sin( 2*PI*Time) - 3.;

    Curves[8].p3.x =  Time;
    Curves[8].p3.y =  Time;
    Curves[8].p3.z = -3.3;
	Bezier( Curves[8], 20 );

	// Curve 10
    Curves[9].r = 0.9;
    Curves[9].g = 1.;
    Curves[9].b = 0.8;

    Curves[9].p0.x = Time;
    Curves[9].p0.y = Time;
    Curves[9].p0.z = -3.3;

    Curves[9].p1.x = 3.;
    Curves[9].p1.y = 3.;
    Curves[9].p1.z = -0.3 * cos( 2*PI*Time) - 3.;

    Curves[9].p2.x = 3.;
    Curves[9].p2.y = -3.;
    Curves[9].p2.z = -0.3 * cos( 2*PI*Time) - 3.;

    Curves[9].p3.x =  Time;
    Curves[9].p3.y = -Time;
    Curves[9].p3.z = -3.3;
	Bezier( Curves[9], 20 );

	// Curve 11
    Curves[10].r = 0.5;
    Curves[10].g = 1.;
    Curves[10].b = 0.9;

    Curves[10].p0.x = Time;
    Curves[10].p0.y = -Time;
    Curves[10].p0.z = -3.3;

    Curves[10].p1.x = 3.;
    Curves[10].p1.y = -3.;
    Curves[10].p1.z = -0.9 * sin( 2*PI*Time) - 3.;

    Curves[10].p2.x = -3.;
    Curves[10].p2.y = -3.;
    Curves[10].p2.z = -0.9 * sin( 2*PI*Time) - 3.;

    Curves[10].p3.x = -Time;
    Curves[10].p3.y = -Time;
    Curves[10].p3.z = -3.3;
	Bezier( Curves[10], 20 );

	// Curve 12
    Curves[11].r = 0.5;
    Curves[11].g = 0.;
    Curves[11].b = 1.;

    Curves[11].p0.x = -Time;
    Curves[11].p0.y = -Time;
    Curves[11].p0.z = -3.3;

    Curves[11].p1.x = -3.;
    Curves[11].p1.y = -3.;
    Curves[11].p1.z = -0.3 * cos( 2*PI*Time) - 3.;

    Curves[11].p2.x = -3.;
    Curves[11].p2.y = 3.;
    Curves[11].p2.z = -0.3 * cos( 2*PI*Time) - 3.;

    Curves[11].p3.x = -Time;
    Curves[11].p3.y =  Time;
    Curves[11].p3.z = -3.3;
	Bezier( Curves[11], 20 );
	glPopMatrix();


	// small spheres (for the light source)

	glPushMatrix();
	glTranslatef( 0., 15., -7. );
	glColor3f( 1., 1., 1. ); 
	OsuSphere( 0.3, 100, 100);   // 0: white spot light at the position ( 0., 15., -7. )
	glPopMatrix();

	glPushMatrix();
	glTranslatef( 0., 0., -28. );
	glColor3f( 0.5, 0., 1. ); 
	OsuSphere( 0.3, 100, 100);   // 1: purple point light at the position ( 0., 0., -28. );
	glPopMatrix();

	// Car yellow headlights
	glPushMatrix();
	glTranslatef(-0.4, 0., 15.*sin(Time*2*PI) );
	glTranslatef( 0., 0., -9.65 );
	glColor3f( 1., 1., 0. ); 
	OsuSphere( 0.15, 100, 100);   // 2: yellow moving spot light
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef( 0.4, 0., 15.*sin(Time*2*PI) );
	glTranslatef( 0., 0., -9.65 );
	glColor3f( 1., 1., 0. ); 
	OsuSphere( 0.15, 100, 100);   // 3: yellow moving spot light
	glPopMatrix();

	// set the lights

	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, MulArray3( .3f, White ) );
	glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
	glEnable( GL_LIGHTING );     // open the lighting

	// 0: white spot light at the position ( 0., 15., -7. ); spot direction ( 0., -1., 0. )

	SetSpotLight( GL_LIGHT0, 0., 15., -7., 0., -1., 0., 1., 1., 1. );

	// 1: purple point light at the position ( 0., 0., -28. );

	SetPointLight( GL_LIGHT1, 0., 0., -28., 0.5, 0., 1. );

	// 2 & 3: car yellow moving spot lights; spot direction ( 0., 0., -1. )

	SetSpotLight( GL_LIGHT2,-0.4, 0., 15.*sin(Time*2*PI) - 9.65, 0., 0., -1., 1., 1., 0. );
	SetSpotLight( GL_LIGHT3, 0.4, 0., 15.*sin(Time*2*PI) - 9.65, 0., 0., -1., 1., 1., 0. );

	// 4: sun's point light at the position ( 0., 0., 25. );
    
	SetPointLight( GL_LIGHT4, 0., 0., 25., 1., 0.3, 0.3 );

	// 5: green moving point light

	glPushMatrix( );
		SetPointLight( GL_LIGHT5,  (float)(RAD * sin(theta)), 0., (float)(RAD * cos(theta)) - 28., 0., 1., 0. );
		glDisable( GL_LIGHTING );
		glColor3f( 0., 1., 0. );
		glPushMatrix( );
			glTranslatef((float)(RAD * sin(theta)), 0., (float)(RAD * cos(theta)));
			// Center around a star at another galaxy
			glTranslatef( 0., 0., -28. );
			glutSolidSphere( 0.3, 100, 100 );
		glPopMatrix( );
		glEnable( GL_LIGHTING );
	glPopMatrix( );


	// Implementing the Keyboard Keys for lightings

	if( Light0On )
		glEnable( GL_LIGHT0 );
	else
		glDisable( GL_LIGHT0 );

	if( Light1On )
		glEnable( GL_LIGHT1 );
	else
		glDisable( GL_LIGHT1 );

	if( Light2On )
		glEnable( GL_LIGHT2 );
	else
		glDisable( GL_LIGHT2 );

	if( Light3On )
		glEnable( GL_LIGHT3 );
	else
		glDisable( GL_LIGHT3 );

	if( Light4On )
		glEnable( GL_LIGHT4 );
	else
		glDisable( GL_LIGHT4 );

	if( Light5On )
		glEnable( GL_LIGHT5 );
	else
		glDisable( GL_LIGHT5 );


	// draw the orbiting spaceship:
	glPushMatrix();
	glTranslatef((float)(15.5 * cos(theta)), 0., (float)(15.5 * sin(theta)));
	glRotatef( 90, 0., 0., -1. );
	glRotatef( 90, 0., -1., 0. );
	glScalef( 0.01, 0.01, 0.01 );
	SetMaterial( 0.3, 0.9, 1., 100. );  // Shiny
	glCallList( SHIP );
	glPopMatrix();


	// draw the textured torus out along -Z
	SetMaterial( 1., 1., 1., 100. );    // Shiny (limit: 128)
	// Flat
	glShadeModel( GL_FLAT );
	// Call the texture (Use the provided code in the handout)
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, Tex4 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 
    glCallList(OSUTorus);
    glDisable( GL_TEXTURE_2D );


	// draw the textured osuspheres 

	// earthtex
	glPushMatrix();
	// Surrounding the sun
	glTranslatef((float)(RADIUS * cos(theta)), 0., (float)(RADIUS * sin(theta)));
	glTranslatef( 0., 0., 25. );
	// Spinning (rotate) itself
	glRotatef( 360.*Time, 0., 1., 0. );
	SetMaterial( 1., 1., 1., 100. );    // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );
	// Call the texture (Use the provided code in the handout)
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, Tex0 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 
	OsuSphere( 4.0, 50, 50);            // Sphere Object
	glDisable( GL_TEXTURE_2D );
	glPopMatrix();

	// suntex
	glPushMatrix();
	glTranslatef( 0., 0., 25. );
	glRotatef( 360.*Time, 1., 0., 0. );
	SetMaterial( 1., 1., 1., 1. );      // Dull
	// Flat
	glShadeModel( GL_FLAT );
	// Call the texture (Use the provided code in the handout)
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, Tex1 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); 
	DistortedSphere( 8.0, 50, 50);     // Sphere Object
	glDisable( GL_TEXTURE_2D );
	glPopMatrix();

	// moontex
	glPushMatrix();
	// Surrounding the earth
	glTranslatef((float)(MOONRAD * sin(theta)), 0., (float)(MOONRAD * cos(theta)));
	glTranslatef((float)(RADIUS * cos(theta)), 0., (float)(RADIUS * sin(theta)));
	glTranslatef( 0., 0., 25. );
	// Spinning (rotate) itself
	glRotatef( 360.*Time, 0., 1., 0. );
	SetMaterial( 1., 1., 1., 100. );   // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );
	// Call the texture (Use the provided code in the handout)
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, Tex2 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); 
	OsuSphere( 2.0, 50, 50);           // Sphere Object
	glDisable( GL_TEXTURE_2D );
	glPopMatrix();

	// star1 in another galaxy, containing a purple light source
	glPushMatrix();
	glTranslatef( 0., 0., -28. );
	// Spinning itself
	glRotatef( 360.*Time*0.3, 0., 0., 1. );
	SetMaterial( 1., 1., 1., 1. );     // Dull
	// Flat
	glShadeModel( GL_FLAT );
	// Call the texture (Use the provided code in the handout)
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, Tex5 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); 
	OsuSphere( 6.0, 50, 50);           // Sphere Object
	glDisable( GL_TEXTURE_2D );
	glPopMatrix();

	// green textured moving star in another galaxy, containing a green light source
	glPushMatrix();
	glTranslatef((float)(RAD * sin(theta)), 0., (float)(RAD * cos(theta)));
	// Center around a star in another galaxy
	glTranslatef( 0., 0., -28. );
	// Spinning itself
	glRotatef( 360.*Time*0.3, 1., 0., 0. );
	SetMaterial( 1., 1., 1., 1. );      // Dull
	// Smooth
	glShadeModel( GL_SMOOTH );
	// Call the texture (Use the provided code in the handout)
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, Tex6 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); 
	OsuSphere( 3.0, 50, 50);            // Sphere Object
	glDisable( GL_TEXTURE_2D );
	glPopMatrix();


	// draw a textured car
	glPushMatrix();
	glTranslatef( 0., 0., 15.*sin(Time*2*PI) );
	glTranslatef( 0., 0., -8. );
	glRotatef( 90, 0., 1., 0. );
	glScalef( 0.2, 0.2, 0.2 );
	SetMaterial( 1., 1., 1., 30. );     // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );       
	// Call the texture (Use the provided code in the handout)
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, Tex3 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 
	glCallList( CAR );
	glDisable( GL_TEXTURE_2D );
	glPopMatrix();


	// draw cats living inside a star in another galaxy

	// draw a white cat 
	glPushMatrix();
	glTranslatef( 0., 0., -26. );
	glRotatef( 90, 0., 1., 0. );
	glScalef( 0.4, 0.4, 0.4 );
	SetMaterial( 1., 1., 1., 30. );     // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );       
	glCallList( CAT );
	glPopMatrix();

	// draw an orange cat 
	glPushMatrix();
	glTranslatef( 5., 0., -26. );
	glRotatef( 90, 0., 1., 0. );
	glScalef( 0.4, 0.4, 0.4 );
	SetMaterial( 0.9, 0.4, 0., 30. );   // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );       
	glCallList( CAT );
	glPopMatrix();

	// draw a cat 
	glPushMatrix();
	glTranslatef( -5., 0., -26. );
	glRotatef( 90, 0., 1., 0. );
	glScalef( 0.4, 0.4, 0.4 );
	SetMaterial( 0.5, 0.5, 0.7, 30. );  // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );       
	glCallList( CAT );
	glPopMatrix();


	// draw deers living inside a green star in another galaxy

	// draw a deer 
	glPushMatrix();
	glTranslatef((float)(RAD * sin(theta)), 0., (float)(RAD * cos(theta)));
	// Center around a star in another galaxy
	glTranslatef( 2., 0., -28. );
	glRotatef( 90, 0., 1., 0. );
	glScalef( 0.4, 0.4, 0.4 );
	SetMaterial( 0.5, 0.3, 0.3, 30. );   // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );       
	glCallList( DEER );
	glPopMatrix();

	// draw a deer 
	glPushMatrix();
	glTranslatef((float)(RAD * sin(theta)), 0., (float)(RAD * cos(theta)));
	// Center around a star in another galaxy
	glTranslatef( -2., 0., -28. );
	glRotatef( 90, 0., 1., 0. );
	glScalef( 0.4, 0.4, 0.4 );
	SetMaterial( 0.5, 0.5, 0.3, 30. );   // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );       
	glCallList( DEER );
	glPopMatrix();


	// draw a dino
	glPushMatrix();
	glTranslatef( 0., 0., 25. );
	glRotatef( 90, 0., 1., 0. );
	glScalef( 0.3, 0.3, 0.3 );
	SetMaterial( 0.6, 0.4, 0.2, 30. );   // Shiny
	// Smooth
	glShadeModel( GL_SMOOTH );       
	glCallList( DINO );
	glPopMatrix();

	// draw some gratuitous text that just rotates on top of the scene:

	// glDisable( GL_DEPTH_TEST );
	// glColor3f( 0., 1., 1. );
	// DoRasterString( 0., 1., 0., (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100., 0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	// turn off the lighting
	glDisable( GL_LIGHTING );
	glColor3f( 1., 1., 1. );
	DoRasterString( 5., 5., 0., (char *)"Final Project by Cheng Ying Wu" );


	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


// For control points & lines
void
DoPointMenu( int id )
{
	PointsOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoLineMenu( int id )
{
	LinesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

// Views
void
DoViewsMenu( int id )
{
	Views = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoShadowsMenu(int id)
{
	ShadowsOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int shadowsmenu = glutCreateMenu(DoShadowsMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	// For control points & lines
	int pointmenu = glutCreateMenu( DoPointMenu );
	glutAddMenuEntry( "No Points",  0 );
	glutAddMenuEntry( "Control Points",   1 );

	int linemenu = glutCreateMenu( DoLineMenu );
	glutAddMenuEntry( "No Lines",  0 );
	glutAddMenuEntry( "Control Lines",   1 );

	// Views Menu
    int viewsmenu = glutCreateMenu( DoViewsMenu );
	glutAddMenuEntry( "Outside View",             0 );
	glutAddMenuEntry( "View from Car",            1 );
	glutAddMenuEntry( "View from Spaceship",      2 );
	glutAddMenuEntry( "View from Earth",          3 );
	glutAddMenuEntry( "Cat Star's Inside View",   4 );
	glutAddMenuEntry( "View from Green Star ",    5 );
	glutAddMenuEntry( "Green Star's Inside View", 6 );
	glutAddMenuEntry( "View on Sun",              7 );
	glutAddMenuEntry( "Sun's Inside View",        8 );
	
	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "DifferentViews",viewsmenu);
	glutAddSubMenu(   "Points",        pointmenu);
	glutAddSubMenu(   "Lines",         linemenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( NULL );

	// Designate Animate( ) as the Idle Function in InitGraphics( )
	glutIdleFunc( Animate );


	// Read in the texture images and create some textured objects
	unsigned char *Texture0;
	unsigned char *Texture1;
	unsigned char *Texture2;
	unsigned char *Texture3;
	unsigned char *Texture4;
	unsigned char *Texture5;
	unsigned char *Texture6;

    int     width0, height0, width1, height1, width2, height2, width3, height3, width4, height4, width5, height5, width6, height6;

	Texture0 = BmpToTexture("worldtex.bmp", &width0, &height0);
	Texture1 = BmpToTexture("suntex.bmp",   &width1, &height1);
	Texture2 = BmpToTexture("moontex.bmp",  &width2, &height2);
	Texture3 = BmpToTexture("cartex.bmp",   &width3, &height3);
	Texture4 = BmpToTexture("ringtex.bmp",  &width4, &height4);
	Texture5 = BmpToTexture("star1tex.bmp", &width5, &height5);
	Texture6 = BmpToTexture("greentex.bmp", &width6, &height6);

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	glGenTextures( 1, &Tex0 );
	glGenTextures( 1, &Tex1 );
	glGenTextures( 1, &Tex2 );
	glGenTextures( 1, &Tex3 );
	glGenTextures( 1, &Tex4 );
	glGenTextures( 1, &Tex5 );
	glGenTextures( 1, &Tex6 );

	// Make the texture current and set its parameters
	// Texture0
	glBindTexture( GL_TEXTURE_2D, Tex0 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, level, ncomps, width0, height0, border, GL_RGB, GL_UNSIGNED_BYTE, Texture0 );

	// Texture1
	glBindTexture( GL_TEXTURE_2D, Tex1 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, level, ncomps, width1, height1, border, GL_RGB, GL_UNSIGNED_BYTE, Texture1 );

	// Texture2
	glBindTexture( GL_TEXTURE_2D, Tex2 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, level, ncomps, width2, height2, border, GL_RGB, GL_UNSIGNED_BYTE, Texture2 );

	// Texture3
	glBindTexture( GL_TEXTURE_2D, Tex3 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, level, ncomps, width3, height3, border, GL_RGB, GL_UNSIGNED_BYTE, Texture3 );

	// Texture4
	glBindTexture( GL_TEXTURE_2D, Tex4 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, level, ncomps, width4, height4, border, GL_RGB, GL_UNSIGNED_BYTE, Texture4 );

	// Texture5
	glBindTexture( GL_TEXTURE_2D, Tex5 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, level, ncomps, width5, height5, border, GL_RGB, GL_UNSIGNED_BYTE, Texture5 );

	// Texture6
	glBindTexture( GL_TEXTURE_2D, Tex6 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, level, ncomps, width6, height6, border, GL_RGB, GL_UNSIGNED_BYTE, Texture6 );


	// OBJ car
	CAR = glGenLists( 1 );
	glNewList( CAR, GL_COMPILE );
	LoadObjFile( "car.obj" );
	glEndList( );

	// OBJ CAT
	CAT = glGenLists( 1 );
	glNewList( CAT, GL_COMPILE );
	LoadObjFile( "cat.obj" );
	glEndList( );

	// OBJ Spaceship
	SHIP = glGenLists( 1 );
	glNewList( SHIP, GL_COMPILE );
	LoadObjFile( "spaceship.obj" );
	glEndList( );

	// OBJ Deer
	DEER = glGenLists( 1 );
	glNewList( DEER, GL_COMPILE );
	LoadObjFile( "deer.obj" );
	glEndList( );

	// OBJ Dino
	DINO = glGenLists( 1 );
	glNewList( DINO, GL_COMPILE );
	LoadObjFile( "dino.obj" );
	glEndList( );


	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );

	// create the object:
	// Red Box with flat shading

	BoxList = glGenLists( 1 );
	glNewList( BoxList, GL_COMPILE );

		
		glPushMatrix( );

		// Flat
		glShadeModel( GL_FLAT );

		// Unitizing
		glEnable( GL_NORMALIZE );

		glColor3f( 1., 1., 1. );
		glBegin( GL_QUADS );

				// +Z 
				glNormal3f( 0.f, 0.f, 1.f );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f( -dx,  dy,  dz );

				// -Z
				glNormal3f( 0.f, 0.f, -1.f);
				glVertex3f( -dx, -dy, -dz );
				glVertex3f( -dx,  dy, -dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f(  dx, -dy, -dz );

				// +X
				glNormal3f( 1.f, 0.f, 0.f );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f(  dx,  dy,  dz );

				// -X
				glNormal3f(-1.f, 0.f, 0.f );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f( -dx,  dy, -dz );
				glVertex3f( -dx, -dy, -dz );

				// +Y
				glNormal3f( 0.f, 1.f, 0.f );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f( -dx,  dy, -dz );

				// -Y
				glNormal3f( 0.f, -1.f, 0.f);
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx, -dy, -dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx, -dy,  dz );

		glEnd( );
		glPopMatrix( );

	glEndList( );


	// create the torus

	OSUTorus = glGenLists( 1 );
	glNewList( OSUTorus, GL_COMPILE );

	    GLfloat r = 3.0;
        GLfloat R = 9.0;
        GLint numSides = 50;
        GLint numRings = 100;

		// Torus 
	    GLfloat ringDelta = 2.0 * M_PI / (float)numRings;
		GLfloat sideDelta = 2.0 * M_PI / (float)numSides;

		GLfloat theta    = 0.0;
		GLfloat cosTheta = 1.0;
		GLfloat sinTheta = 0.0;

		glPushMatrix( );
		glTranslatef( 0., 0., -7. );

		// Smooth
		glShadeModel( GL_SMOOTH );

		// Unitizing
		glEnable( GL_NORMALIZE );

		glColor3f( 0., 1.0, 0. );
		for( int i = 0; i < numRings; i++ )
		{
			GLfloat theta1 = theta + ringDelta;
			GLfloat cosTheta1 = cos(theta1);
			GLfloat sinTheta1 = sin(theta1);

			GLfloat phi = 0.0;
			float t  = (float)i     / (float)numRings;
			float t1 = (float)(i+1) / (float)numRings;

			glBegin( GL_QUAD_STRIP );

			for( int j = 0; j <= numSides; j++ )
			{
				phi += sideDelta;
				GLfloat cosPhi = cos(phi);
				GLfloat sinPhi = sin(phi);
				GLfloat dist = R + r * cosPhi;

				float s = (float)j / (float)numSides;
				glTexCoord2f( s, t );
				glNormal3f( cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi );
				glVertex3f( cosTheta1 * dist,   -sinTheta1 * dist,   r * sinPhi );

				glTexCoord2f( s, t1 );
				glNormal3f( cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi );
				glVertex3f( cosTheta * dist,   -sinTheta * dist,   r * sinPhi );
			}

			glEnd( );

			theta = theta1;
			cosTheta = cosTheta1;
			sinTheta = sinTheta1;
		}
		glPopMatrix( );	

	glEndList( );


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 2.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		// new added
		case 'f':
		case 'F':
			Frozen = ! Frozen;
			if( Frozen )
				glutIdleFunc( NULL );
			else
				glutIdleFunc( Animate );
			break;

		case '0':
			Light0On = ! Light0On;
			break;

		case '1':
			Light1On = ! Light1On;
			break;

		case '2':
			Light2On = ! Light2On;
			break;

		case '3':
			Light3On = ! Light3On;
			break;

		case '4':
			Light4On = ! Light4On;
			break;

		case '5':
			Light5On = ! Light5On;
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	// if( true )
	// 	fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 0;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;

	// For Bézier curves
	PointsOn = 0;
	LinesOn = 0;

	// For Views
	Views = 0;

	// For lights
	Light0On = Light1On = Light2On = Light3On = Light4On = Light5On = true;
	Frozen = false;

}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}

// read a BMP file into a Texture:

#define VERBOSE		false
#define BMP_MAGIC_NUMBER	0x4d42
#ifndef BI_RGB
#define BI_RGB			0
#define BI_RLE8			1
#define BI_RLE4			2
#endif


// bmp file header:
struct bmfh
{
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;



// read a BMP file into a Texture:

unsigned char *
BmpToTexture( char *filename, int *width, int *height )
{
	FILE *fp;
#ifdef _WIN32
        errno_t err = fopen_s( &fp, filename, "rb" );
        if( err != 0 )
        {
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
        }
#else
	fp = fopen( filename, "rb" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
	}
#endif

	FileHeader.bfType = ReadShort( fp );


	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if( VERBOSE ) fprintf( stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
			FileHeader.bfType, FileHeader.bfType&0xff, (FileHeader.bfType>>8)&0xff );
	if( FileHeader.bfType != BMP_MAGIC_NUMBER )
	{
		fprintf( stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType );
		fclose( fp );
		return NULL;
	}


	FileHeader.bfSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize );

	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );

	FileHeader.bfOffBytes = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfOffBytes = %d\n", FileHeader.bfOffBytes );


	InfoHeader.biSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSize = %d\n", InfoHeader.biSize );
	InfoHeader.biWidth = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biWidth = %d\n", InfoHeader.biWidth );
	InfoHeader.biHeight = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biHeight = %d\n", InfoHeader.biHeight );

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biPlanes = %d\n", InfoHeader.biPlanes );

	InfoHeader.biBitCount = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount );

	InfoHeader.biCompression = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression );

	InfoHeader.biSizeImage = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage );

	InfoHeader.biXPixelsPerMeter = ReadInt( fp );
	InfoHeader.biYPixelsPerMeter = ReadInt( fp );

	InfoHeader.biClrUsed = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed );

	InfoHeader.biClrImportant = ReadInt( fp );


	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );


	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char *texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\n" );
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ( ( InfoHeader.biBitCount*InfoHeader.biWidth + 31 ) / 32 );
	if( VERBOSE )	fprintf( stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes );

	int myRowSizeInBytes = ( InfoHeader.biBitCount*InfoHeader.biWidth + 7 ) / 8;
	if( VERBOSE )	fprintf( stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes );

	int oldNumExtra =  4*(( (3*InfoHeader.biWidth)+3)/4) - 3*InfoHeader.biWidth;
	if( VERBOSE )	fprintf( stderr, "Old NumExtra padding = %d\n", oldNumExtra );

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if( VERBOSE )	fprintf( stderr, "New NumExtra padding = %d\n", numExtra );


	// this function does not support compression:

	if( InfoHeader.biCompression != 0 )
	{
		fprintf( stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}
	

	// we can handle 24 bits of direct color:
	if( InfoHeader.biBitCount == 24 )
	{
		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if( InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256 )
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32 *colorTable = new struct rgba32[ InfoHeader.biClrUsed ];

		rewind( fp );
		fseek( fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET );
		for( int c = 0; c < InfoHeader.biClrUsed; c++ )
		{
			colorTable[c].r = fgetc( fp );
			colorTable[c].g = fgetc( fp );
			colorTable[c].b = fgetc( fp );
			colorTable[c].a = fgetc( fp );
			if( VERBOSE )	fprintf( stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a );
		}

		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				int index = fgetc( fp );
				*(tp+0) = colorTable[index].r;	// r
				*(tp+1) = colorTable[index].g;	// g
				*(tp+2) = colorTable[index].b;	// b
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}

		delete[ ] colorTable;
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	const unsigned char b2 = fgetc( fp );
	const unsigned char b3 = fgetc( fp );
	return ( b3 << 24 )  |  ( b2 << 16 )  |  ( b1 << 8 )  |  b0;
}

short
ReadShort( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	return ( b1 << 8 )  |  b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

// void
// Cross(float v1[3], float v2[3], float vout[3])
// {
// 	float tmp[3];
// 	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
// 	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
// 	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
// 	vout[0] = tmp[0];
// 	vout[1] = tmp[1];
// 	vout[2] = tmp[2];
// }

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

// float
// Unit(float vin[3], float vout[3])
// {
// 	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
// 	if (dist > 0.0)
// 	{
// 		dist = sqrtf(dist);
// 		vout[0] = vin[0] / dist;
// 		vout[1] = vin[1] / dist;
// 		vout[2] = vin[2] / dist;
// 	}
// 	else
// 	{
// 		vout[0] = vin[0];
// 		vout[1] = vin[1];
// 		vout[2] = vin[2];
// 	}
// 	return dist;
// }
