#include "main.h"

#include "kwConst.h"
#include "kwImage_object.h"
#include "kwReaderQR.h"

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <string>

// ---------------------------------------------------------------------------
//  Decoding helpers (shared by image mode and camera mode)
// ---------------------------------------------------------------------------

// Run the reader on one BGR frame. Returns true on a successful decode and
// writes the decoded payload into `out`. When `quiet` is true the decoder's
// information output (eye / version / mask lines) is silenced so it does not
// flood the terminal in the per-frame camera loop.
static bool decodeFrame(const cv::Mat& matBGR, bool quiet, std::string& out)
{
    cv::Mat matGray;
    cv::cvtColor(matBGR, matGray, cv::COLOR_BGR2GRAY);

    kwImageU8 imCorrected(matGray.rows, matGray.cols);
    memcpy(imCorrected.membase, matGray.data, imCorrected.numOfPixels * sizeof(uchar));

    std::streambuf* realCout = std::cout.rdbuf();
    std::ostringstream nullSink;
    if (quiet)
        std::cout.rdbuf(nullSink.rdbuf());

    kwReaderQR QR;
    unsigned char text[1000];
    bool ok = (QR.DoReading(imCorrected, text) == 1);
    if (ok)   // DoReading does not NUL-terminate; keep output to one line.
        out.assign((const char*)text);

    std::cout.rdbuf(realCout);

    if (ok) {
        size_t nl = out.find('\n');
        if (nl != std::string::npos)
            out.erase(nl);
    }
    return ok;
}

// Decode a single image file and print the result.
static int runImageMode(const std::string& path)
{
    cv::Mat matColor = cv::imread(path);
    if (matColor.empty()) {
        std::cerr << "ERROR: could not load image '" << path << "'." << std::endl;
        return 1;
    }

    std::string text;
    if (decodeFrame(matColor, false, text))
        std::cout << text << std::endl;
    else
        std::cerr << "No QR code decoded from '" << path << "'." << std::endl;
    return 0;
}

// Live camera mode: keep scanning frames; print a payload each time a QR is
// found. Press 'q' or ESC in the preview window to Quit.
static int runCameraMode(int deviceIndex, bool showWindow)
{
    cv::VideoCapture cam(deviceIndex);
    if (!cam.isOpened()) {
        std::cerr << "ERROR: could not open camera device " << deviceIndex << "." << std::endl;
        return 1;
    }

    std::cout << "Camera mode (device " << deviceIndex << "). Scanning... "
              << (showWindow ? "press 'q' or ESC to quit." : "press Ctrl-C to quit.") << std::endl;

    int cooldownFrames = 0;   // skip decoding for a moment after a detection
    while (true) {
        cv::Mat frame;
        if (!cam.read(frame) || frame.empty()) {
            std::cerr << "ERROR: failed to read camera frame." << std::endl;
            return 1;
        }

        std::string text;
        if (cooldownFrames > 0)
            --cooldownFrames;
        else if (decodeFrame(frame, true, text)) {
            std::cout << "DECODED: " << text << std::endl;
            cooldownFrames = 10;
        }

        if (showWindow) {
            cv::imshow("QR camera (q = quit)", frame);
            int key = cv::waitKey(1) & 0xFF;
            if (key == 'q' || key == 27)
                break;
        } else {
            cv::waitKey(20);   // pace captures without a window
        }
    }

    if (showWindow) {
        cam.release();
        cv::destroyAllWindows();
    }
    return 0;
}

static int printUsage(const char* prog)
{
    std::cout <<
        "Usage:\n"
        "  " << prog << " [options] [image]      Decode a QR code from an image file (default mode).\n"
        "  " << prog << " [options] -c [-d idx]   Scan a live camera.\n"
        "\n"
        "Options:\n"
        "  -c, --camera         Camera mode with the default device (0).\n"
        "  -d, --device <idx>   Camera device index; implies -c.\n"
        "  -n, --no-window      Camera mode without the preview window (Ctrl-C to quit).\n"
        "  -h, --help           Show this help and exit.\n"
        "\n"
        "Examples:\n"
        "  " << prog << " test_images/QR/V5.png\n"
        "  " << prog << " -c\n"
        "  " << prog << " -d 1 -n\n";
    return 0;
}

int main(int argc, char* argv[])
{
    bool        camera = false;
    bool        showWindow = true;
    int         deviceIndex = 0;
    std::string imagePath;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-c" || a == "--camera")
            camera = true;
        else if (a == "-d" || a == "--device") {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: " << a << " expects a device index." << std::endl;
                return 2;
            }
            deviceIndex = std::atoi(argv[++i]);
            camera = true;
        }
        else if (a == "-n" || a == "--no-window")
            showWindow = false;
        else if (a == "-h" || a == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        else if (!a.empty() && a[0] == '-') {
            std::cerr << "ERROR: unknown option '" << a << "'." << std::endl;
            return 2;
        }
        else
            imagePath = a;   // positional file argument
    }

    if (camera)
        return runCameraMode(deviceIndex, showWindow);

    if (imagePath.empty())
        imagePath = "test_images/QR/V5_H.png";   // sample kept for convenience
    return runImageMode(imagePath);
}
