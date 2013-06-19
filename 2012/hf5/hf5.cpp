//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2012-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Prontvai Zsolt
// Neptun : QUKVX3
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// MsWindows-on ez is kell
#include <windows.h>     
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>     


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...
//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
   float x, y, z;

   Vector( ) { 
	x = y = z = 0;
   }
   Vector(float x0, float y0, float z0 = 0) { 
	x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) { 
	return Vector(x * a, y * a, z * a); 
   }
	Vector operator/(float a) { 
	return Vector(x / a, y / a, z / a); 
   }
   Vector operator+(const Vector& v) {
 	return Vector(x + v.x, y + v.y, z + v.z); 
   }
   Vector operator-(const Vector& v) {
 	return Vector(x - v.x, y - v.y, z - v.z); 
   }
   float operator*(const Vector& v) { 	// dot product
	return (x * v.x + y * v.y + z * v.z); 
   }
   Vector operator%(const Vector& v) { 	// cross product
	return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   float Length() { return sqrt(x * x + y * y + z * z); }
};

Vector Transform(Vector w);
void drawSphere(float r, int k, int h);

Vector heliPos=Vector(0,0,-15);
float quaternionS=1.0;
Vector quaternionW;
float rotorAngle=0;

Vector camPos=Vector(0,8,0);
Vector camV=Vector(0,0,0);

long lasttime;

struct Ball{
	Vector pos;
	Vector v;
	float r;
	
	Ball(){
		pos=heliPos;
		v=Transform(Vector(0,0,-10));
		r=0.2;
	}
	void draw(){
		float colorD[]={1,1,1,1};
		float colorS[]={1,1,1,1};
		glMaterialfv(GL_FRONT,GL_AMBIENT,colorD);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorS);
		glMaterialf(GL_FRONT,GL_SHININESS,50);
		glPushMatrix();
		glTranslatef(pos.x,pos.y,pos.z);
		drawSphere(r,20,20);
		glPopMatrix();
	}
	void move(int t){
		v=v+Vector(0,-0.005,0);
		pos=pos+v*t/1000;
		if(pos.y<-10){
			pos.y=-10;
			v.y=-0.9*v.y;
			
		}
	}
};

const int screenWidth = 600;
const int screenHeight = 600;

#define MAX_BALL 100
Ball BFG[MAX_BALL];
int db=0;


Vector Transform(Vector w){
	float s=0;
	float length2=quaternionS*quaternionS+quaternionW.x*quaternionW.x+quaternionW.y*quaternionW.y+quaternionW.z*quaternionW.z;
	float tempS=s*quaternionS-w*quaternionW;
	Vector tempW=w*quaternionS+quaternionW*s+quaternionW%w;
	float iS=quaternionS/length2;
	Vector iW=Vector((-quaternionW.x)/length2,(-quaternionW.y)/length2,(-quaternionW.z)/length2);
	return tempW*iS+iW*tempS+tempW%iW;	
}


void simulate(){
	long time = glutGet(GLUT_ELAPSED_TIME);
	long elapsedtime=time-lasttime;
	lasttime=time;
	while(elapsedtime>0){
		rotorAngle+=360.0/100.0;
		heliPos=heliPos+Transform(Vector(0,0,-1))/500.0;
		
		for(int i=0;i<db && i<MAX_BALL;i++) BFG[i].move(1);
		
		float felhajto = 0.001;
		float surlodasi = 0.9999;
		float kotel = 0.005*(((heliPos-camPos).Length()>15)?((heliPos-camPos).Length()-15):0);
		camV = camV*surlodasi+((heliPos-camPos)/(heliPos-camPos).Length())*kotel+Vector(0,1,0)*felhajto;
		camPos = camPos+camV/10000.0;
		elapsedtime--;
	}
}

