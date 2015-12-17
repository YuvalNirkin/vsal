#ifndef __OG_VIDEO_STREAM_OPENCV__
#define __OG_VIDEO_STREAM_OPENCV__

/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "vsal/vsal_export.h"
#include "vsal/VideoStream.h"
#include <opencv2\core.hpp>

/************************************************************************************
*									 Declarations									*
************************************************************************************/

/************************************************************************************
*										Classes										*
************************************************************************************/

namespace vsal
{
    /** Provide a single video stream from camera or video file with direct access to 
        the data of the grabbed frames.
    */
    class VSAL_EXPORT VideoStreamOpenCV : public VideoStream
    {
    public:

        /** Virtual destuctor
        */
        virtual ~VideoStreamOpenCV() {}

        /** Get the last grabbed frame without copying.
        */
        virtual cv::Mat getFrame() = 0;

        /** Get the last grabbed frame without copying as grayscale.
        */
        virtual cv::Mat getFrameGrayscale() = 0;
    };

}	// namespace vsal


#endif	// __OG_VIDEO_STREAM_OPENCV__