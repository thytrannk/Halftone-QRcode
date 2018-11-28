#include "halftone.h"
#include "project.h"

void floyd() {
#define ALPHA 0.4375
#define BETA 0.1875
#define GAMMA 0.3125
#define DELTA 0.0625

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
            halftone[current * image_nChannel + 3] = (BYTE) 1;     // alpha
        }
    }

    delete[] image_new;
}

int  weight(int x, int y) {
    int w = 0;
    w = 3 - x*x - y*y;
    return w;
}
bool  isValidNeighbour(int x, int y) {
    bool t = false;
    if (x >= 0 && x < imageSizeX && y >= 0 && y < imageSizeY) {
        t = true;
    }
    return t;
}
int classRank(int x, int y) {
    int i, j, r;
    int m1[8][8]=//M1
            {{0,32,8,40,2,34,10,42},
             {48,16,56,24,50,18,58,26},
             {12,44,4,36,14,46,6,38},
             {60,28,52,20,62,30,54,22},
             {3,35,11,43,1,33,9,41},
             {51,19,59,27,49,17,57,25},
             {15,47,7,39,13,45,5,37},
             {63,31,55,23,61,29,53,21}};

//    int m2[8][8] =// M2
//            {{34,48,40,32,29,15,23,31},
//             {42,58,56,53,21, 5, 7,10},
//             {50,62,61,45,13, 1, 2,18},
//             {38,46,54,37,25,17, 9,26},
//             {28,14,22,30,35,49,41,33},
//             {20,4,  6,11,43,59,57,52},
//             {12,0,  3,19,51,63,60,44},
//             {24,16, 8,27,39,47,55,36}};

    // The top row has y == 255, and the bottom row has y == 0
    // Thus, the maintain the orientation, we have to convert the y-coordinate to the row number by calculating imageSizeY-1-y
    i =  x%8;
    j  = (imageSizeY-1-y)%8;
    r = m1[j][i];

    return r;
}

void accumNeighbour(int u, int v, int i, int j, int &w) {
    if (classRank(u, v) > classRank(i, j)) {
        // The top row has y == 255, and the bottom row has y == 0
        // Thus, the maintain the orientation, we have to convert the y-coordinate to the row number by calculating imageSizeY-1-y
        // As a result, v - j has to be switched to j - v (because the y-axis is upside-down)
        w += weight(u-i, j-v);
    }
}

void diffuseNeighbour(float *image_new, int u, int v, int i, int j, float error_r, float error_g, float error_b, int w) {
    // The top row has y == 255, and the bottom row has y == 0
    // Thus, the maintain the orientation, we have to convert the y-coordinate to the row number by calculating imageSizeY-1-y
    // As a result, v - j has to be switched to j - v (because the y-axis is upside-down)
    image_new[(v * imageSizeX + u) * image_nChannel] += error_r * weight(u - i, j - v) / w;
    image_new[(v * imageSizeX + u) * image_nChannel + 1] += error_g * weight(u - i, j - v) / w;
    image_new[(v * imageSizeX + u) * image_nChannel + 2] += error_b * weight(u - i, j - v) / w;
}

void dotDiffusion() {
#define NEIGHBOURHOODSIZE 1 // other values do not seem to work well

    int x, y, i, k;
    int r,g,b;
    int w;
    float raw_r, raw_g, raw_b;
    float error_r, error_g, error_b;

    float *image_new = new float[imageSizeX*imageSizeY * image_nChannel];

    for (i = 0; i < imageSizeX * imageSizeY * image_nChannel; i++) {
        image_new[i] = (float) image[i];
    }

    for(k = 0; k < 64; k++) {
        for(y = imageSizeY - 1; y >= 0; y--) {
            // y is y-coordinate; small y is at the bottom of the image
            for (x = 0; x < imageSizeX; x++) {
                //x is x-coordinate
                if (classRank(x, y) == k) {
                    raw_r = image_new[(y * imageSizeX + x) * image_nChannel];            //red
                    raw_g = image_new[(y * imageSizeX + x) * image_nChannel + 1];        //green
                    raw_b = image_new[(y * imageSizeX + x) * image_nChannel + 2];        //blue

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

                    w = 0;

                    for (int u = x - NEIGHBOURHOODSIZE; u <= x + NEIGHBOURHOODSIZE; u++) {
                        for (int v = y - NEIGHBOURHOODSIZE; v <= y + NEIGHBOURHOODSIZE; v++) {
                            // if (u,v) is not (x,y) and (u,v) is within image
                            if ((u != x || v != y) && isValidNeighbour(u, v)) {
                                accumNeighbour(u, v, x, y, w);
                            }
                        }
                    }

                    if (w > 0) {
                        for (int u = x - NEIGHBOURHOODSIZE; u <= x + NEIGHBOURHOODSIZE; u++) {
                            for (int v = y - NEIGHBOURHOODSIZE; v <= y + NEIGHBOURHOODSIZE; v++) {
                                // if (u,v) is not (x,y) and (u,v) is within image
                                if ((u != x || v != y) && isValidNeighbour(u, v)) {
                                    diffuseNeighbour(image_new, u, v, x, y, error_r, error_g, error_b, w);
                                }
                            }
                        }
                    }

                    halftone[(y * imageSizeX + x) * image_nChannel] = (BYTE) r;        //red
                    halftone[(y * imageSizeX + x) * image_nChannel + 1] = (BYTE) g;        //green
                    halftone[(y * imageSizeX + x) * image_nChannel + 2] = (BYTE) b;        //blue
                    halftone[(y * imageSizeX + x) * image_nChannel + 3] = (BYTE) 1;    //alpha
                }
            }
        }
    }

    delete[] image_new;
}