void forgatas(float a, Vector w){
	float rad=a*M_PI/180.0f;
	float s=cos(rad/2.0);
	w=w*sin(rad/2.0);
	float tempS=s*quaternionS-w*quaternionW;
	Vector tempW=w*quaternionS+quaternionW*s+quaternionW%w;
	quaternionS=tempS;
	quaternionW=tempW;
	float length=sqrt(quaternionS*quaternionS+quaternionW.x*quaternionW.x+quaternionW.y*quaternionW.y+quaternionW.z*quaternionW.z);
	if (length<0.999999){
		quaternionS/=length;
		quaternionW=quaternionW/length;
	}
}

void drawCylinder(float r, float l, int k, int h){
	glBegin(GL_POLYGON);
	for(int i=0;i<=k;i++){
		float t=2*M_PI/(float)k*(double)i;
		float x=cos(t);
		float y=sin(t);
		glNormal3f(0.0f,0.0f,-1.0f);
       	glVertex3f(x*r,y*r,0);
	}
	glEnd();
	
	glBegin(GL_POLYGON);
	for(int i=0;i<=k;i++){
		float t=2*M_PI/(float)k*(double)i;
		float x=cos(t);
		float y=sin(t);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(x*r,y*r,l);
	}
	glEnd();
		
	for(int i=0;i<h;i++){
		float z0=(l/(float)h)*(float)i;
		float z1=(l/(float)h)*((float)i+1.0f);
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0;j<=k;j++){
			float t=2*M_PI/(float)k*(double)j;
			float x=cos(t);
			float y=sin(t);
			glNormal3f(x,y,0);
           	glVertex3f(x*r,y*r,z0);
            glNormal3f(x,y,0);
            glVertex3f(x*r,y*r,z1);
		}
		glEnd();
	}
}


void drawCone(float r, float l, int k, int h){
	glBegin(GL_POLYGON);
	for(int i=0;i<=k;i++){
		float t=2*M_PI/(float)k*(double)i;
		float x=cos(t);
		float y=sin(t);
		glNormal3f(0.0f,0.0f,-1.0f);
       	glVertex3f(x*r,y*r,0);
	}
	glEnd();
	
	for(int i=0;i<h;i++){
		float z0=(l/(float)h)*(float)i;
		float zr0=(float)(h-i)/(float)h*r;
		float z1=(l/(float)h)*((float)i+1.0f);
		float zr1=(float)(h-i-1)/(float)h*r;
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0;j<=k;j++){
			float t=2*M_PI/(float)k*(double)j;
			float x=cos(t);
			float y=sin(t);
			glNormal3f(x*r,y*r,r*r/l);
           	glVertex3f(x*zr0,y*zr0,z0);
            glVertex3f(x*zr1,y*zr1,z1);
		}
		glEnd();
	}
}
void drawSphere(float r, int k, int h) {
	for(int i=0;i<h;i++){
		float fi0=M_PI*(-0.5f+(float)i/(float)h);
		float z0=sin(fi0)*r;
		float zr0=cos(fi0)*r;
		float fi1=M_PI*(-0.5f+(float)(i+1)/(float)h);
		float z1=sin(fi1)*r;;
		float zr1=cos(fi1)*r;
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0;j<=k;j++){
			float t=2*M_PI/(float)k*(double)j;
			float x=cos(t);
			float y=sin(t);
			glNormal3f(x*zr0,y*zr0,z0);
           	glVertex3f(x*zr0,y*zr0,z0);
			glNormal3f(x*zr1,y*zr1,z1);
            glVertex3f(x*zr1,y*zr1,z1);
		}
		glEnd();
	}
}

void drawHalfSphere(float r, int k, int h) {
	for(int i=0;i<h;i++){
		float fi0=M_PI*(-0.5f+(float)i/(float)h);
		float z0=sin(fi0)*r;
		float zr0=cos(fi0)*r;
		float fi1=M_PI*(-0.5f+(float)(i+1)/(float)h);
		float z1=sin(fi1)*r;;
		float zr1=cos(fi1)*r;
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0;j<=k;j++){
			float t=M_PI/(float)k*(double)j;
			float x=cos(t);
			float y=sin(t);
			glNormal3f(x*zr0,y*zr0,z0);
           	glVertex3f(x*zr0,y*zr0,z0);
			glNormal3f(x*zr1,y*zr1,z1);
            glVertex3f(x*zr1,y*zr1,z1);
		}
		glEnd();
	}
}

