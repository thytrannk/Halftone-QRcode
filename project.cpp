
//  AUTHOR:  Herb Yang
//  

#include <iostream>
#include "project.h"
#include "lib/qrenc/qrenc.h" // part of libqrencode
#include "halftoneQR.h"
#include "readQR.h" // this calls zxing library

using namespace std;

// define extern variables
int imageSizeX, imageSizeY, impSizeX, impSizeY, qrSizeX, qrSizeY;
BYTE *image, *imp_map, *qr_image; // pointer to the image
BYTE *output; // pointer to the output image
int image_bpp, image_nChannel, imp_bpp, imp_nChannel, qr_bpp, qr_nChannel;
FIBITMAP *img, *imp, *qr_img;
char filename[100], qrFilename[100], qrText[100];
QRcode *qrCode, *halftonedQRCode;

// swap the red and blue channels - this is because
// FreeImage loads the image in the format BGR and not RGB as used in OpenGL
void swapRedwithBlue(BYTE *imageptr, int sizeX, int sizeY){
	// need to swap the channels	
	BYTE a;
	for (int i = 0; i < sizeY; i++){
		for (int j = 0; j < sizeX; j++){
			a = imageptr[(i*sizeX + j) * 3]; // blue
			imageptr[(i*sizeX + j) * 3] = imageptr[(i*sizeX + j)*3+ 2];
			imageptr[(i*sizeX + j) * 3 + 2] = a;
		}
	}
}
// load an image (can be an image to be halftoned, or a QR image)
void loadImage (char *filename, int &sizeX, int &sizeY, int &bpp, int &nChannel, BYTE **imageptr, FIBITMAP **img) {
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
    output = new BYTE[imageSizeX*imageSizeY*3];
}

//  ------- Main: Initialize glut window and register call backs ---------- 
int main(int argc, char **argv) {
    // read in image
    FreeImage_Initialise();
    strcpy(filename, "house-bw.png");
    loadImage(filename, imageSizeX, imageSizeY, image_bpp, image_nChannel, &image, &img);
    strcpy(filename, "house-bw-imp-map.png");
    loadImage(filename, impSizeX, impSizeY, imp_bpp, imp_nChannel, &imp_map, &imp);
    cleanOutput();
    // generate QR code encoding a text
    strcpy(qrText, "house");
    qrCode = QRcode_encodeString(qrText, 4, QR_ECLEVEL_H, QR_MODE_8, 1);
    // halftone the generated QR code
    halftonedQRCode = halftoneQR(qrCode, image, imp_map, imageSizeX, imageSizeY);
    // store the halftoned QR code into a .png file
    strcpy(qrFilename, "output.png");
    writePNG(halftonedQRCode, qrFilename);
    QRcode_free(qrCode);
    delete[] halftonedQRCode->data;
    delete halftonedQRCode;
    // read the text from the .png file of the halftoned QR code
    int success = readQR(qrFilename);

    return 0;
}