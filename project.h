#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include <FreeImage.h>
#include <qrencode.h>
#include <string>

using namespace std;

extern int imageSizeX, imageSizeY;
extern BYTE *image; // pointer to the image
extern BYTE *halftone; // pointer to the halftone image
extern int image_bpp, image_nChannel;
extern FIBITMAP *img;
extern char filename[100], qrFilename[100];
extern string qrText;
extern QRcode *qrCode, *halftonedQRCode;
extern int threshold; // initial threshold for floyd steinberg
extern int method;

#endif //PROJECT_PROJECT_H
