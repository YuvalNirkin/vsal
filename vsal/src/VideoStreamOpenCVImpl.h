#ifndef __VSAL_VIDEO_STREAM_OPENCV_IMPL__
#define __VSAL_VIDEO_STREAM_OPENCV_IMPL__

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
		\param fps Frames per second.
        */
        VideoStreamOpenCVImpl(int device, int frameWidth, int frameHeight,
			double fps = 0);

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

		/** Returns true if the video stream is from a live camera feed.
		*/
		bool isLive() const;

		/** Returns true if video stream has been initialized already.
		*/
		bool isOpened() const;

        /** Check whether a new frame was captured.
        */
        bool isUpdated();

        /** Get the last grabbed frame without copying.
        */
        cv::Mat getFrame();

        /** Get the last grabbed frame without copying as grayscale.
        */
        cv::Mat getFrameGrayscale();

		/** Get the index of the last grabbed frame.
		*/
		size_t getFrameIndex() const;

		/** 0-based index of the frame to be decoded/captured next.
		*/
		void seek(size_t index);

		/**	Total number of frames or the number of frames read in case of a live feed.
		*/
		size_t size() const;

    private:
        int mDevice;
        int mRequestedWidth, mRequestedHeight;
        std::string mVideoFile;
        cv::Mat mFrame;
        cv::VideoCapture mCap;
        double mFPS;
        double mTimestamp;
		size_t mFrameIndex;
		size_t mTotalFrames;
    };

}	// namespace vsal


#endif	// __VSAL_VIDEO_STREAM_OPENCV_IMPL__