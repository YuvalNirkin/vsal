/************************************************************************************
*									   Includes										*
************************************************************************************/
#include <vsal/VideoStreamFactory.h>
#include <vsal/VideoStreamOpenCV.h>
#include <iostream>
#include <string>
#include <exception>
#include <memory>   // unique_ptr
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
    try
    {
        // Create video source
        vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
        std::unique_ptr<vsal::VideoStreamOpenCV> vs(
            (vsal::VideoStreamOpenCV*)vsf.create(argc, argv));
        if (vs == nullptr) throw exception("No video source specified!");

        // Open video source
        if (!vs->open()) throw exception("Failed to open video source!");
        unsigned int width = vs->getWidth();
        unsigned int height = vs->getHeight();

        // Report video parameters
        cout << "Video parameters:" << endl;
        cout << "width = " << width << endl;
        cout << "height = " << height << endl;

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
            cv::putText(frame, (boost::format("FPS: %f") % vs->getFPS()).str(), cv::Point(15, 35),
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
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}