#include <stdio.h>
#include <math.h>

// #include <GL/gl.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif


int		NLngs, NLats;
struct pt *	Points;
 

// For animation
float   Time;
#define MS_PER_CYCLE	4500


struct pt
{
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

inline
struct pt *
PPointer( int lat, int lng )
{
	if( lat < 0 )	lat += (NLats-1);
	if( lng < 0 )	lng += (NLngs-0);
	if( lat > NLats-1 )	lat -= (NLats-1);
	if( lng > NLngs-1 )	lng -= (NLngs-0);
	return &Points[ NLngs*lat + lng ];
}

inline
void
DrawPt( struct pt *p )
{
	glNormal3fv( &p->nx );
	glTexCoord2fv( &p->s );
	glVertex3fv( &p->x );
}

void
DistortedSphere( float radius, int slices, int stacks )
{
	// set the globals:

	NLngs = slices;
	NLats = stacks;
	if( NLngs < 3 )
		NLngs = 3;
	if( NLats < 3 )
		NLats = 3;

	// allocate the point data structure:

	Points = new struct pt[ NLngs * NLats ];

	// fill the Pts structure:

	for( int ilat = 0; ilat < NLats; ilat++ )
	{
		float lat = -M_PI/2.  +  M_PI * (float)ilat / (float)(NLats-1);	// ilat=0/lat=0. is the south pole
											// ilat=NumLats-1, lat=+M_PI/2. is the north pole
		float xz = cosf( lat );
		float  y = sinf( lat );
		for( int ilng = 0; ilng < NLngs; ilng++ )				// ilng=0, lng=-M_PI and
											// ilng=NumLngs-1, lng=+M_PI are the same meridian
		{
			float lng = -M_PI  +  2. * M_PI * (float)ilng / (float)(NLngs-1);
			float x =  xz * cosf( lng );
			float z = -xz * sinf( lng );
			struct pt* p = PPointer( ilat, ilng );
			p->x  = radius * x;
			p->y  = radius * y;
			p->z  = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			
			// With some distortion of the texture-image GL_REPLACE'ed on it
			p->s = (( lng + M_PI    ) / ( 2.*M_PI ) + Time)/2;
			p->t = (( lat + M_PI/2. ) / M_PI + Time)/2;
					
		}
	}

	struct pt top, bot;		// top, bottom points

	top.x =  0.;		top.y  = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s  = 0.;		top.t  = 1.;

	bot.x =  0.;		bot.y  = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s  = 0.;		bot.t  =  0.;

	// connect the north pole to the latitude NumLats-2:

	glBegin(GL_TRIANGLE_STRIP);
	for (int ilng = 0; ilng < NLngs; ilng++)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NLngs - 1);
		top.s = (lng + M_PI) / (2. * M_PI);
		DrawPt(&top);
		struct pt* p = PPointer(NLats - 2, ilng);	// ilat=NumLats-1 is the north pole
		DrawPt(p);
	}
	glEnd();

	// connect the south pole to the latitude 1:

	glBegin( GL_TRIANGLE_STRIP );
	for (int ilng = NLngs - 1; ilng >= 0; ilng--)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NLngs - 1);
		bot.s = (lng + M_PI) / (2. * M_PI);
		DrawPt(&bot);
		struct pt* p = PPointer(1, ilng);					// ilat=0 is the south pole
		DrawPt(p);
	}
	glEnd();

	// connect the horizontal strips:

	for( int ilat = 2; ilat < NLats-1; ilat++ )
	{
		struct pt* p;
		glBegin(GL_TRIANGLE_STRIP);
		for( int ilng = 0; ilng < NLngs; ilng++ )
		{
			p = PPointer( ilat, ilng );
			DrawPt( p );
			p = PPointer( ilat-1, ilng );
			DrawPt( p );
		}
		glEnd();
	}

	// clean-up:

	delete [ ] Points;
	Points = NULL;
}
