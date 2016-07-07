#ifndef __OG_VIDEO_STREAM_IMAGES__
#define __OG_VIDEO_STREAM_IMAGES__

/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "vsal/VideoStreamOpenCV.h"
#include <vector>

/************************************************************************************
*									 Declarations									*
************************************************************************************/

/************************************************************************************
*										Classes										*
************************************************************************************/

namespace vsal
{
    /** Provide a single video stream from a set of images in a directory.
    */
    class VideoStreamImages : public VideoStreamOpenCV
    {
    public:

        /** Create video stream from images in the specified directory.
            All images must be in the same resolution.
            \param dirPath Path to a directory to read the images from.
            \param fps The fake frames per second to use.
        */
        VideoStreamImages(const std::string& path, double fps = 30);

        /** Destructor.
        */
        ~VideoStreamImages();

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
        of the stream (assume 1/30 [s] between each image).
        */
        double getTimestamp() const;

        /** Get frames per second (always 30).
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

		static bool is_image(const std::string& path);

		static bool is_pattern(const std::string& path);

		static bool is_valid_pattern(const std::string& path);

    private:
        void getFrames(const std::string& path, std::vector<std::string>& frames) const;
		void getFramesFromDir(const std::string& dirPath,
			std::vector<std::string>& frames) const;
		void getFramesFromPattern(const std::string& path,
			std::vector<std::string>& frames) const;

    private:
        std::string mPath;
        int mWidth, mHeight;
        unsigned int mCurrFrameIndex;
        double mFPS;
        double mDt;
        bool mFirstFrame;
        std::vector<std::string> mFrames;
        cv::Mat mFrame;
    };

}	// namespace vsal


#endif	// __OG_VIDEO_STREAM_IMAGES__