/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "vsal/Viewer.h"
#include <exception>
#include <boost/format.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

/************************************************************************************
*									  Namespaces									*
************************************************************************************/
using std::runtime_error;

/************************************************************************************
*									 Declarations									*
************************************************************************************/


/************************************************************************************
*									Implementation									*
************************************************************************************/
namespace vsal
{
	Viewer::Viewer(const std::string& title, vsal::VideoStreamOpenCV* vs,
		bool pause, bool loop, int delay) :
		m_title(title), m_vs(vs),
		m_running(true), m_refresh(true), m_pause(pause), m_loop(loop), m_init(false),
		m_total_frames(0), m_next_frame(0), m_delay(delay)
	{
		if (vs == nullptr) throw runtime_error("Invalid video source specified!");
	}

	int Viewer::getDelay() const { return m_delay;  }

	void Viewer::init()
	{
		if (m_init) return;

		// Open video source
		if (!m_vs->isOpened() && !m_vs->open())
			throw runtime_error("Failed to open video source!");

		// Initialize parameters
		m_total_frames = m_vs->size();
		m_loop = m_loop && !m_vs->isLive();

		double vs_fps = m_vs->getFPS();
		if (m_delay < 0 && vs_fps >= 1)
		{
			// Use the video stream's frames per second
			m_delay = (int)std::round(1e3 / vs_fps);
		}
		m_init = true;
	}

	bool Viewer::isLoop() const { return m_loop;  }

	bool Viewer::isPaused() const { return m_pause; }

	void Viewer::pause(bool pause)
	{
		m_pause = pause;
		m_refresh = !m_pause;
	}



	void Viewer::setDelay(int delay) { m_delay = delay; }

	void Viewer::setLoop(bool loop) { m_loop = loop; }

	void Viewer::stop() { m_running = false; }

	void Viewer::run()
	{
		init();
		m_running = true;

		// Main loop
		while (m_running)
		{
			if (m_refresh)
			{
				if (m_loop && m_next_frame >= m_total_frames)
					m_vs->seek(0);
				size_t prev_ind = m_vs->getFrameIndex();
				if (!m_vs->read()) break;
				size_t curr_ind = m_vs->getFrameIndex();
				m_next_frame = curr_ind + 1;
				onFrameChange(prev_ind, curr_ind);
				if (m_vs->isUpdated())
				{
					cv::Mat frame = m_vs->getFrame();
					onRender(frame);
					cv::imshow(m_title, frame);
					m_refresh = !m_pause;
				}
			}

			// Check for keys pressed
            char key = cv::waitKey(m_delay);
			onKeyPress(key);
		}
	}

	void Viewer::onFrameChange(size_t prev_ind, size_t curr_ind) {}

    void Viewer::onKeyPress(char key)
	{
		switch (key) {
		case 27:	// Escape
		case 'q':
			stop(); break;
        case ' ':	// Space
		case 'p':
			pause(!m_pause); break;
        case 'Q':	// left
        case 'T':	// down
			seek_relative(-1); break;
        case 'S':	// right
        case 'R':	// up
			seek_relative(1); break;
		default:
			break;
		}
	}

	void Viewer::onRender(cv::Mat& frame)
	{
		std::string msg;

		// Show overlay
		if (m_vs->isLive()) msg = (boost::format("Frame count: %d") %
			m_vs->getFrameIndex()).str();
		else msg = (boost::format("Frame: %d / %d")
			% m_vs->getFrameIndex() % (m_total_frames - 1)).str();
		cv::putText(frame, msg, cv::Point(15, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5,
			cv::Scalar(0, 102, 255), 1, CV_AA);
		cv::putText(frame, (boost::format("FPS: %.1f") % m_vs->getFPS()).str(), cv::Point(15, 40),
			cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
		cv::putText(frame, "press space\\p to pause or escape\\q to quit", cv::Point(10, frame.rows - 20),
			cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
	}

	void Viewer::seek_relative(int di)
	{
		if (di == 0) return;
		m_pause = true;
		int i = (int)m_vs->getFrameIndex();
		int n = (int)m_total_frames;
		i = ((i + di) % n + n) % n;
		m_next_frame = (size_t)i;
		m_vs->seek(m_next_frame);
		m_refresh = true;
	}

}	// namespace vsal
