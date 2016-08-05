#ifndef __VSAL_VIDEO_STREAM_FACTORY__
#define __VSAL_VIDEO_STREAM_FACTORY__

/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "vsal/vsal_export.h"
#include "vsal/VideoStream.h"
#include <string>

/************************************************************************************
*									 Declarations									*
************************************************************************************/

/************************************************************************************
*										Classes										*
************************************************************************************/

namespace vsal
{
    /** Singleton class for generating video streams from cameras or video files.
    */
    class VSAL_EXPORT VideoStreamFactory
    {
    public:

        /** Destructor
        */
        ~VideoStreamFactory();

        /** Get a singleton instance of the factory.
        */
        static VideoStreamFactory& getInstance();

        /** Create video stream from camera.
            uEye camera ids start from 1.
            \param device Device's id.
            \param frameWidth The width in pixels for each of the incoming frames.
            \param frameHeight The height in pixels for each of the incoming frames.
			\param Frames per second.
            \return A video stream than can be tracked from.
        */
        VideoStream* create(int device, int frameWidth = 0, int frameHeight = 0,
			double fps = 0);

        /** Create video stream from video file or a directory of images.
		\param path Path to a video file or a directory of images.
		\param fps Frames per second for displaying.
        */
        VideoStream* create(const std::string& path, double fps);

		/** Create video stream from video file, a directory of images or from camera.
		*/
		VideoStream* create(const std::string& path, int device,
			int frameWidth = 0, int frameHeight = 0, double fps = 0.0);

        /** Create video stream with command line arguments.
        */
        VideoStream* create(int argc, char** argv);

    private:

        /** Private default constructor
        */
        VideoStreamFactory();

    };

}	// namespace vsal


#endif	// __VSAL_VIDEO_STREAM_FACTORY__