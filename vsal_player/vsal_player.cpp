/************************************************************************************
*									   Includes										*
************************************************************************************/

// vsal
#include <vsal/VideoStreamFactory.h>
#include <vsal/VideoStreamOpenCV.h>
#include <vsal/Viewer.h>

// std
#include <iostream>
#include <string>
#include <exception>
#include <memory>   // unique_ptr

// Boost
#include <boost/format.hpp>
#include <boost/program_options.hpp>

// OpenCV
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
using std::runtime_error;
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
    float fps, scale;
    bool draw_ind;
    try {
        options_description desc("Allowed options");
        desc.add_options()
            ("help", "display the help message")
            ("input,i", value<string>(&inputPath)->required(), "path to video sequence")
            ("device,d", value<int>(&device)->default_value(-1),
                "device id")
            ("width,w", value<unsigned int>(&width)->default_value(0),
                "frame width")
            ("height,h", value<unsigned int>(&height)->default_value(0),
                "frame height")
            ("fps,f", value<float>(&fps)->default_value(0.0f), "frames per second")
            ("scale,s", value<float>(&scale)->default_value(1.0f), "frame scale multiplier")
            ;
        variables_map vm;
        store(command_line_parser(argc, argv).options(desc).
            positional(positional_options_description().add("input", -1)).run(), vm);
        if (vm.count("help")) {
            cout << "Usage: cache_face_landmarks [options]" << endl;
            cout << desc << endl;
            exit(0);
        }
        notify(vm);
    }
    catch (const error& e) {
        cout << "Error while parsing command-line arguments: " << e.what() << endl;
        cout << "Use --help to display a list of options." << endl;
        exit(1);
    }

    try
	{
        // Create video source
        vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
        std::unique_ptr<vsal::VideoStreamOpenCV> vs(
            (vsal::VideoStreamOpenCV*)vsf.create(inputPath, device, width, height));
        if (vs == nullptr) throw runtime_error("No video source specified!");

		// Initialize viewer
        vsal::Viewer viewer("vsal_player", vs.get(), false, true, fps, scale);
		viewer.run();
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}
