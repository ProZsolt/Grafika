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

struct Pont {
	Vector v;
	float suly;
	
	void rajzol(){
		glColor3f(1, 0, 0.5);
		glBegin(GL_POLYGON);
			for(int j=0;j<360;j++){
				float t=j*3.1415/180;
				//forrás: http://mathworld.wolfram.com/HeartCurve.html
				float x=0.2*(16*sin(t)*sin(t)*sin(t));
				float y=0.2*(13*cos(t)-5*cos(2*t)-2*cos(3*t)-cos(4*t));
				glVertex2f(v.x+x,v.y+y);
			}
		glEnd();
	}
};

Vector Li(Pont p0,Pont p1,Pont p2,float t){
	float t0=p0.suly;
	float t1=p1.suly;
	float t2=p2.suly;
	return p0.v*(((t-t1)*(t-t2))/((t0-t1)*(t0-t2)))+p1.v*(((t-t0)*(t-t2))/((t1-t0)*(t1-t2)))+p2.v*(((t-t0)*(t-t1))/((t2-t0)*(t2-t1)));
}

const int screenWidth = 600;
const int screenHeight = 600;

Pont pontKK[100];
Pont pontCR[100];
int pontok=0;
float bal=100;
float jobb=500;
float also=100;
float felso=500;

int jobbklik=0;
float egerx;
float egery;

bool vagas(float balsik,float jobbsik,float felsosik,float alsosik,Vector p0,Vector p1){
	unsigned char c1=0;
	unsigned char c0=0;
	if(p0.x<balsik)c0=1;
	if(p0.x>jobbsik)c0=c0+2;
	if(p0.y>felsosik)c0=c0+4;
	if(p0.y<alsosik)c0=c0+8;
	if(p1.x<balsik)c1=1;
	if(p1.x>jobbsik)c1=c1+2;
	if(p1.y>felsosik)c1=c1+4;
	if(p1.y<alsosik)c1=c1+8;
	while(1){
		if(c0==0 && c1==0)return true;
		if((c0&c1)!=0)return false;
		unsigned char f;
		Vector px;
		Vector irany=p1-p0;
		if((c0|c1)&1){
			f=1;
			px.x=balsik;
			px.y=(irany.y*px.x-irany.y*p0.x+irany.x*p0.y)/irany.x;
		}
		else if((c0|c1)&2){
			f=2;
			px.x=jobbsik;
			px.y=(irany.y*px.x-irany.y*p0.x+irany.x*p0.y)/irany.x;
		}
		else if((c0|c1)&4){
			f=4;
			px.y=felsosik;
			px.x=(irany.y*p0.x-irany.x*p0.x+irany.x*px.y)/irany.y;
		}
		else if((c0|c1)&8){
			f=8;
			px.y=alsosik;
			px.x=(irany.y*p0.x-irany.x*p0.x+irany.x*px.y)/irany.y;
		}
		unsigned char cx=0;
		if(px.x<balsik)cx=1;
		if(px.x>jobbsik)cx=cx+2;
		if(px.y>felsosik)cx=cx+4;
		if(px.y<alsosik)cx=cx+8;
		if(c0&f){
			p0=px;
			c0=cx;
		}
		else{
			p1=px;
			c1=cx;
		}
	}	
}

