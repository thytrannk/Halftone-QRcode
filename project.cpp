
//  AUTHOR:  Herb Yang
//  
#include <GL/glut.h>
#include <FreeImage.h>
#include <iostream>
using namespace std;
GLint imageSizeX, imageSizeY;
GLubyte * image; // pointer to the image
GLubyte * output; // pointer to the output image
static GLuint texName, texNameOut;
int bpp, nChannel;
int threshold = 125; // initial threshold for floyd steinberg
int matrixChoice = 1;// in dot diffusion, there are two choices - 1 or 2
FIBITMAP* img;
char filename[100], outFilename[100];

// halftoning functions
// 1: dither
// 2: floyd and steinberg
// 3: dot diffusion
void dither()
{
	int i, j;
	int r,g,b;
	int p,q;
	int m[4][4] ={{1, 7, 10,16},
	{12,14,3,5},
	{8,2,15,9},
	{13,11,6,4}};
	for(i=0;i<imageSizeY;i++)
		for(j=0;j<imageSizeX;j++)
		{
			r =(int) image[(i*imageSizeX+j)*3];		    //red
			g =(int)  image[(i*imageSizeX+j)*3+1];		//green
			b =(int)  image[(i*imageSizeX+j)*3+2];		//blue
			p =  i%4;
			q  = j%4;
			r = (float) r/255.0 * 16;
			g = (float) g/255.0 * 16;
			b = (float) b/255.0 * 16;
			if(r<m[q][p]) r = 0;
			else r = 255;
			if(g<m[q][p]) g = 0;
			else g = 255;
			if(b<m[q][p]) b = 0;
			else b = 255;
			output[(i*imageSizeX+j)*3]	=r;		//red
			output[(i*imageSizeX+j)*3+1]=g;		//green
			output[(i*imageSizeX+j)*3+2]=b;		//blue
		}
}

void floyd()
{
#define ALPHA 0.4375
#define BETA 0.1875
#define GAMMA 0.3125
#define DELTA 0.0625
// add code in here

    int x, y, i;
    int r,g,b;
    float raw_r, raw_g, raw_b;
    float error_r, error_g, error_b;

    float *image_new = new float[imageSizeX*imageSizeY*3];

    for (i = 0; i < imageSizeX * imageSizeY * 3; i++) {
        image_new[i] = (float) image[i];
    }

    for(y = imageSizeY - 1; y >= 0; y--) {
		// y is y-coordinate; small y is at the bottom of the image
        for (x = 0; x < imageSizeX; x++) {
			// x is x-coordinate
			int current, alpha_nb, beta_nb, gamma_nb, delta_nb;
			// Location of current pixel and neighboring pixels
			current = y * imageSizeX + x;
			alpha_nb = y * imageSizeX + x + 1;
			beta_nb = (y - 1) * imageSizeX + x - 1;
			gamma_nb = (y - 1) * imageSizeX + x;
			delta_nb = (y - 1) * imageSizeX + x + 1;

            raw_r = image_new[current * 3];            //red
            raw_g = image_new[current * 3 + 1];        //green
            raw_b = image_new[current * 3 + 2];        //blue

            if (raw_r < threshold) {
                r = 0;
            } else {
                r = 255;
            }
            if (raw_g < threshold) {
                g = 0;
            } else {
                g = 255;
            }
            if (raw_b < threshold) {
                b = 0;
            } else {
                b = 255;
            }

            error_r = raw_r - r;
            error_g = raw_g - g;
            error_b = raw_b - b;

            if (x + 1 < imageSizeX) {
                image_new[alpha_nb * 3] += ALPHA * error_r;
                image_new[alpha_nb * 3 + 1] += ALPHA * error_g;
                image_new[alpha_nb * 3 + 2] += ALPHA * error_b;
            }
            if (y - 1 >= 0 && x > 0) {
                image_new[beta_nb * 3] += BETA * error_r;
                image_new[beta_nb * 3 + 1] += BETA * error_g;
                image_new[beta_nb * 3 + 2] += BETA * error_b;
            }
            if (y - 1 >= 0) {
                image_new[gamma_nb * 3] += GAMMA * error_r;
                image_new[gamma_nb * 3 + 1] += GAMMA * error_g;
                image_new[gamma_nb * 3 + 2] += GAMMA * error_b;
            }
            if (y - 1 >= 0 && x + 1 < imageSizeX) {
                image_new[delta_nb * 3] += DELTA * error_r;
                image_new[delta_nb * 3 + 1] += DELTA * error_g;
                image_new[delta_nb * 3 + 2] += DELTA * error_b;
            }

            output[current * 3] = (GLubyte) r;        //red
            output[current * 3 + 1] = (GLubyte) g;        //green
            output[current * 3 + 2] = (GLubyte) b;        //blue
        }
    }

	delete[] image_new;
}
// end of floyd-steinberg
//
// dot diffusion
//
int  weight(int x, int y)
{
	int w = 0;
	// add code in here
	w = 3 - x*x - y*y;
	return w;
}
bool  isValidNeighbour(int x, int y)
{
	bool t = false;
	// add code in here
    if (x >= 0 && x < imageSizeX && y >= 0 && y < imageSizeY) {
        t = true;
    }
	return t;
}
int classRank(int x, int y)
{
	int i, j, r;
	int m1[8][8]=//M1
	{{0,32,8,40,2,34,10,42},
	{48,16,56,24,50,18,58,26},
	{12,44,4,36,14,46,6,38},
	{60,28,52,20,62,30,54,22},
	{3,35,11,43,1,33,9,41},
	{51,19,59,27,49,17,57,25},
	{15,47,7,39,13,45,5,37},
	{63,31,55,23,61,29,53,21}};

	int m2[8][8] =// M2
	{{34,48,40,32,29,15,23,31},
	{42,58,56,53,21, 5, 7,10},
	{50,62,61,45,13, 1, 2,18},
	{38,46,54,37,25,17, 9,26},
	{28,14,22,30,35,49,41,33},
	{20,4,  6,11,43,59,57,52},
	{12,0,  3,19,51,63,60,44},
	{24,16, 8,27,39,47,55,36}};
	// add code in here
    // The top row has y == 255, and the bottom row has y == 0
    // Thus, the maintain the orientation, we have to convert the y-coordinate to the row number by calculating imageSizeY-1-y
    i =  x%8;
    j  = (imageSizeY-1-y)%8;
    if (matrixChoice == 1) {
        r = m1[j][i];
    } else {
        r = m2[j][i];
    }

	return r;
}

