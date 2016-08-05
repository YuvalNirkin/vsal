#ifndef __VSAL_VIEWER__
#define __VSAL_VIEWER__

/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "vsal/vsal_export.h"
#include "vsal/VideoStreamOpenCV.h"
#include <opencv2/core.hpp>

/************************************************************************************
*									 Declarations									*
************************************************************************************/

/************************************************************************************
*										Classes										*
************************************************************************************/

namespace vsal
{
	enum KeyCode
	{
#if _WIN32
		LEFT = 2424832,
		UP = 2490368,
		RIGHT = 2555904,
		DOWN = 2621440,
#else
		LEFT = 81,
		UP = 82,
		RIGHT = 83,
		DOWN = 84,
#endif
		ESCAPE = 27,
		SPACE = ' '
	};

	/** Simple viewer implementation using video stream.
	Includes pause feature and bidirectional stepping.
	*/
    class VSAL_EXPORT Viewer
	{
	public:

		/** Constructor.
		\param title The title of the display window.
		\param vs The video stream to display.
		\param pause Set whether to start paused or not.
		\param loop Set whether to loop the video stream or not.
		\param delay Set the minimum delay between displayed frames [ms].
		By default the delay will be derived from the video stream.
		*/
		Viewer(const std::string& title, vsal::VideoStreamOpenCV* vs,
			bool pause = false, bool loop = false, int delay = -1);

		/**	Get the minimum delay between displayed frames.
		By default the delay will be derived from the video stream.
		This should be called after initialization.
		*/
		virtual int getDelay() const;

		/**	Initialize the viewer and the video stream if necessary.
		*/
		virtual void init();

		/**	Returns true if the video stream will be run in a loop.
		*/
		virtual bool isLoop() const;

		/**	Returns true if the viewer is paused.
		*/
		virtual bool isPaused() const;

		/**	Pause or unpause the viewer.
		*/
		virtual void pause(bool pause);

		/**	Set the minimum delay between displayed frames [ms].
		By default the delay will be derived from the video stream.
		*/
		virtual void setDelay(int delay);
		
		/**	Set whether to loop the video stream or not.
		*/
		virtual void setLoop(bool loop);

		/**	Stops the viewer. Intended to be used from a different thread.
		*/
		virtual void stop();

		/**	Start the viewer. 
		*/
		virtual void run();	

	protected:

		/**	This method will called after a new frame was read.
		\param prev_ind The index of the previous frame.
		\param curr_ind The index of the current frame.
		*/
		virtual void onFrameChange(size_t prev_ind, size_t curr_ind);

		/**	This method will be called every iteration for processing key presses.
		\param key The code of the presses key.
		*/
        virtual void onKeyPress(int key);

		/**	This method will be called wheneven a render is required.
		\param frame The current frame from the video stream.
		*/
		virtual void onRender(cv::Mat& frame);

		/**	Seek a frame that is relative to the current frame.
		\param di The delta index from the current frame.
		*/
		virtual void seek_relative(int di);

	protected:
		std::string m_title;			///< The title of the display window.
		vsal::VideoStreamOpenCV* m_vs;	///< The video stream to display.
		bool m_running, m_refresh, m_pause, m_loop, m_init;
		size_t m_total_frames, m_next_frame;
		int m_delay;
	};

}	// namespace vsal


#endif	// __VSAL_VIEWER__
