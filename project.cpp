
//  AUTHOR:  Herb Yang
//  

#include <iostream>
#include "project.h"
#include "lib/qrenc/qrenc.h" // part of libqrencode
#include "halftoneQR.h"
#include "readQR.h" // this calls zxing library

using namespace std;

// define extern variables
int imageSizeX, imageSizeY;
BYTE *image; // pointer to the image
BYTE *halftone; // pointer to the halftone image
int image_bpp;
int image_nChannel;
FIBITMAP *img;
char filename[100], qrFilename[100], qrText[100];
QRcode *qrCode, *halftonedQRCode;
int threshold = 128; // initial threshold for floyd steinberg

// load an image
void loadImage (char *filename, int &sizeX, int &sizeY, int &bpp, int &nChannel, BYTE **imageptr, FIBITMAP **img) {
    FREE_IMAGE_FORMAT formato = FreeImage_GetFIFFromFilename(filename);
    if (formato == FIF_UNKNOWN) {
        cout << "Image format is unknown!\n";
        exit(1);
    }
    if (!(*img)) FreeImage_Unload(*img);
    *img = FreeImage_Load(formato, filename, RAW_DISPLAY);
    if (!(*img)) {
        cout << "Image cannot be found!";
        exit(1);
    }
    // Convert non-32 bit images
    if (FreeImage_GetBPP(*img) != 32) {
        FIBITMAP* oldImage = *img;
        *img = FreeImage_ConvertTo32Bits(oldImage);
        FreeImage_Unload(oldImage);
    }

    sizeX = FreeImage_GetWidth(*img);
    sizeY = FreeImage_GetHeight(*img);
    bpp = FreeImage_GetBPP(*img);
    nChannel = bpp / 8;
    cout << "X =" << sizeX << " Y = " << sizeY << " bpp =" << bpp << " nChannel =" << nChannel << "\n";

    cout << "Do you want to rescale the image? (y/n) ";
    char rescale;
    cin >> rescale;
    if (rescale == 'y' || rescale == 'Y') {
        cout << "New width: ";
        int width;
        cin >> width;
        cout << "New height: ";
        int height;
        cin >> height;
        FIBITMAP *temp = *img;
        *img = FreeImage_Rescale(temp, width, height, FILTER_BOX);
        FreeImage_Unload(temp);
        sizeX = FreeImage_GetWidth(*img);
        sizeY = FreeImage_GetHeight(*img);
        bpp = FreeImage_GetBPP(*img);
        nChannel = bpp / 8;
        cout << "X =" << sizeX << " Y = " << sizeY << " bpp =" << bpp << " nChannel =" << nChannel << "\n";
    }
    if (!(*imageptr)) {
        delete[] (*imageptr);
    }
    *imageptr = FreeImage_GetBits(*img);
    if (*imageptr == nullptr) {
        cout << "Null pointer in image\n";
        exit(1);
    }

    FreeImage_Unload(*img);
}
// remove old halftone and create new one
void cleanOutput () {
    if (!halftone) {
        delete[] halftone;
    }
    halftone = new BYTE[imageSizeX*imageSizeY*3];
}

void saveImage(char *filename, BYTE *buf, int nChannel, int bpp){
    RGBQUAD color;
    FIBITMAP *im = FreeImage_Allocate(imageSizeX, imageSizeY, bpp);
    for (int i = 0; i < imageSizeY; i++) {
        for (int j = 0; j < imageSizeX; j++) {
            color.rgbRed = buf[(i * imageSizeX + j) * nChannel];
            color.rgbGreen = buf[(i * imageSizeX + j) * nChannel + 1];
            color.rgbBlue = buf[(i * imageSizeX + j) * nChannel + 2];
            FreeImage_SetPixelColor(im, j, i, &color);
        }
    }
    FreeImage_Save(FIF_BMP, im, filename, 0);
//    FreeImage_Unload(im);
}

//  ------- Main: Initialize glut window and register call backs ---------- 
int main(int argc, char **argv) {
    // read in image
    FreeImage_Initialise();
    // strcpy(filename, "cat-bw-small4.png");
    cout << "Object image file name: ";
    cin >> filename;
    loadImage(filename, imageSizeX, imageSizeY, image_bpp, image_nChannel, &image, &img);
    cleanOutput();
    strcpy(filename, "rescaled.bmp");
    saveImage(filename, image, image_nChannel, image_bpp);
    // generate QR code encoding a text
    strcpy(qrText, "house");
    qrCode = QRcode_encodeString(qrText, 6, QR_ECLEVEL_H, QR_MODE_8, 1);
    // halftone the generated QR code
    halftoneQR();
    // store the halftoned QR code into a .png file
    strcpy(qrFilename, "output.png");
    writePNG(halftonedQRCode, qrFilename);
    strcpy(filename, "halftone.bmp");
    saveImage(filename, halftone, image_nChannel, image_bpp);
    QRcode_free(qrCode);
//    delete[] halftonedQRCode->data;
//    delete halftonedQRCode;
    // read the text from the .png file of the halftoned QR code
    int result = readQR(qrFilename);
    if (result) {
        cout << "Could not decode QR code." << endl;
    }

    return 0;
}