void drawQuarterSphere(float r, int k, int h) {
	for(int i=0;i<h;i++){
		float fi0=M_PI/2*((float)i/(float)h);
		float z0=sin(fi0)*r;
		float zr0=cos(fi0)*r;
		float fi1=M_PI/2*((float)(i+1)/(float)h);
		float z1=sin(fi1)*r;;
		float zr1=cos(fi1)*r;
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0;j<=k;j++){
			float t=M_PI/(float)k*(double)j;
			float x=cos(t);
			float y=sin(t);
			glNormal3f(x*zr0,y*zr0,z0);
           	glVertex3f(x*zr0,y*zr0,z0);
			glNormal3f(x*zr1,y*zr1,z1);
            glVertex3f(x*zr1,y*zr1,z1);
		}
		glEnd();
	}
}

void drawRectangle(float h,float sz,float m){
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,1.0f);
   	glVertex3f(-0.5*sz,0.0f,0.0f);
	glVertex3f(-0.5*sz,m,0.0f);
	glVertex3f(0.5*sz,m,0.0f);
	glVertex3f(0.5*sz,0.0f,0.0f);
	
	glNormal3f(0.0f,0.0f,-1.0f);
   	glVertex3f(-0.5*sz,0.0f,h);
	glVertex3f(-0.5*sz,m,h);
	glVertex3f(0.5*sz,m,h);
	glVertex3f(0.5*sz,0.0f,h);
	
	glNormal3f(0.0f,1.0f,0.0f);
   	glVertex3f(-0.5*sz,m,h);
	glVertex3f(-0.5*sz,m,0.0f);
	glVertex3f(0.5*sz,m,0.0f);
	glVertex3f(0.5*sz,m,h);
	
	glNormal3f(0.0f,-1.0f,0.0f);
   	glVertex3f(-0.5*sz,0.0f,h);
	glVertex3f(-0.5*sz,0.0f,0.0f);
	glVertex3f(0.5*sz,0.0f,0.0f);
	glVertex3f(0.5*sz,0.0f,h);
	
	glNormal3f(1.0f,0.0f,0.0f);
   	glVertex3f(0.5*sz,0.0f,h);
	glVertex3f(0.5*sz,0.0f,0.0f);
	glVertex3f(0.5*sz,m,0.0f);
	glVertex3f(0.5*sz,m,h);
	
	glNormal3f(-1.0f,0.0f,0.0f);
   	glVertex3f(-0.5*sz,0.0f,h);
	glVertex3f(-0.5*sz,0.0f,0.0f);
	glVertex3f(-0.5*sz,m,0.0f);
	glVertex3f(-0.5*sz,m,h);
	glEnd();
}

void drawTopRotor(float r, float angle){
	glPushMatrix();
	glRotatef(angle,0.0f,1.0f,0.0f);
	drawRectangle(r,r/10.0f,r/20.0f);
	glRotatef(180.0f,0.0f,1.0f,0.0f);
	drawRectangle(r,r/10.0f,r/20.0f);
	glPopMatrix();
}

void drawBackRotor(float r, float angle){
	glPushMatrix();
	glRotatef(angle,0.0f,1.0f,0.0f);
	drawRectangle(r,r/10.0f,r/20.0f);
	glRotatef(120.0f,0.0f,1.0f,0.0f);
	drawRectangle(r,r/10.0f,r/20.0f);
	glRotatef(120.0f,0.0f,1.0f,0.0f);
	drawRectangle(r,r/10.0f,r/20.0f);
	glPopMatrix();
}

