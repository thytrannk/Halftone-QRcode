#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include <FreeImage.h>
#include <qrencode.h>

extern int imageSizeX, imageSizeY;
extern BYTE *image; // pointer to the image
extern BYTE *halftone; // pointer to the halftone image
extern int image_bpp, image_nChannel;
extern FIBITMAP *img;
extern char filename[100], qrFilename[100], qrText[100];
extern QRcode *qrCode, *halftonedQRCode;
extern int threshold; // initial threshold for floyd steinberg
extern int method;

#endif //PROJECT_PROJECT_H
