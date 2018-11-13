/*
 * Convert a normal QR code to a halftoned QR code
 */

#include <iostream>
#include "halftoneQR.h"

enum position {
    topleft,
    topmid,
    topright,
    centerleft,
    centermid,
    centerright,
    botleft,
    botmid,
    botright,
};

static void originalCoor2Coor (int originalX, int originalY, enum position position1, int &x, int &y) {
    switch (position1) {
        case topleft:
            x = originalX * 3;
            y = originalY * 3;
            break;
        case topmid:
            x = originalX * 3 + 1;
            y = originalY * 3;
            break;
        case topright:
            x = originalX * 3 + 2;
            y = originalY * 3;
        case centerleft:
            x = originalX * 3;
            y = originalY * 3 + 1;
        case centermid:
            x = originalX * 3 + 1;
            y = originalY * 3 + 1;
        case centerright:
            x = originalX * 3 + 2;
            y = originalY * 3 + 1;
        case botleft:
            x = originalX * 3;
            y = originalY * 3 + 2;
        case botmid:
            x = originalX * 3 + 1;
            y = originalY * 3 + 2;
        case botright:
            x = originalX * 3 + 2;
            y = originalY * 3 + 2;
    }
}

QRcode *halftoneQR(QRcode *qrCode, GLubyte *image) {
    int originalWidth = qrCode->width;
    int newWidth = originalWidth;
    auto *newQR = new QRcode;
    newQR->version = qrCode->version;
    newQR->width = newWidth;
    newQR->data = new unsigned char[newWidth*newWidth];

    for (int y = 0; y < originalWidth; y++) {
        for (int x = 0; x < originalWidth; x++) {
            int newX, newY;
//            originalCoor2Coor(x, y, topleft, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, topmid, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, topright, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, centerleft, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, centermid, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, centerright, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, botleft, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, botmid, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, botright, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
//            originalCoor2Coor(x, y, topleft, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
//            originalCoor2Coor(x, y, topmid, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
//            originalCoor2Coor(x, y, topright, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
//            originalCoor2Coor(x, y, centerleft, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
//            originalCoor2Coor(x, y, centermid, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
//            originalCoor2Coor(x, y, centerright, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
//            originalCoor2Coor(x, y, botleft, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
//            originalCoor2Coor(x, y, botmid, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
//            originalCoor2Coor(x, y, botright, newX, newY);
//            newQR->data[newY * newWidth + newX] = qrCode->data[10];
            if ((qrCode->data[y * originalWidth + x] & (11 << 1))) {
//                newQR->data[y * newWidth + x] = qrCode->data[y * newWidth + x] | 1; // turn to black
//                newQR->data[y * newWidth + x] = qrCode->data[y * newWidth + x] & 11111110; // turn to white
                newQR->data[y * newWidth + x] = qrCode->data[y * newWidth + x];
            }
        }
    }
    return newQR;
}