void drawGatling(float r, float l,float angle){
	glPushMatrix();
	glPushMatrix();
	glTranslatef(0,-r,0);
	drawRectangle(2*r,2*r,2*r);
	glPopMatrix();
	glTranslatef(0,0,2*r);
	glRotatef(angle,0.0f,0.0f,1.0f);
	glPushMatrix();
	glTranslatef(0,5.0/6.0*r,0);
	drawCylinder(r/6.0,l,20,20);
	glPopMatrix();
	glPushMatrix();
	glRotatef(60,0.0f,0.0f,1.0f);
	glTranslatef(0,5.0/6.0*r,0);
	drawCylinder(r/6.0,l,20,20);
	glPopMatrix();
	glPushMatrix();
	glRotatef(120,0.0f,0.0f,1.0f);
	glTranslatef(0,5.0/6.0*r,0);
	drawCylinder(r/6.0,l,20,20);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180,0.0f,0.0f,1.0f);
	glTranslatef(0,5.0/6.0*r,0);
	drawCylinder(r/6.0,l,20,20);
	glPopMatrix();
	glPushMatrix();
	glRotatef(240,0.0f,0.0f,1.0f);
	glTranslatef(0,5.0/6.0*r,0);
	drawCylinder(r/6.0,l,20,20);
	glPopMatrix();
	glPushMatrix();
	glRotatef(300,0.0f,0.0f,1.0f);
	glTranslatef(0,5.0/6.0*r,0);
	drawCylinder(r/6.0,l,20,20);
	glPopMatrix();
	glTranslatef(0,0,0.6*l);
	drawCylinder(r,0.2*l,20,20);
	glPopMatrix();
}

void drawBuilding(float a){
	float colorD0[]={1,1,1,1};
	float colorD1[]={1,0,0,1};
	float colorS0[]={0,0,0,1};
	float colorS1[]={0,0,0,1};
	glMaterialfv(GL_FRONT,GL_AMBIENT,colorD0);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorS0);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3f(-a,0.0f,a);
	glVertex3f(-a,0.0f,-a);
	glVertex3f(-a,2*a,-a);
	glVertex3f(-a,2*a,a);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(a,0.0f,a);
	glVertex3f(a,0.0f,-a);
	glVertex3f(a,2*a,-a);
	glVertex3f(a,2*a,a);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(-a,2*a,a);
	glVertex3f(-a,0.0f,a);
	glVertex3f(a,0.0f,a);
	glVertex3f(a,2*a,a);
	glNormal3f(0.0f,0.0f,-1.0f);
	glVertex3f(-a,2*a,-a);
	glVertex3f(-a,0.0f,-a);
	glVertex3f(a,0.0f,-a);
	glVertex3f(a,2*a,-a);
	glEnd();
	glMaterialfv(GL_FRONT,GL_AMBIENT,colorD1);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorS1);
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f,1.0f,1.0f);
	glVertex3f(-a,2*a,a);
	glVertex3f(a,2*a,a);
	glVertex3f(0,3*a,0);
	glNormal3f(0.0f,1.0f,-1.0f);
	glVertex3f(-a,2*a,-a);
	glVertex3f(a,2*a,-a);
	glVertex3f(0,3*a,0);
	glNormal3f(1.0f,1.0f,0.0f);
	glVertex3f(a,2*a,-a);
	glVertex3f(a,2*a,a);
	glVertex3f(0,3*a,0);
	glNormal3f(-1.0f,1.0f,0.0f);
	glVertex3f(-a,2*a,-a);
	glVertex3f(-a,2*a,a);
	glVertex3f(0,3*a,0);
	glEnd();
}

