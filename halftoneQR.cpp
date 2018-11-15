/*
 * Convert a normal QR code to a halftoned QR code
 */

#include <iostream>
#include "halftoneQR.h"
#include "project.h"

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

int startX_img, startY_img, startX_qr, startY_qr; // start location of mapping for image and qr code
int x, y; // width and height of mapping area

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
            break;
        case centerleft:
            x = originalX * 3;
            y = originalY * 3 + 1;
            break;
        case centermid:
            x = originalX * 3 + 1;
            y = originalY * 3 + 1;
            break;
        case centerright:
            x = originalX * 3 + 2;
            y = originalY * 3 + 1;
            break;
        case botleft:
            x = originalX * 3;
            y = originalY * 3 + 2;
            break;
        case botmid:
            x = originalX * 3 + 1;
            y = originalY * 3 + 2;
            break;
        case botright:
            x = originalX * 3 + 2;
            y = originalY * 3 + 2;
            break;
    }
}

void QRtimes3(void) {
    int originalWidth = qrCode->width;
    int newWidth = originalWidth * 3;
    halftonedQRCode = new QRcode;
    halftonedQRCode->version = qrCode->version;
    halftonedQRCode->width = newWidth;
    halftonedQRCode->data = new unsigned char[newWidth*newWidth];

    for (int y = 0; y < originalWidth; y++) {
        for (int x = 0; x < originalWidth; x++) {
            int newX, newY;
            originalCoor2Coor(x, y, topleft, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
            originalCoor2Coor(x, y, topmid, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
            originalCoor2Coor(x, y, topright, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
            originalCoor2Coor(x, y, centerleft, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
            originalCoor2Coor(x, y, centermid, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
            originalCoor2Coor(x, y, centerright, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
            originalCoor2Coor(x, y, botleft, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
            originalCoor2Coor(x, y, botmid, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];
            originalCoor2Coor(x, y, botright, newX, newY);
            halftonedQRCode->data[newY * newWidth + newX] = qrCode->data[y * originalWidth + x];

//            if ((qrCode->data[y * originalWidth + x] & (11 << 1))) {
////                newQR->data[y * newWidth + x] = qrCode->data[y * newWidth + x] | 1; // turn to black
////                newQR->data[y * newWidth + x] = qrCode->data[y * newWidth + x] & 11111110; // turn to white
//                halftonedQRCode->data[y * newWidth + x] = qrCode->data[y * newWidth + x];
//            }
        }
    }

//    for (int y = 0; y < newWidth; y++) {
//        for (int x = 0; x < newWidth; x++) {
//            halftonedQRCode->data[y * newWidth + x] = qrCode->data[((int) (y/3)) * originalWidth + ((int) (x/3))];
//        }
//    }
}

void calcStart(void) {
    // calculate alignment between qr code and image

    if (imageSizeX < halftonedQRCode->width) {
        startX_img = 0;
        startX_qr = (halftonedQRCode->width - imageSizeX) / 2;
        x = imageSizeX;
    } else {
        startX_qr = 0;
        startX_img = (imageSizeX - halftonedQRCode->width) / 2;
        x = halftonedQRCode->width;
    }
    if (imageSizeY < halftonedQRCode->width) {
        startY_img = 0;
        startY_qr = (halftonedQRCode->width - imageSizeY) / 2;
        y = imageSizeY;
    } else {
        startY_qr = 0;
        startY_img = (imageSizeY - halftonedQRCode->width) / 2;
        y = halftonedQRCode->width;
    }
}

void *halftoneQR(void) {

    // create newQR as QR with module subdivided
    QRtimes3();
    // calculate alignment between qr code and image
    calcStart();


}