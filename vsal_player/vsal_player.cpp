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
    try
	{
        // Create video source
        vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
        std::unique_ptr<vsal::VideoStreamOpenCV> vs(
            (vsal::VideoStreamOpenCV*)vsf.create(argc, argv));
        if (vs == nullptr) throw runtime_error("No video source specified!");

		// Initialize viewer
        vsal::Viewer viewer("vsal_player", vs.get(), false, true);
		viewer.run();
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}
