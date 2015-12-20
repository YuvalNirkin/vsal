/************************************************************************************
*									   Includes										*
************************************************************************************/
#include <vsal/VideoStreamFactory.h>
#include <vsal/VideoStreamOpenCV.h>
#include <iostream>
#include <string>
#include <exception>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

/************************************************************************************
*									  Namespaces									*
************************************************************************************/
using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::exception;
using namespace boost::program_options;

/************************************************************************************
*									Implementation									*
************************************************************************************/
int main(int argc, char** argv)
{
    // Parse command line arguments
    string inputPath;
    int device;
    unsigned int width, height;
    double fps;
    try {
        options_description desc("Allowed options");
        desc.add_options()
            ("help", "display the help message")
            ("input,i", value<string>(&inputPath), "input path")
            ("device,d", value<int>(&device)->default_value(-1),
            "device id")
            ("width,w", value<unsigned int>(&width)->default_value(640),
            "frame width")
            ("height,h", value<unsigned int>(&height)->default_value(480),
            "frame height")
            ("fps,f", value<double>(&fps)->default_value(30.0),
            "frames per second")
            ;
        variables_map vm;
        store(command_line_parser(argc, argv).options(desc).
            positional(positional_options_description().add("input", -1)).run(), vm);
        if (vm.count("help")) {
            cout << "Usage: preview [options]" << endl;
            cout << desc << endl;
            exit(0);
        }
        notify(vm);
    }
    catch (const error& e) {
        cout << "Error while parsing command-line arguments: " << e.what() << endl;
        cout << "Use --help to display a list of options." << endl;
        exit(0);
    }

    try
    {
        bool live = device >= 0;

        // Create video source
        vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
        vsal::VideoStreamOpenCV* vs = nullptr;
        if (live) vs = (vsal::VideoStreamOpenCV*)vsf.create(device, width, height);
        else if (!inputPath.empty()) vs = (vsal::VideoStreamOpenCV*)vsf.create(inputPath);
        else throw exception("No video source specified!");

        // Open video source
        vs->open();
        width = vs->getWidth();
        height = vs->getHeight();
        fps = vs->getFPS();

        // Report video parameters
        cout << "Video parameters:" << endl;
        cout << "width = " << width << endl;
        cout << "height = " << height << endl;
        cout << "fps = " << fps << endl;

        // Preview loop
        cv::Mat frame;
        int frameCounter = 0;
        while (vs->read())
        {
            if (!vs->isUpdated()) continue;

            frame = vs->getFrame();
            
            // Show overlay
            cv::putText(frame, (boost::format("Frame count: %d") % ++frameCounter).str(), cv::Point(15, 15),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255),
                1, CV_AA);
            cv::putText(frame, "press any key to stop", cv::Point(10, frame.rows - 20),
                cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 255),
                1, CV_AA);    

            // Show frame
            cv::imshow("frame", frame);
            int key = cv::waitKey(1);
            if (key >= 0) break;
        }

        // Cleanup
        vs->close();
        if (vs != nullptr) delete vs;
        vs = nullptr;
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}