void accumNeighbour(int u, int v, int i, int j, int &w) {
    if (classRank(u, v) > classRank(i, j)) {
        // The top row has y == 255, and the bottom row has y == 0
        // Thus, the maintain the orientation, we have to convert the y-coordinate to the row number by calculating imageSizeY-1-y
        // As a result, v - j has to be switched to j - v (because the y-axis is upside-down)
        w += weight(u-i, j-v);
    }
}

void diffuseNeighbour(float *image_new, int u, int v, int i, int j, float error_r, float error_g, float error_b, int w) {
    // The top row has y == 255, and the bottom row has y == 0
    // Thus, the maintain the orientation, we have to convert the y-coordinate to the row number by calculating imageSizeY-1-y
    // As a result, v - j has to be switched to j - v (because the y-axis is upside-down)
    image_new[(v * imageSizeX + u) * 3] += error_r * weight(u - i, j - v) / w;
    image_new[(v * imageSizeX + u) * 3 + 1] += error_g * weight(u - i, j - v) / w;
    image_new[(v * imageSizeX + u) * 3 + 2] += error_b * weight(u - i, j - v) / w;
}

void dotDiffusion()
{
#define NEIGHBOURHOODSIZE 1 // other values do not seem to work well
	// add code in here
    int x, y, i, k;
    int r,g,b;
    int w;
    float raw_r, raw_g, raw_b;
    float error_r, error_g, error_b;

	float *image_new = new float[imageSizeX*imageSizeY * 3];

    for (i = 0; i < imageSizeX * imageSizeY * 3; i++) {
        image_new[i] = (float) image[i];
    }

    for(k = 0; k < 64; k++) {
        for(y = imageSizeY - 1; y >= 0; y--) {
            // y is y-coordinate; small y is at the bottom of the image
            for (x = 0; x < imageSizeX; x++) {
				//x is x-coordinate
                if (classRank(x, y) == k) {
                    raw_r = image_new[(y * imageSizeX + x) * 3];            //red
                    raw_g = image_new[(y * imageSizeX + x) * 3 + 1];        //green
                    raw_b = image_new[(y * imageSizeX + x) * 3 + 2];        //blue

                    if (raw_r < threshold) {
                        r = 0;
                    } else {
                        r = 255;
                    }
                    if (raw_g < threshold) {
                        g = 0;
                    } else {
                        g = 255;
                    }
                    if (raw_b < threshold) {
                        b = 0;
                    } else {
                        b = 255;
                    }
                    error_r = raw_r - r;
                    error_g = raw_g - g;
                    error_b = raw_b - b;

                    w = 0;

                    for (int u = x - NEIGHBOURHOODSIZE; u <= x + NEIGHBOURHOODSIZE; u++) {
                    	for (int v = y - NEIGHBOURHOODSIZE; v <= y + NEIGHBOURHOODSIZE; v++) {
                    		// if (u,v) is not (x,y) and (u,v) is within image
                    		if ((u != x || v != y) && isValidNeighbour(u, v)) {
                    			accumNeighbour(u, v, x, y, w);
                    		}
                    	}
                    }

                    if (w > 0) {
						for (int u = x - NEIGHBOURHOODSIZE; u <= x + NEIGHBOURHOODSIZE; u++) {
							for (int v = y - NEIGHBOURHOODSIZE; v <= y + NEIGHBOURHOODSIZE; v++) {
								// if (u,v) is not (x,y) and (u,v) is within image
								if ((u != x || v != y) && isValidNeighbour(u, v)) {
									diffuseNeighbour(image_new, u, v, x, y, error_r, error_g, error_b, w);
								}
							}
						}
					}

                    output[(y * imageSizeX + x) * 3] = (GLubyte) r;        //red
                    output[(y * imageSizeX + x) * 3 + 1] = (GLubyte) g;        //green
                    output[(y * imageSizeX + x) * 3 + 2] = (GLubyte) b;        //blue
                }
            }
        }
    }

	delete[] image_new;
}
// end of dot diffusion
//
// swap the red and blue channels - this is because
// FreeImage loads the image in the format BGR and not RGB as used in OpenGL
void swapRedwithBlue(){
	// need to swap the channels	
	GLubyte a;
	for(int i = 0; i< imageSizeY; i++){
		for(int j = 0; j<imageSizeX; j++){
			a = image[(i*imageSizeX + j)*3]; // blue
			image[(i*imageSizeX + j)*3] = image[(i*imageSizeX + j)*3+ 2];
			image[(i*imageSizeX + j)*3+ 2] = a;
		}
	}
}