void drawLand(){
	float colorD0[]={0.5,1.0,0.5,1};
	float colorS0[]={0,0,0,0};
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glMaterialfv(GL_FRONT,GL_AMBIENT,colorD0);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorS0);
	glBegin(GL_QUADS);
		for(int i=-200;i<200;i++){
			for(int j=-200;j<200;j++){
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f,0.0f);
				glVertex3f(i, -10.0f, j);
				glTexCoord2f(0.0f,1.0f);
				glVertex3f(i, -10.0f, j+1);
				glTexCoord2f(1.0f,1.0f);
				glVertex3f(i+1,  -10.0f, j+1);
				glTexCoord2f(1.0f,0.0f);
				glVertex3f(i+1,  -10.0f, j);
			}
		}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawHelicopter(){
	float colorA0[]={0,0,0,0};
	float colorA1[]={0,0,0,0};
	float colorA2[]={0,0,0,0};
	float colorD0[]={0.7529,0.6902,0.5686,1};
	float colorD1[]={0,0,0,1};
	float colorD2[]={0,0,0,0.2};
	float colorS0[]={0.7529,0.6902,0.5686,1};
	float colorS1[]={0.5,0.5,0.5,1};
	float colorS2[]={0,0,1,0.2};
	glPushMatrix();
	glTranslatef(heliPos.x,heliPos.y,heliPos.z);
	glRotatef(atan2(quaternionW.Length(),quaternionS)*180.0f/M_PI*2.0f,quaternionW.x,quaternionW.y,quaternionW.z);
	glMaterialfv(GL_FRONT,GL_AMBIENT,colorA0);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorS0);
	glMaterialf(GL_FRONT,GL_SHININESS,50);
	glPushMatrix();
	glTranslatef(0.0f,0.8f,0.0f);
	glRotatef(-90.0f,1.0f,0.0f,0.0f);
	drawCylinder(0.2,0.4,20,20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0f,0.0f,1.4f);
	drawCylinder(0.2,2.0,20,20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1.0f,-1.0f,-2.5f);
	drawCylinder(0.1,4.0,20,20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(1.0f,-1.0f,-2.5f);
	drawCylinder(0.1,4.0,20,20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0f,0.0f,0.5f);
	glRotatef(90.0f,1.0f,-1.0f,0.0f);
	glTranslatef(0.0f,0.0f,0.4f);
	drawCylinder(0.1,1.0,20,20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0f,0.0f,-1.5f);
	glRotatef(90.0f,1.0f,-1.0f,0.0f);
	glTranslatef(0.0f,0.0f,0.4f);
	drawCylinder(0.1,1.0,20,20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0f,0.0f,0.5f);
	glRotatef(-90.0f,-1.0f,-1.0f,0.0f);
	glTranslatef(0.0f,0.0f,0.4f);
	drawCylinder(0.1,1.0,20,20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0f,0.0f,-1.5f);
	glRotatef(-90.0f,-1.0f,-1.0f,0.0f);
	glTranslatef(0.0f,0.0f,0.4f);
	drawCylinder(0.1,1.0,20,20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f,1.2f,0.0f);
	glMaterialfv(GL_FRONT,GL_AMBIENT,colorA1);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorS1);
	glMaterialf(GL_FRONT,GL_SHININESS,1);
	drawTopRotor(3.0f,rotorAngle);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.2f,0.0f,3.3f);
	glRotatef(-90.0f,0.0f,0.0f,1.0f);
	drawBackRotor(1.0f,rotorAngle);
	glPopMatrix();
	
	glPushMatrix();
	glRotatef(180.0f,0.0f,1.0f,0.0f);
	glTranslatef(0.0f,0.0f,2.5f);
	drawGatling(0.25,1.5,rotorAngle);
	glPopMatrix();
	
	glMaterialfv(GL_FRONT,GL_AMBIENT,colorA0);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorS0);
	glMaterialf(GL_FRONT,GL_SHININESS,50);
	glPushMatrix();
	glTranslatef(0.0f,0.0f,-0.5f);
	glScalef(1.0f,1.0f,2.0f);
	drawQuarterSphere(1.0, 20, 20);
	glPushMatrix();
	glRotatef(180.0f,0.0f,0.0f,1.0f);
	drawHalfSphere(1.0, 20, 20);
	glPopMatrix();
	glRotatef(180.0f,0.0f,1.0f,0.0f);
	glMaterialfv(GL_FRONT,GL_AMBIENT,colorA2);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorS2);
	glMaterialf(GL_FRONT,GL_SHININESS,50);
	drawQuarterSphere(1.0, 20, 20);
	glPopMatrix();
	
	glPopMatrix();
}

