#ifndef __VSAL_VIDEO_STREAM__
#define __VSAL_VIDEO_STREAM__

/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "vsal/vsal_export.h"

/************************************************************************************
*									 Declarations									*
************************************************************************************/

/************************************************************************************
*										Classes										*
************************************************************************************/

namespace vsal
{
	/** Provide a single video stream from camera or video file.
	*/
    class VSAL_EXPORT VideoStream
	{
	public:

        /** Virtual destuctor
        */
        virtual ~VideoStream() {}

        /** Open the video stream.
        */
        virtual bool open() = 0;

		/**	Close the video stream.
		*/
        virtual void close() = 0;

        /** Read frame.
        */
        virtual bool read() = 0;

		/** Get the frame's width.
		*/
        virtual int getWidth() const = 0;

		/** Get the frame's height.
		*/
        virtual int getHeight() const = 0;
		
		/** Get the timestamp in seconds of the last grabbed frame since the start
			of the stream.
		*/
        virtual double getTimestamp() const = 0;

        /** Get frames per second.
        */
        virtual double getFPS() const = 0;

		/** Get the data of the last grabbed frame.
		*/
        virtual void getFrameData(unsigned char* data) const = 0;

		/** Get the index of the last grabbed frame.
		*/
		virtual size_t getFrameIndex() const = 0;

		/** Returns true if the video stream is from a live camera feed.
		*/
		virtual bool isLive() const = 0;

		/** Returns true if video stream has been initialized already.
		*/
		virtual bool isOpened() const = 0;

        /** Check whether a new frame was captured.
        */
        virtual bool isUpdated() = 0;

		/** 0-based index of the frame to be decoded/captured next.
		*/
		virtual void seek(size_t index) = 0;

		/**	Total number of frames or the number of frames read in case of a live feed.
		*/
		virtual size_t size() const = 0;
	};

}	// namespace vsal


#endif	// __VSAL_VIDEO_STREAM__