int kijelol(){
	float balsik=egerx-2.5*(jobb-bal)/600.0;
	float jobbsik=egerx+2.5*(jobb-bal)/600.0;
	float felsosik=egery+2.5*(felso-also)/600.0;
	float alsosik=egery-2.5*(felso-also)/600.0;
	Vector elozo;
	for(int i=3;i<pontok;i++){
		for(int j=0;j<=1000/(pontok-3);j++){
			Vector a=Li(pontKK[i-3],pontKK[i-2],pontKK[i-1],pontKK[i-2].suly+(float)j*(pontKK[i-1].suly-pontKK[i-2].suly)/(float)(1000/(pontok-3)));
			Vector b=Li(pontKK[i-2],pontKK[i-1],pontKK[i],pontKK[i-2].suly+(float)j*(pontKK[i-1].suly-pontKK[i-2].suly)/(float)(1000/(pontok-3)));
			Vector f=b*(float)j/(float)(1000/(pontok-3))+a*(1-(float)j/(float)(1000/(pontok-3)));
			if(!(i==3 && j==0)){
				if(vagas(balsik,jobbsik,felsosik,alsosik,elozo,f))return 1;
			}
			elozo=f;
		}
	}
	for(int i=3;i<pontok;i++){
		for(int j=0;j<=1000/(pontok-3);j++){
			float t=pontCR[i-2].suly+(float)j*(pontCR[i-1].suly-pontCR[i-2].suly)/(float)(1000/(pontok-3));
			Vector v=((pontCR[i-2].v-pontCR[i-3].v)/(pontCR[i-2].suly-pontCR[i-3].suly)+(pontCR[i-1].v-pontCR[i-2].v)/(pontCR[i-1].suly-pontCR[i-2].suly))*0.5;
			Vector vp=((pontCR[i-1].v-pontCR[i-2].v)/(pontCR[i-1].suly-pontCR[i-2].suly)+(pontCR[i].v-pontCR[i-1].v)/(pontCR[i].suly-pontCR[i-1].suly))*0.5;
			Vector a=(vp+v)/((pontCR[i-1].suly-pontCR[i-2].suly)*(pontCR[i-1].suly-pontCR[i-2].suly))-((pontCR[i-1].v-pontCR[i-2].v)*2)/((pontCR[i-1].suly-pontCR[i-2].suly)*(pontCR[i-1].suly-pontCR[i-2].suly)*(pontCR[i-1].suly-pontCR[i-2].suly));
			Vector b=((pontCR[i-1].v-pontCR[i-2].v)*3)/((pontCR[i-1].suly-pontCR[i-2].suly)*(pontCR[i-1].suly-pontCR[i-2].suly))-(vp+v*2)/(pontCR[i-1].suly-pontCR[i-2].suly);
			Vector c=v;
			Vector d=pontCR[i-2].v;
			Vector f=a*(t-pontCR[i-2].suly)*(t-pontCR[i-2].suly)*(t-pontCR[i-2].suly)+b*(t-pontCR[i-2].suly)*(t-pontCR[i-2].suly)+c*(t-pontCR[i-2].suly)+d;
			if(!(i==3 && j==0)){
				if(vagas(balsik,jobbsik,felsosik,alsosik,elozo,f))return 2;
			}
			elozo=f;
		}
	}
	return 0;
}


void onInitialization( ) { 
	glViewport(0, 0, screenWidth, screenHeight);
}