void drawQuaternion(){
	float colorA0[]={0,0,0,0};
	float colorD0[]={1,0,0,1};
	float colorS0[]={1,0,0,1};
	Vector normW=quaternionW*(1/quaternionW.Length());
	Vector v=Vector(0,0,1);
	Vector normal=normW%v;
	float cosin=normW*v;
	float fi=acos(cosin)*180/M_PI;
	float length=fabs(quaternionS)*4.0/cos(M_PI/4);
	glPushMatrix();
	glMaterialfv(GL_FRONT,GL_AMBIENT,colorA0);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,colorD0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorS0);
	glMaterialf(GL_FRONT,GL_SHININESS,50);
	glTranslatef(heliPos.x,heliPos.y,heliPos.z);
	glRotatef(-fi,normal.x,normal.y,normal.z);
	drawCylinder(0.1f,length*5.0f/6.0f,20,20);
	glTranslatef(0.0f,0.0f,length*5.0f/6.0f);
	drawCone(0.2f,length/6.0f,20,20);
	glPopMatrix();
}

void onInitialization( ) { 
	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	unsigned int texture;
	unsigned char bitmap[1024][3];
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	for(int i=0;i<1024;i++){
		bitmap[i][0]=rand()%100;
		bitmap[i][1]=rand()%200;
		bitmap[i][2]=rand()%100;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void onDisplay( ) {
    glClearColor(0.6f, 0.6f, 1.0f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
	
	simulate();
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(60.0, (float)screenWidth / (float)screenHeight, 1.0, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camPos.x,camPos.y,camPos.z, heliPos.x,heliPos.y,heliPos.z, 0,1,0);

	
	float ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
	float lightColor0[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float lightPos0[] = {0.0f, 20.0f, 20.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	drawLand();
	
	for(int i=0;i<db && i<MAX_BALL;i++) BFG[i].draw();
	
	glPushMatrix();
	glTranslatef(0.0f,-10.0f,-40.0f);
	drawBuilding(2.0f);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(10.0f,-10.0f,-40.0f);
	drawBuilding(2.0f);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-5.0f,-10.0f,-60.0f);
	drawBuilding(2.0f);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(10.0f,-10.0f,-60.0f);
	drawBuilding(2.0f);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(10.0f,-10.0f,-25.0f);
	drawBuilding(2.0f);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-10.0f,-10.0f,-25.0f);
	drawBuilding(2.0f);
	glPopMatrix();
	
	
	drawQuaternion();
	drawHelicopter();


    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}


void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'R') {
		
		forgatas(20.0f,Vector(0.0f,0.0f,1.0f));
		glutPostRedisplay( );
	}
	if (key == 'E') {
		forgatas(-20.0f,Vector(0.0f,0.0f,1.0f));
		glutPostRedisplay( );
	}
	if (key == 'P') {
		forgatas(20.0f,Vector(1.0f,0.0f,0.0f));
		glutPostRedisplay( );
	}
	if (key == 'O') {
		forgatas(-20.0f,Vector(1.0f,0.0f,0.0f));
		glutPostRedisplay( );
	}
	if (key == 'Y') {
		forgatas(20.0f,Vector(0.0f,1.0f,0.0f));
		glutPostRedisplay( );
	}
	if (key == 'X') {
		forgatas(-20.0f,Vector(0.0f,1.0f,0.0f));
		glutPostRedisplay( );
	}
	if (key == ' ') {
		
		BFG[db%100]=Ball();
		db++;
		glutPostRedisplay( );
	}
}

void onMouse(int button, int state, int x, int y) {
}

void onIdle( ) {
	glutPostRedisplay( );
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse); 
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}