// void saveImage(char * filename, GLubyte * buf);
void saveImage(char * filename, GLubyte  * buf){
	RGBQUAD color;
	FIBITMAP *im = FreeImage_Allocate(imageSizeX, imageSizeY, bpp);
	for(int i=0; i<imageSizeY;i++)
		for(int j=0;j<imageSizeX;j++)
		{
			color.rgbRed = buf[(i*imageSizeX+j)*3];
			color.rgbGreen = buf[(i*imageSizeX+j)*3+1];
			color.rgbBlue = buf[(i*imageSizeX+j)*3+2];
			FreeImage_SetPixelColor(im,j,i, &color);
		}
		if(FreeImage_Save(FIF_BMP, im, filename, 0))
			cout << filename<<" saved \n";
}
// void loadImage(filename)
void loadImage(char * filename){
    FREE_IMAGE_FORMAT formato = FreeImage_GetFIFFromFilename(filename);
    if(formato == FIF_UNKNOWN){
        cout<<"image format unknown\n";
        exit(1);
    }
    if (!img)FreeImage_Unload(img);// this is new
    img = FreeImage_Load(formato, filename);
    if(!img){
        cout<<"image not found!";
        exit(1);
    }
    imageSizeX = FreeImage_GetWidth(img);
    imageSizeY = FreeImage_GetHeight(img);
    bpp = FreeImage_GetBPP(img);
    nChannel = bpp/8;
    cout<<"X ="<<imageSizeX<<" Y = "<<imageSizeY<<" bpp ="<<bpp<<" nChannel ="<<nChannel<<"\n";
    if(nChannel ==1){
        cout<<"convert to 24 bit before running this program\n";
        exit(1);
    } else {
        if(!image){
            delete[] image;
        }
        image = (GLubyte*) FreeImage_GetBits(img);
        if(image == NULL){
            cout<<"Null pointer in image\n";
            exit(1);
        }
    }
    if(!output){
        delete[] output;
    }
    output = new GLubyte[ imageSizeX*imageSizeY*3 ];
    swapRedwithBlue();
    return;
}

