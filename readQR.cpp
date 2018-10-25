#include <iostream>
#include <fstream>
#include <string>
#include "readQR.h"
#include <zxing/common/Counted.h>
#include <zxing/Binarizer.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/Result.h>
#include <zxing/ReaderException.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <exception>
#include <zxing/Exception.h>
#include <zxing/common/IllegalArgumentException.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/DecodeHints.h>

#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/multi/qrcode/QRCodeMultiReader.h>
#include <zxing/multi/ByQuadrantReader.h>
#include <zxing/multi/MultipleBarcodeReader.h>
#include <zxing/multi/GenericMultipleBarcodeReader.h>

#include "lib/zxing/ImageReaderSource.h"
#include "readQR.h"

using namespace std;
using namespace zxing;
using namespace zxing::multi;
using namespace zxing::qrcode;

vector<Ref<Result> > decode(Ref<BinaryBitmap> image, DecodeHints hints) {
    Ref<Reader> reader(new MultiFormatReader);
    return vector<Ref<Result> >(1, reader->decode(image, hints));
}

vector<Ref<Result> > decode_multi(Ref<BinaryBitmap> image, DecodeHints hints) {
    MultiFormatReader delegate;
    GenericMultipleBarcodeReader reader(delegate);
    return reader.decodeMultiple(image, hints);
}

int read_image(Ref<LuminanceSource> source, bool hybrid, string expected) {
    vector<Ref<Result> > results;
    string cell_result;
    int res = -1;

    try {
        Ref<Binarizer> binarizer;
        binarizer = new GlobalHistogramBinarizer(source);
        DecodeHints hints(DecodeHints::DEFAULT_HINT);
//        hints.setTryHarder(try_harder);
        Ref<BinaryBitmap> binary(new BinaryBitmap(binarizer));
        results = decode(binary, hints);
        res = 0;
    } catch (const ReaderException& e) {
        cell_result = "zxing::ReaderException: " + string(e.what());
        res = -2;
    } catch (const zxing::IllegalArgumentException& e) {
        cell_result = "zxing::IllegalArgumentException: " + string(e.what());
        res = -3;
    } catch (const zxing::Exception& e) {
        cell_result = "zxing::Exception: " + string(e.what());
        res = -4;
    } catch (const std::exception& e) {
        cell_result = "std::exception: " + string(e.what());
        res = -5;
    }

    for (size_t i = 0; i < results.size(); i++) {
        cout << results[i]->getText()->getText() << endl;
    }

    return res;
}

string read_expected(string imagefilename) {
    string textfilename = imagefilename;
    string::size_type dotpos = textfilename.rfind(".");

    textfilename.replace(dotpos + 1, textfilename.length() - dotpos - 1, "txt");
    ifstream textfile(textfilename.c_str(), ios::binary);
    textfilename.replace(dotpos + 1, textfilename.length() - dotpos - 1, "bin");
    ifstream binfile(textfilename.c_str(), ios::binary);
    ifstream *file = 0;
    if (textfile.is_open()) {
        file = &textfile;
    } else if (binfile.is_open()) {
        file = &binfile;
    } else {
        return std::string();
    }
    file->seekg(0, ios_base::end);
    size_t size = size_t(file->tellg());
    file->seekg(0, ios_base::beg);

    if (size == 0) {
        return std::string();
    }

    char* data = new char[size + 1];
    file->read(data, size);
    data[size] = '\0';
    string expected(data);
    delete[] data;

    return expected;
}

int readQR (char *filename) {
    Ref<LuminanceSource> source;
    try {
    source = ImageReaderSource::create(filename);
    } catch (const zxing::IllegalArgumentException &e) {
    cerr << e.what() << " (ignoring)" << endl;
    }

    string expected = read_expected(filename);
    int result = read_image(source, false, expected);
    return result;
}