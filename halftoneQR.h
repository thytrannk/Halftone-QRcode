#ifndef PROJECT_HALFTONED_QR_H
#define PROJECT_HALFTONED_QR_H

#include <qrencode.h>
#include <FreeImage.h>

QRcode *halftoneQR(QRcode *qrCode, BYTE *image, BYTE *imp_map, int imageSizeX, int imageSizeY);

#endif //PROJECT_HALFTONED_QR_H