//  OpenGL window handling routines
//  initialize some opengl parameters
void initGL(int w, int h) 
{
	glViewport(0, 0, w, h);			// use a screen size of WIDTH x HEIGHT
	glEnable(GL_TEXTURE_2D);		// Enable 2D texturing
	glMatrixMode(GL_PROJECTION);    // Make a simple 2D projection on the entire window
	glLoadIdentity();
	glOrtho(0.0, w, h, 0.0, 0.0, 100.0);
	glMatrixMode(GL_MODELVIEW);		// Set the matrix mode to object modeling
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
}
//  display function
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGB  ,imageSizeX, imageSizeY, 0,  GL_RGB ,GL_UNSIGNED_BYTE, (void*)image );
    /* Draw a quad */
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0,   imageSizeY);
    glTexCoord2i(0, 1); glVertex2i(0,   0);
    glTexCoord2i(1, 1); glVertex2i(imageSizeX, 0);
    glTexCoord2i(1, 0); glVertex2i(imageSizeX,imageSizeY);
    glEnd();
    glBindTexture(GL_TEXTURE_2D,texNameOut);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,imageSizeX, imageSizeY, 0, GL_RGB,GL_UNSIGNED_BYTE, (void*)output );
    /* Draw another quad */
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(imageSizeX,   imageSizeY);
    glTexCoord2i(0, 1); glVertex2i(imageSizeX,   0);
    glTexCoord2i(1, 1); glVertex2i(imageSizeX*2, 0);
    glTexCoord2i(1, 0); glVertex2i(imageSizeX*2,imageSizeY);
    glEnd();
    glutSwapBuffers();
    glFlush();
}
//initTexture()
//This is new.
void initTexture() {
    glBindTexture(GL_TEXTURE_2D, texName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenTextures(1, &texNameOut);
    glBindTexture(GL_TEXTURE_2D, texNameOut);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
// reshape function
void reshape(GLsizei newwidth, GLsizei newheight) 
{  
	glutReshapeWindow(imageSizeX*2 , imageSizeY); // do not let the window change size
}
// mouse handler
void mouse(int button, int state, int x, int y)
{
	if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN)
	{
		// do something with the mouse
		glutPostRedisplay();
	}
}
// keyboard handler
void keyPressed (unsigned char key, int x, int y) { 
	switch(key){
	case '1': // dithering
		cout<<"Dithering...\n";
		dither();
		break;
	case '2': // Floyd and Steinberg
		cout<<"Floyd and Steinberg...\n";
		cout<<"Threshold [0-255]: ";
		cin>> threshold;
		floyd();
		break;
	case '3': // dot diffusion
		cout<<"Dot diffusion...\n";
		cout<<"Threshold [0-255]: ";
		cin>> threshold;
		cout<<"Matrix choice [1 or 2]:";
		cin>>matrixChoice;
		dotDiffusion();
		break;
	case 'f': // load a new image file
		cout<<"Input filename:";
		cin>>filename;
		loadImage(filename);	
		glutReshapeWindow(imageSizeX*2, imageSizeY);
		initGL(imageSizeX*2 , imageSizeY);
		break;
	case 's': // save the output image file in bmp format
		cout<<"Output filename:";
		cin>>outFilename;
		saveImage(outFilename, output);
		break;

	}
	glutPostRedisplay();
}
//  ------- Main: Initialize glut window and register call backs ---------- 
// void main(int argc, char **argv)
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
    // read in image
    FreeImage_Initialise();
    cout<<"Input filename:";
    cin>>filename;
    loadImage(filename);
    glutInitWindowSize(imageSizeX*2 , imageSizeY);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Halftoning");
    initGL(imageSizeX*2 , imageSizeY);
    initTexture();// This is new.
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyPressed);
    glutMainLoop();
}