void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ..
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(bal,jobb,also,felso);
	
	for(int i=0;i<pontok;i++){
		pontKK[i].rajzol();
		pontCR[i].rajzol();
	}
	if(jobbklik==2)glColor3f(0, 0, 0);
	else glColor3f(0, 1, 0);
	for(int i=3;i<pontok;i++){
		glBegin(GL_LINE_STRIP);
			for(int j=0;j<=1000/(pontok-3);j++){
				float t=pontCR[i-2].suly+(float)j*(pontCR[i-1].suly-pontCR[i-2].suly)/(float)(1000/(pontok-3));
				Vector v=((pontCR[i-2].v-pontCR[i-3].v)/(pontCR[i-2].suly-pontCR[i-3].suly)+(pontCR[i-1].v-pontCR[i-2].v)/(pontCR[i-1].suly-pontCR[i-2].suly))*0.5;
				Vector vp=((pontCR[i-1].v-pontCR[i-2].v)/(pontCR[i-1].suly-pontCR[i-2].suly)+(pontCR[i].v-pontCR[i-1].v)/(pontCR[i].suly-pontCR[i-1].suly))*0.5;
				Vector a=(vp+v)/((pontCR[i-1].suly-pontCR[i-2].suly)*(pontCR[i-1].suly-pontCR[i-2].suly))-((pontCR[i-1].v-pontCR[i-2].v)*2)/((pontCR[i-1].suly-pontCR[i-2].suly)*(pontCR[i-1].suly-pontCR[i-2].suly)*(pontCR[i-1].suly-pontCR[i-2].suly));
				Vector b=((pontCR[i-1].v-pontCR[i-2].v)*3)/((pontCR[i-1].suly-pontCR[i-2].suly)*(pontCR[i-1].suly-pontCR[i-2].suly))-(vp+v*2)/(pontCR[i-1].suly-pontCR[i-2].suly);
				Vector c=v;
				Vector d=pontCR[i-2].v;
				Vector f=a*(t-pontCR[i-2].suly)*(t-pontCR[i-2].suly)*(t-pontCR[i-2].suly)+b*(t-pontCR[i-2].suly)*(t-pontCR[i-2].suly)+c*(t-pontCR[i-2].suly)+d;
				glVertex2f(f.x,f.y);
			}
		glEnd();
	}
	
	if(jobbklik==1)glColor3f(0, 0, 0);
	else glColor3f(1, 0, 0);
	for(int i=3;i<pontok;i++){
		glBegin(GL_LINE_STRIP);
			for(int j=0;j<=1000/(pontok-3);j++){
				Vector a=Li(pontKK[i-3],pontKK[i-2],pontKK[i-1],pontKK[i-2].suly+(float)j*(pontKK[i-1].suly-pontKK[i-2].suly)/(float)(1000/(pontok-3)));
				Vector b=Li(pontKK[i-2],pontKK[i-1],pontKK[i],pontKK[i-2].suly+(float)j*(pontKK[i-1].suly-pontKK[i-2].suly)/(float)(1000/(pontok-3)));
				glVertex2f(b.x*(float)j/(float)(1000/(pontok-3))+a.x*(1-(float)j/(float)(1000/(pontok-3))),b.y*(float)j/(float)(1000/(pontok-3))+a.y*(1-(float)j/(float)(1000/(pontok-3))));
			}
		glEnd();
	}

    // ...

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'z'){
		float vizszintes=jobb-bal;
		float fuggoleges=felso-also;
		bal=bal+((float)x/600.0)*(0.9*vizszintes);
		felso=felso-((float)y/600.0)*(0.9*fuggoleges);
		jobb=bal+vizszintes*0.1;
		also=felso-fuggoleges*0.1;
		glutPostRedisplay( );
	}
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && pontok<100){
		pontKK[pontok].v.x=pontCR[pontok].v.x=bal+((float)x/600.0)*(jobb-bal);
		pontKK[pontok].v.y=pontCR[pontok].v.y=felso-((float)y/600.0)*(felso-also);
		pontKK[pontok].suly=pontCR[pontok].suly=pontok?pontCR[pontok-1].suly+pontok:1;
		pontok++;
		glutPostRedisplay();
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		if(jobbklik==0){
			egerx=bal+((float)x/600.0)*(jobb-bal);
			egery=felso-((float)y/600.0)*(felso-also);
			jobbklik=kijelol();
		}
		else{
			egerx=bal+((float)x/600.0)*(jobb-bal)-egerx;
			egery=felso-((float)y/600.0)*(felso-also)-egery;
			if(jobbklik==1){
				for(int i=0;i<pontok;i++){
					pontKK[i].v.x+=egerx;
					pontKK[i].v.y+=egery;
				}
			}
			if(jobbklik==2){
				for(int i=0;i<pontok;i++){
					pontCR[i].v.x+=egerx;
					pontCR[i].v.y+=egery;
				}
			}
			jobbklik=0;
		}
		glutPostRedisplay();
	}
}

void onIdle( ){}

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

