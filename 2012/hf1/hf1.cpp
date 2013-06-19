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


#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GLUT/glut.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...


 
//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color( ) { 
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) { 
	r = r0; g = g0; b = b0;
   }
   Color operator*(float a) { 
	return Color(r * a, g * a, b * a); 
   }
   Color operator*(const Color& c) { 
	return Color(r * c.r, g * c.g, b * c.b); 
   }
   Color operator+(const Color& c) {
 	return Color(r + c.r, g + c.g, b + c.b); 
   }
};

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;


Color image[screenWidth*screenHeight];	// egy alkalmazás ablaknyi kép

float tvx=0;
float tvy=0;

float hvx=-2500;
float hvy=-2500;
float hix=1.0;
float hiy=0;

float gvx=2500;
float gvy=2500;
float gix=-1.0;
float giy=0;

float r=0.02;
long lasttime;
long connect=0;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
	glViewport(0, 0, screenWidth, screenHeight);

    tvx=(float)(rand()%10000)-5000;
	tvy=(float)(rand()%10000)-5000;
	
    for(int y = 0; y < screenHeight; y++){
		for(int x = 0; x < screenWidth; x++){
			float vx=((float)x/300-1)*5000;
			float vy=((float)y/300-1)*5000;
			float szin=(sin(vx/1500*vy/1500)+1)/2;
			image[y*screenWidth + x] = Color(szin,szin,szin);
		}
	}
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
	
	long time = glutGet(GLUT_ELAPSED_TIME);
	long elapsedtime=time-lasttime;
	lasttime=time;
	bool lathato;
	while(elapsedtime>0){
		int szelet;
		if(elapsedtime%100){
			szelet=elapsedtime%100;
		}
		else{
			szelet=100;
		}
		elapsedtime=elapsedtime-szelet;
		
		float hm=atan((192.0/1125000)*hvy*cos(hvx*hvy/2250000)*hix+(192.0/1125000)*hvx*cos(hvx*hvy/2250000)*hiy);
		float hv=szelet*cos(hm)*(1-hm*2/3.14159);
		hvy+=hiy*hv;
		if(fabs(hvy)>=5000){
			hiy=-hiy;
			hvy+=hiy*2*(fabs(hvy)-5000);
		}
		hvx+=hix*hv;
		if(fabs(hvx)>=5000){
			hix=-hix;
			hvx+=hix*2*(fabs(hvx)-5000);
		}
		
		float gm=atan((192.0/1125000)*gvy*cos(gvx*gvy/2250000)*gix+(192.0/1125000)*gvx*cos(gvx*gvy/2250000)*giy);
		float gv=szelet*cos(gm)*(1-gm*2/3.14159);
		gvy+=giy*gv;
		if(fabs(gvy)>=5000){
			giy=-giy;
			gvy+=giy*2*(fabs(gvy)-5000);
		}
		gvx+=gix*gv;
		if(fabs(gvx)>=5000){
			gix=-gix;
			gvx+=gix*2*(fabs(gvx)-5000);
		}
		
		lathato=true;
		
		float hvz=((sin(hvx/1500*hvy/1500)+1)*382)+250;
		float gvz=((sin(gvx/1500*gvy/1500)+1)*382)+250;
		float tvz=((sin(tvx/1500*tvy/1500)+1)*382)+270;
		
		for(int i=1;i<100;i++){
			float t=(float)i/100;
			float ax=hvx*t+tvx*(1-t);
			float ay=hvy*t+tvy*(1-t);
			float az=hvz*t+tvz*(1-t);
			float azb=((sin(ax/1500*ay/1500)+1)*382)+250;
			if(azb>az)lathato=false;
		}
		
		for(int i=1;i<100;i++){
			float t=(float)i/100;
			float ax=gvx*t+tvx*(1-t);
			float ay=gvy*t+tvy*(1-t);
			float az=gvz*t+tvz*(1-t);
			float azb=((sin(ax/1500*ay/1500)+1)*382)+250;
			if(azb>az)lathato=false;
		}
			
		if(lathato)connect+=szelet;
	
	}
	
	
	
	
    glClearColor(0.0, 0.0, 0.0, 0.0);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
    
	float tx=tvx/5000;
	float ty=tvy/5000;
	float gx=gvx/5000;
	float gy=gvy/5000;
	float hx=hvx/5000;
	float hy=hvy/5000;
	
	glColor3f(1, 1, 0);
	glBegin(GL_TRIANGLES);
		for (int i = 0; i < 3; i++){
			glVertex2f(r*sin(i*3.14159/1.5)+tx,r*cos(i*3.14159/1.5)+ty);
		}
	glEnd( );
	
	glColor3f(1, 0, 0.5);
	glBegin(GL_LINE_LOOP);
		for (int i = 0; i < 100; i++){
			glVertex2f(r*sin(i*3.14159/50.0)+gx,r*cos(i*3.14159/50.0)+gy);
		}
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(gx,gy-r);
		glVertex2f(gx,gy-r*3);
		glVertex2f(gx-r,gy-r*2);
		glVertex2f(gx+r,gy-r*2);
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_LINE_LOOP);
		for (int i = 0; i < 100; i++){
			glVertex2f(r*sin(i*3.14159/50.0)+hx,r*cos(i*3.14159/50.0)+hy);
		}
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(hx+sqrt(r*r/2),hy+sqrt(r*r/2));
		glVertex2f(hx+sqrt(9*r*r/2),hy+sqrt(9*r*r/2));
		glVertex2f(hx+sqrt(9*r*r/2),hy+sqrt(9*r*r/2));
		glVertex2f(hx+sqrt(9*r*r/2)-r,hy+sqrt(9*r*r/2));
		glVertex2f(hx+sqrt(9*r*r/2),hy+sqrt(9*r*r/2));
		glVertex2f(hx+sqrt(9*r*r/2),hy+sqrt(9*r*r/2)-r);
	glEnd();
	
	if(lathato){
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
			glVertex2f(hvx/5000,hvy/5000);
			glVertex2f(gvx/5000,gvy/5000);
			glVertex2f(tvx/5000,tvy/5000);
		glEnd( );
	}
	
	float arany=(float)connect/(float)time;
	
	glBegin(GL_QUADS);
		glColor3f(0, 1, 0);
		glVertex2f(-0.5,1);
		glVertex2f(arany-0.5,1);
		glVertex2f(arany-0.5,0.9);
		glVertex2f(-0.5,0.9);
		glColor3f(1, 0, 0);
		glVertex2f(arany-0.5,1);
		glVertex2f(0.5,1);
		glVertex2f(0.5,0.9);
		glVertex2f(arany-0.5,0.9);
	glEnd();
	

	//..
    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 't'){
		tvx=(float)(rand()%10000)-5000;
		tvy=(float)(rand()%10000)-5000;
		glutPostRedisplay( );
	}

}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN){
		float vx=((float)x/300-1)*5000;
		float vy=-(((float)y/300-1)*5000);
		gix=(vx-gvx)/(sqrt((vx-gvx)*(vx-gvx)+(vy-gvy)*(vy-gvy)));
		giy=(vy-gvy)/(sqrt((vx-gvx)*(vx-gvx)+(vy-gvy)*(vy-gvy)));
		glutPostRedisplay( ); 	
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		float vx=((float)x/300-1)*5000;
		float vy=-(((float)y/300-1)*5000);
		hix=(vx-hvx)/(sqrt((vx-hvx)*(vx-hvx)+(vy-hvy)*(vy-hvy)));
		hiy=(vy-hvy)/(sqrt((vx-hvx)*(vx-hvx)+(vy-hvy)*(vy-hvy)));
		glutPostRedisplay( ); 	
	}
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

