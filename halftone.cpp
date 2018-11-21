#include "halftone.h"
#include "project.h"

void dither ()
{
    int i, j;
    int r,g,b;
    int p,q;
    int m[4][4] ={{1, 7, 10,16},
                  {12,14,3,5},
                  {8,2,15,9},
                  {13,11,6,4}};
    for (i = 0; i < imageSizeY; i++) {
        for (j = 0; j < imageSizeX; j++) {
            r = (int) image[(i * imageSizeX + j) * image_nChannel];            //red
            g = (int) image[(i * imageSizeX + j) * image_nChannel + 1];        //green
            b = (int) image[(i * imageSizeX + j) * image_nChannel + 2];        //blue
            p = i % 4;
            q = j % 4;
            r = (int) ((float) r / 255.0 * 16);
            g = (int) ((float) g / 255.0 * 16);
            b = (int) ((float) b / 255.0 * 16);
            if (r < m[q][p]) r = 0;
            else r = 255;
            if (g < m[q][p]) g = 0;
            else g = 255;
            if (b < m[q][p]) b = 0;
            else b = 255;
            halftone[(i * imageSizeX + j) * image_nChannel] = (BYTE) r;        //red
            halftone[(i * imageSizeX + j) * image_nChannel + 1] = (BYTE) g;        //green
            halftone[(i * imageSizeX + j) * image_nChannel + 2] = (BYTE) b;        //blue
        }
    }
}

void floyd()
{
#define ALPHA 0.4375
#define BETA 0.1875
#define GAMMA 0.3125
#define DELTA 0.0625
// add code in here

    int x, y, i;
    int r,g,b;
    float raw_r, raw_g, raw_b;
    float error_r, error_g, error_b;

    float *image_new = new float[imageSizeX * imageSizeY * image_nChannel];

    for (i = 0; i < imageSizeX * imageSizeY * image_nChannel; i++) {
        image_new[i] = (float) image[i];
    }

    for(y = imageSizeY - 1; y >= 0; y--) {
        // y is y-coordinate; small y is at the bottom of the image
        for (x = 0; x < imageSizeX; x++) {
            // x is x-coordinate
            int current, alpha_nb, beta_nb, gamma_nb, delta_nb;
            // Location of current pixel and neighboring pixels
            current = y * imageSizeX + x;
            alpha_nb = y * imageSizeX + x + 1;
            beta_nb = (y - 1) * imageSizeX + x - 1;
            gamma_nb = (y - 1) * imageSizeX + x;
            delta_nb = (y - 1) * imageSizeX + x + 1;

            raw_r = image_new[current * image_nChannel];            //red
            raw_g = image_new[current * image_nChannel + 1];        //green
            raw_b = image_new[current * image_nChannel + 2];        //blue

            if (raw_r < threshold) {
                r = 0;
            } else {
                r = 255;
            }
            if (raw_g < threshold) {
                g = 0;
            } else {
                g = 255;
            }
            if (raw_b < threshold) {
                b = 0;
            } else {
                b = 255;
            }

            error_r = raw_r - r;
            error_g = raw_g - g;
            error_b = raw_b - b;

            if (x + 1 < imageSizeX) {
                image_new[alpha_nb * image_nChannel] += ALPHA * error_r;
                image_new[alpha_nb * image_nChannel + 1] += ALPHA * error_g;
                image_new[alpha_nb * image_nChannel + 2] += ALPHA * error_b;
            }
            if (y - 1 >= 0 && x > 0) {
                image_new[beta_nb * image_nChannel] += BETA * error_r;
                image_new[beta_nb * image_nChannel + 1] += BETA * error_g;
                image_new[beta_nb * image_nChannel + 2] += BETA * error_b;
            }
            if (y - 1 >= 0) {
                image_new[gamma_nb * image_nChannel] += GAMMA * error_r;
                image_new[gamma_nb * image_nChannel + 1] += GAMMA * error_g;
                image_new[gamma_nb * image_nChannel + 2] += GAMMA * error_b;
            }
            if (y - 1 >= 0 && x + 1 < imageSizeX) {
                image_new[delta_nb * image_nChannel] += DELTA * error_r;
                image_new[delta_nb * image_nChannel + 1] += DELTA * error_g;
                image_new[delta_nb * image_nChannel + 2] += DELTA * error_b;
            }

            halftone[current * image_nChannel] = (BYTE) r;        //red
            halftone[current * image_nChannel + 1] = (BYTE) g;        //green
            halftone[current * image_nChannel + 2] = (BYTE) b;        //blue
        }
    }

    delete[] image_new;
}