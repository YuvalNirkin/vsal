#ifndef __OG_VIDEO_STREAM_FACTORY__
#define __OG_VIDEO_STREAM_FACTORY__

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
            \param device Device's id
            \param frameWidth The width in pixels for each of the incoming frames
            \param frameHeight The height in pixels for each of the incoming frames
            \return A video stream than can be tracked from.
        */
        VideoStream* create(int device, int frameWidth, int frameHeight);

        /** Create video stream from video file or a directory of images.
        */
        VideoStream* create(const std::string& path);

    private:

        /** Private default constructor
        */
        VideoStreamFactory();

    };

}	// namespace vsal


#endif	// __OG_VIDEO_STREAM_FACTORY__