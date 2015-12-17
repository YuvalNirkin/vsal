#ifndef __OG_VIDEO_STREAM_OPENCV_IMPL__
#define __OG_VIDEO_STREAM_OPENCV_IMPL__

/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "vsal/VideoStreamOpenCV.h"
#include <opencv2/highgui.hpp>

/************************************************************************************
*									 Declarations									*
************************************************************************************/

/************************************************************************************
*										Classes										*
************************************************************************************/

namespace vsal
{
    /** Provide a single video stream from camera or video file with OpenCV interface.
    */
    class VideoStreamOpenCVImpl : public VideoStreamOpenCV
    {
    public:

        /** Create video stream from a camera device id.
        \param device Device's id
        \param frameWidth The width in pixels for each of the incoming frames
        \param frameHeight The height in pixels for each of the incoming frames
        */
        VideoStreamOpenCVImpl(int device, int frameWidth, int frameHeight);

        /** Create video stream from a video file.
        \param filePath Path to the video file
        */
        VideoStreamOpenCVImpl(const std::string& filePath);

        /** Destructor.
        */
        ~VideoStreamOpenCVImpl();

        /** Open the video stream.
        */
        bool open();

        /**	Close the video stream.
        */
        void close();

        /** Read frame.
        */
        bool read();

        /** Get the frame's width.
        */
        int getWidth() const;

        /** Get the frame's height.
        */
        int getHeight() const;

        /** Get the timestamp in seconds of the last grabbed frame since the start
        of the stream.
        */
        double getTimestamp() const;

        /** Get frames per second.
        */
        double getFPS() const;

        /** Get the data of the last grabbed frame.
        */
        void getFrameData(unsigned char* data) const;

        /** Check whether a new frame was captured.
        */
        bool isUpdated();

        /** Get the last grabbed frame without copying.
        */
        cv::Mat getFrame();

        /** Get the last grabbed frame without copying as grayscale.
        */
        cv::Mat getFrameGrayscale();

    private:
        int mDevice;
        int mRequestedWidth, mRequestedHeight;
        std::string mVideoFile;
        cv::Mat mFrame;
        cv::VideoCapture mCap;
        double mFPS;
        double mTimestamp;
    };

}	// namespace vsal


#endif	// __OG_VIDEO_STREAM_OPENCV_IMPL__