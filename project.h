#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include <FreeImage.h>
#include <qrencode.h>

extern int imageSizeX, imageSizeY, impSizeX, impSizeY, qrSizeX, qrSizeY;
extern BYTE *image, *imp_map, *qr_image; // pointer to the image
extern BYTE *halftone; // pointer to the halftone image
extern int image_bpp, image_nChannel, imp_bpp, imp_nChannel, qr_bpp, qr_nChannel;
extern FIBITMAP *img, *imp, *qr_img;
extern char filename[100], qrFilename[100], qrText[100];
extern QRcode *qrCode, *halftonedQRCode;
extern int threshold; // initial threshold for floyd steinberg

#endif //PROJECT_PROJECT_H
