#ifndef __OG_VIDEO_STREAM_UEYE__
#define __OG_VIDEO_STREAM_UEYE__

/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "VideoStreamExport.h"
#include "VideoStreamOpenCV.h"

/************************************************************************************
*									 Declarations									*
************************************************************************************/

/************************************************************************************
*										Classes										*
************************************************************************************/

namespace og
{
    /** Provide a single video stream from IDS uEye camera.
    */
    class VS_API VideoStream_uEye : public VideoStreamOpenCV
    {
    public:

        /** Create video stream for a uEye camera.
        */
        VideoStream_uEye();

        /** Create video stream for a uEye camera.
            \param frameWidth The width in pixels for each of the incoming frames
            \param frameHeight The height in pixels for each of the incoming frames
        */
        VideoStream_uEye(int frameWidth, int frameHeight);

        /** Create video stream for a uEye camera.
            \param device Device's id
            \param frameWidth The width in pixels for each of the incoming frames
            \param frameHeight The height in pixels for each of the incoming frames
        */
        VideoStream_uEye(int device, int frameWidth, int frameHeight);

        /** Destructor.
        */
        ~VideoStream_uEye();

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
        void allocateSequence();

        void checkError(int err);

    private:
        int mDevice;
        int mRequestedWidth, mRequestedHeight;
        std::string mVideoFile;
        cv::Mat mFrame;
        double mTimestamp;
        unsigned long mCamHandle;
        int mMemoryID;
        unsigned long long mFrameCounter;
        bool mUpdated;

        // Memory and sequence buffers
        static const int SEQUENCE_SIZE = 2;
        int		m_lSeqMemId[SEQUENCE_SIZE];	// camera memory - buffer ID
        char*	m_pcSeqImgMem[SEQUENCE_SIZE];	// camera memory - pointer to buffer
        int		m_nSeqNumId[SEQUENCE_SIZE];	// varibale to hold the number of the sequence buffer Id
        int		m_SeqMode;						// ringbuffer or linear bufer
    };

}	// namespace og


#endif	// __OG_VIDEO_STREAM_UEYE__