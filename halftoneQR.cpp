/*
 * Convert a normal QR code to a halftoned QR code
 */

#include <iostream>
#include "halftoneQR.h"
#include "project.h"
#include "halftone.h"

void QRtimes3(void) {
    int originalWidth = qrCode->width;
    int newWidth = originalWidth * 3;
    halftonedQRCode = new QRcode;
    halftonedQRCode->version = qrCode->version;
    halftonedQRCode->width = newWidth;
    halftonedQRCode->data = new unsigned char[newWidth*newWidth];

    for (int y = 0; y < newWidth; y++) {
        for (int x = 0; x < newWidth; x++) {
            halftonedQRCode->data[y * newWidth + x] = qrCode->data[((int) (y/3)) * originalWidth + ((int) (x/3))];
        }
    }
}

void calcStart(int &startX_img, int &startY_img, int &startX_qr, int &startY_qr, int &x, int &y) {
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
        startY_img = imageSizeY;
        startY_qr = (halftonedQRCode->width - imageSizeY) / 2;
        y = imageSizeY;
    } else {
        startY_qr = 0;
        startY_img = imageSizeY - ((imageSizeY - halftonedQRCode->width) / 2);
        y = halftonedQRCode->width;
    }
}

void *halftoneQR(void) {

    // create newQR as QR with module subdivided
    QRtimes3();
    // calculate alignment between qr code and image
    int startX_img, startY_img, startX_qr, startY_qr; // start location of mapping for image and qr code
    int x, y; // width and height of mapping area
    calcStart(startX_img, startY_img, startX_qr, startY_qr, x, y);
    // compute the halftone image
    if (method == 2) {
        dotDiffusion();
    } else {
        floyd();
    }
    for (int j = 0; j < y; j++) {
        for (int i = 0; i < x; i++) {
            int x_img = startX_img + i;
            int y_img = startY_img - j;
            int x_qr = startX_qr + i;
            int y_qr = startY_qr + j;
            int width_qr = halftonedQRCode->width;
            if (halftonedQRCode->data[y_qr * width_qr + x_qr] >> 7) {
                continue;
            }
            if ((x_qr % 3 == 1) && (y_qr % 3 == 1)) {
                // center submodule
                continue;
            }
            if (!halftone[(y_img * imageSizeX + x_img) * image_nChannel]) {
                // halftone bit is black
//                halftonedQRCode->data[y_qr * width_qr + x_qr] =
//                        (unsigned char) (((halftonedQRCode->data[y_qr * width_qr + x_qr] >> 1) << 1) + 1);
                halftonedQRCode->data[y_qr * width_qr + x_qr] =
                        (unsigned char) (halftonedQRCode->data[y_qr * width_qr + x_qr] | 1);
            } else {
//                halftonedQRCode->data[y_qr * width_qr + x_qr] =
//                        (halftonedQRCode->data[y_qr * width_qr + x_qr] >> 1) << 1;
                halftonedQRCode->data[y_qr * width_qr + x_qr] =
                        (unsigned char) (halftonedQRCode->data[y_qr * width_qr + x_qr] & 11111110);
            }
        }
    }
}
