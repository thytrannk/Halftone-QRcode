# Halftone QR code

This code base implements part of the algorithm by Chu et al. [1] that generates QR codes [2] with embedded images. QR
codes with embedded images are very useful in marketing: they can be understood both by humans (as images) and by
computers (as QR codes).

## Results

Below are some of the QR codes with embedded images generated using this implementation. All of these QR codes could be
decoded using both ZXing library [3] (using .png images as input) and a QR code reader application on Android [4]
(scanned under normal room lighting).

![result image](img/results.png?raw=true)

## Program description

* Input: an image (.jpg, .png, or .bmp), which can be either colored or greyscale, but the program will automatically
convert it to greyscale
* The user has an option of rescaling the image so that it fits the QR code better.
* The user can choose which algorithm to embed the image into the QR code (Floyd-Steinberg error diffusion [5] and
dot diffusion [6] algorithms).
* The user then inputs the text that will be encoded in the QR code.
* The program will output the QR code with the image embedded.

### Example
Text encoded by the QR code: "This is a picture of Donald Duck."

Image embedded: a rescaled version of the image file donald_duck.jpg


    Object image file name: donald_duck.jpg
    X = 350; Y = 262; bpp = 32; nChannel = 4
    The QR code has size 123x123 pixels. Do you want to rescale the image to better fit the QR code? (y/n) y
    New width: 175
    New height: 131
    X = 175; Y = 131; bpp = 32; nChannel = 4
    Which halftoning algorithm do you want to use? (1: Floyd-Steinberg; 2: dot diffusion) 2
    What do you want to encode in the QR code? This is a picture of Donald Duck.
    Output filename (without extension): outputQR
    Decoded information from halftone QR: This is a picture of Donald Duck.
    
## Development requirements

* [FreeImage 3.18.0](http://freeimage.sourceforge.net)
* [Libqrencode](https://github.com/fukuchi/libqrencode)
* [ZXing](https://github.com/zxing/zxing)

## References

[1] H.-K. Chu, C.-S. Chang, R.-R. Lee, and N. J. Mitra. Halftone qr codes. ACM Trans. Graph., 32(6):217:1−217:8, Nov. 2013.
    ISSN 0730−0301. doi: 10.1145/2508363.2508408. URL http://doi.acm.org/10.1145/2508363.2508408.

[2] Denso Wave Incorporated. Qrcode.com. URL http://www.qrcode.com/en/.

[3] S. Owen, D. Switkin, and Z. Team. Zxing, 2008. URL https://github.com/zxing/zxing.

[4] Sustainable App Developer. Qr code reader, 2018. URL https://play.google.com/store/apps/details?id=com.qrcodereaderapp.

[5] R.W. Floyd, L. Steinberg. An adaptive algorithm for spatial grey scale. Proceedings of the Society of Information
Display 17, 75–77, 1976.

[6] D. E. Knuth. Digital halftones by dot diffusion. ACM Trans. Graph., 6(4):245–273, Oct. 1987. ISSN 0730-0301.
doi: 10.1145/35039.35040. URL http://doi.acm.org/10.1145/35039.35040.
