
//  AUTHOR:  Herb Yang
//  
#include <GL/glut.h>
#include <FreeImage.h>
#include <iostream>
#include <qrencode.h>
#include "lib/qrenc/qrenc.h" // part of libqrencode
#include "halftoneQR.h"
#include "readQR.h" // this calls zxing library

using namespace std;
GLint imageSizeX, imageSizeY, qrSizeX, qrSizeY;
GLubyte *image, *qr_image; // pointer to the image
GLubyte *output; // pointer to the output image
static GLuint texName, texNameOut;
int image_bpp, image_nChannel, qr_bpp, qr_nChannel;
int threshold = 128; // initial threshold for floyd steinberg
FIBITMAP *img, *qr_img;
char filename[100], qrFilename[100], qrText[100];
QRcode *qrCode, *halftonedQRCode;

// halftoning functions
// 1: dither
// 2: floyd and steinberg
void dither ()
{
	int i, j;
	int r,g,b;
	int p,q;
	int m[4][4] ={{1, 7, 10,16},
	{12,14,3,5},
	{8,2,15,9},
	{13,11,6,4}};
	for (i = 0; i < imageSizeY; i++) {
        for (j = 0; j < imageSizeX; j++) {
            r = (int) image[(i * imageSizeX + j) * 3];            //red
            g = (int) image[(i * imageSizeX + j) * 3 + 1];        //green
            b = (int) image[(i * imageSizeX + j) * 3 + 2];        //blue
            p = i % 4;
            q = j % 4;
            r = (int) ((float) r / 255.0 * 16);
            g = (int) ((float) g / 255.0 * 16);
            b = (int) ((float) b / 255.0 * 16);
            if (r < m[q][p]) r = 0;
            else r = 255;
            if (g < m[q][p]) g = 0;
            else g = 255;
            if (b < m[q][p]) b = 0;
            else b = 255;
            output[(i * imageSizeX + j) * 3] = (GLubyte) r;        //red
            output[(i * imageSizeX + j) * 3 + 1] = (GLubyte) g;        //green
            output[(i * imageSizeX + j) * 3 + 2] = (GLubyte) b;        //blue
        }
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
// swap the red and blue channels - this is because
// FreeImage loads the image in the format BGR and not RGB as used in OpenGL
void swapRedwithBlue(GLubyte *imageptr, GLint sizeX, GLint sizeY){
	// need to swap the channels	
	GLubyte a;
	for (int i = 0; i < sizeY; i++){
		for (int j = 0; j < sizeX; j++){
			a = imageptr[(i*sizeX + j) * 3]; // blue
			imageptr[(i*sizeX + j) * 3] = imageptr[(i*sizeX + j)*3+ 2];
			imageptr[(i*sizeX + j) * 3 + 2] = a;
		}
	}
}
// load an image (can be an image to be halftoned, or a QR image)
void loadImage (char *filename, GLint &sizeX, GLint &sizeY, int &bpp, int &nChannel, GLubyte **imageptr, FIBITMAP **img) {
    FREE_IMAGE_FORMAT formato = FreeImage_GetFIFFromFilename(filename);
    if (formato == FIF_UNKNOWN) {
        cout << "image format unknown\n";
        exit(1);
    }
    if (!(*img)) FreeImage_Unload(*img);
    *img = FreeImage_Load(formato, filename);
    if (!(*img)) {
        cout << "image not found!";
        exit(1);
    }
    sizeX = FreeImage_GetWidth(*img);
    sizeY = FreeImage_GetHeight(*img);
    bpp = FreeImage_GetBPP(*img);
    nChannel = bpp / 8;
    cout << "X =" << sizeX << " Y = " << sizeY << " bpp =" << bpp << " nChannel =" << nChannel << "\n";
    if (nChannel == 1) {
        cout << "convert to 24 bit before running this program\n";
        exit(1);
    } else {
        if (!(*imageptr)) {
            delete[] (*imageptr);
        }
        *imageptr = FreeImage_GetBits(*img);
        if (*imageptr == nullptr) {
            cout << "Null pointer in image\n";
            exit(1);
        }
    }
    swapRedwithBlue(*imageptr, sizeX, sizeY);
}
// remove old output and create new one
void cleanOutput () {
    if (!output) {
        delete[] output;
    }
    output = new GLubyte[imageSizeX*imageSizeY*3];
}

//  OpenGL window handling routines
//  initialize some opengl parameters
void initGL(int w, int h) {
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
void display() {
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
//init texture
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
void reshape(GLsizei newwidth, GLsizei newheight) {
	glutReshapeWindow(imageSizeX*2 , imageSizeY); // do not let the window change size
}
// mouse handler
void mouse(int button, int state, int x, int y) {
	if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN)
	{
		// do something with the mouse
		glutPostRedisplay();
	}
}
// keyboard handler
void keyPressed (unsigned char key, int x, int y) { 
	switch(key) {
	case '1': // dithering
		dither();
		break;
	case '2': // Floyd and Steinberg
		floyd();
		break;
//	case 'f': // load a new image file
//		cout<<"Input filename:";
//		cin>>filename;
//		loadImage(filename);
//      removeOutput();
//		glutReshapeWindow(imageSizeX*2, imageSizeY);
//		initGL(imageSizeX*2 , imageSizeY);
//		break;
    default:
        break;
	}
	glutPostRedisplay();
}
//  ------- Main: Initialize glut window and register call backs ---------- 
int main(int argc, char **argv) {
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // read in image
    FreeImage_Initialise();
    strcpy(filename, "house-bw-noal.png");
    loadImage(filename, imageSizeX, imageSizeY, image_bpp, image_nChannel, &image, &img);
    cleanOutput();
    // generate QR code encoding a text
    strcpy(qrText, "house");
    qrCode = QRcode_encodeString(qrText, 4, QR_ECLEVEL_H, QR_MODE_8, 1);
    // halftone the generated QR code
    halftonedQRCode = halftoneQR(qrCode, image);
    // store the halftoned QR code into a .png file
    strcpy(qrFilename, "output.png");
    writePNG(halftonedQRCode, qrFilename);
    QRcode_free(qrCode);
    delete[] halftonedQRCode->data;
    delete halftonedQRCode;
    // read the text from the .png file of the halftoned QR code
    int success = readQR(qrFilename);
//    loadImage(qrFilename, qrSizeX, qrSizeY, qr_bpp, qr_nChannel, &qr_image, &qr_img);
//    glutInitWindowSize(imageSizeX*2 , imageSizeY);
//    glutInitWindowPosition(0,0);
//    glutCreateWindow("Project");
//    initGL(imageSizeX*2 , imageSizeY);
//    initTexture();// This is new.
//    glutDisplayFunc(display);
//    glutReshapeFunc(reshape);
//    glutMouseFunc(mouse);
//    glutKeyboardFunc(keyPressed);
//    glutMainLoop();
    return 0;
}