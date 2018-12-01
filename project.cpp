#include <iostream>
#include "project.h"
#include "lib/qrenc/qrenc.h" // part of libqrencode
#include "halftoneQR.h"
#include "lib/zxing/readQR.h" // this calls zxing library

using namespace std;

// define extern variables
int imageSizeX, imageSizeY;
BYTE *image; // pointer to the image
BYTE *halftone; // pointer to the halftone image
int image_bpp;
int image_nChannel;
FIBITMAP *img;
char filename[100], qrFilename[100];
string qrText;
QRcode *qrCode, *halftonedQRCode;
int threshold = 128; // initial threshold for floyd steinberg
int method = 1;

// load an image
void loadImage (char *filename) {
    FREE_IMAGE_FORMAT formato = FreeImage_GetFIFFromFilename(filename);
    if (formato == FIF_UNKNOWN) {
        cout << "Image format is unknown!\n";
        exit(1);
    }
    if (!img) FreeImage_Unload(img);
    img = FreeImage_Load(formato, filename, RAW_DISPLAY);
    if (!img) {
        cout << "Image cannot be found!";
        exit(1);
    }
    // Convert to 32 bit greyscale images

    FIBITMAP* tempImg;
    tempImg = FreeImage_ConvertToGreyscale(img);
    FreeImage_Unload(img);
    img = FreeImage_ConvertTo32Bits(tempImg);
    FreeImage_Unload(tempImg);

    imageSizeX = FreeImage_GetWidth(img);
    imageSizeY = FreeImage_GetHeight(img);
    image_bpp = FreeImage_GetBPP(img);
    image_nChannel = image_bpp / 8;
    cout << "X =" << imageSizeX << " Y = " << imageSizeY << " bpp =" << image_bpp << " nChannel =" << image_nChannel << endl;

    cout << "The QR code has size 123x123 pixels. Do you want to rescale the image to better fit the QR code? (y/n) ";
    char rescale;
    cin >> rescale;
    if (rescale == 'y' || rescale == 'Y') {
        cout << "New width: ";
        int width;
        cin >> width;
        cout << "New height: ";
        int height;
        cin >> height;
        FIBITMAP *temp = img;
        img = FreeImage_Rescale(temp, width, height, FILTER_BOX);
        FreeImage_Unload(temp);
        imageSizeX = FreeImage_GetWidth(img);
        imageSizeY = FreeImage_GetHeight(img);
        image_bpp = FreeImage_GetBPP(img);
        image_nChannel = image_bpp / 8;
        cout << "X =" << imageSizeX << " Y = " << imageSizeY << " bpp =" << image_bpp << " nChannel =" << image_nChannel << endl;
    }
    if (!image) {
        delete[] image;
    }
    image = FreeImage_GetBits(img);
    if (image == nullptr) {
        cout << "Null pointer in image.\n";
        exit(1);
    }

    FreeImage_Unload(img);
}
// remove old halftone and create new one
void cleanOutput () {
    if (!halftone) {
        delete[] halftone;
    }
    halftone = new BYTE[imageSizeX * imageSizeY * image_nChannel];
}

void saveImage(char *filename, BYTE *buf, int nChannel){
    RGBQUAD color;
    FIBITMAP *im = FreeImage_Allocate(imageSizeX, imageSizeY, 24);
    for (int i = 0; i < imageSizeY; i++) {
        for (int j = 0; j < imageSizeX; j++) {
            color.rgbRed = buf[(i * imageSizeX + j) * nChannel];
            color.rgbGreen = buf[(i * imageSizeX + j) * nChannel + 1];
            color.rgbBlue = buf[(i * imageSizeX + j) * nChannel + 2];
            FreeImage_SetPixelColor(im, j, i, &color);
        }
    }
    FreeImage_Save(FIF_BMP, im, filename, 0);
    FreeImage_Unload(im);
}

int main(int argc, char **argv) {
    // read in image
    FreeImage_Initialise();
    cout << "Object image file name: ";
    cin >> filename;
    loadImage(filename);
    cleanOutput();
    cout << "Which halftoning algorithm do you want to use? (1: Floyd-Steinberg; 2: dot diffusion) ";
    cin >> method;
    if (method != 1 && method != 2) {
        cout << "Default algorithm: Floyd-Steinberg." << endl;
        method = 1;
    }
//    strcpy(filename, "rescaled.bmp");
//    saveImage(filename, image, image_nChannel);
    // generate QR code encoding a text
    cout << "What do you want to encode in the QR code? ";
    cin.ignore();
    getline(cin, qrText);
    qrCode = QRcode_encodeString(qrText.c_str(), 6, QR_ECLEVEL_H, QR_MODE_8, 1);
    // halftone the generated QR code
    halftoneQR();
    // store the halftoned QR code into a .png file
    cout << "Output filename (without extension): ";
    cin >> qrFilename;
    char ext[5] = ".png";
    strcat(qrFilename, ext);
    writePNG(halftonedQRCode, qrFilename);
//    strcpy(filename, "halftone.bmp");
//    saveImage(filename, halftone, image_nChannel);
    QRcode_free(qrCode);
    delete[] halftonedQRCode->data;
    delete halftonedQRCode;
    // read the text from the .png file of the halftoned QR code
    cout << "Decoded information from halftone QR: ";
    int result = readQR(qrFilename);
    if (result) {
        cout << "Could not decode QR code." << endl;
    }
    return 0;
}