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
const int MIN_OVERLAY_WIDTH = 400;
const int MIN_OVERLAY_HEIGHT = 200;

/************************************************************************************
*									Implementation									*
************************************************************************************/
namespace vsal
{
	Viewer::Viewer(const std::string& title, vsal::VideoStreamOpenCV* vs,
		bool pause, bool loop, float fps, float scale) :
		m_title(title), m_vs(vs),
		m_running(true), m_refresh(true), m_pause(pause), m_loop(loop), m_init(false),
		m_total_frames(0), m_next_frame(0), m_delay(0),
        m_fps(fps), m_requested_fps(fps), m_scale(scale)
	{
		if (vs == nullptr) throw runtime_error("Invalid video source specified!");
	}

	int Viewer::getDelay() const { return m_delay;  }

    float Viewer::getFPS() const { return m_fps; }

    float Viewer::getScale() const { return m_scale; }

	void Viewer::init()
	{
		if (m_init) return;

		// Open video source
		if (!m_vs->isOpened() && !m_vs->open())
			throw runtime_error("Failed to open video source!");

		// Initialize parameters
		m_total_frames = m_vs->size();
		m_loop = m_loop && !m_vs->isLive();

        // Set fps
        m_fps = 30;     // Default fps
        m_delay = 33;   // Default delay
        if(!setFPS(m_requested_fps)) setFPS((float)m_vs->getFPS());

		m_init = true;
	}

	bool Viewer::isLoop() const { return m_loop;  }

	bool Viewer::isPaused() const { return m_pause; }

	void Viewer::pause(bool pause)
	{
		m_pause = pause;
		m_refresh = !m_pause;
	}

	bool Viewer::setDelay(int delay)
    { 
        if (delay <= 0) return false;
        m_delay = delay;
        m_fps = 1e3f / m_delay;
        return true;
    }

    bool Viewer::setFPS(float fps)
    {
        if (fps < 1) return false;
        m_fps = fps;
        m_delay = (int)std::round(1e3f / m_fps);
        return true;
    }

	void Viewer::setLoop(bool loop) { m_loop = loop; }

    bool Viewer::setScale(float scale)
    {
        if (scale < 0.05f) return false;
        m_scale = scale;
        m_refresh = true;
        return true;
    }

	void Viewer::stop() { m_running = false; }

	void Viewer::run()
	{
		init();
		m_running = true;

		// Main loop
        cv::Mat render_frame;
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
                    render_frame = frame.clone();   // TODO: Performance issue
                    if (std::abs(m_scale - 1.0f) < 1e-3f) onRender(render_frame);
                    else
                    {
                        cv::Size size((int)std::round(frame.cols*m_scale),
                            (int)std::round(frame.rows*m_scale));
                        frame_scaled.create(size, frame.type());
                        cv::resize(frame, frame_scaled, size);
                        render_frame = frame_scaled;
                        onRender(render_frame);
                    }
					cv::imshow(m_title, render_frame);
					m_refresh = !m_pause;
				}
			}

			// Check for keys pressed
            int key = cv::waitKey(m_delay);
#ifdef __linux__
			key = (int)(char)key;
#endif
			if(key >= 0) onKeyPress(key);
		}
	}

	void Viewer::onFrameChange(size_t prev_ind, size_t curr_ind) {}

    void Viewer::onKeyPress(int key)
	{
		switch (key) 
        {
		case ESCAPE:
		case 'q':
			stop(); break;
        case SPACE:
	    case 'p':
			pause(!m_pause); break;
        case LEFT:
        case DOWN:
			seek_relative(-1); break;
        case RIGHT:
        case UP:
			seek_relative(1); break;
        case '-': 
            setScale(m_scale - 0.1f);
            if(m_pause) seek_relative(0);
            break;
        case '+': 
            setScale(m_scale + 0.1f); 
            if (m_pause) seek_relative(0);
            break;
		default:
			break;
		}
	}

	void Viewer::onRender(cv::Mat& frame)
	{
        std::string msg;
        if (frame.cols < MIN_OVERLAY_WIDTH || frame.rows < MIN_OVERLAY_HEIGHT)
        {
            // Render minimalized overlay
            if (m_vs->isLive()) msg = (boost::format("%d") %
                m_vs->getFrameIndex()).str();
            else msg = (boost::format("%d / %d")
                % m_vs->getFrameIndex() % (m_total_frames - 1)).str();
            cv::putText(frame, msg, cv::Point(5, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(0, 102, 255), 1, CV_AA);
        }
        else
        {
            // Render full overlay
            if (m_vs->isLive()) msg = (boost::format("Frame: %d") %
                m_vs->getFrameIndex()).str();
            else msg = (boost::format("Frame: %d / %d")
                % m_vs->getFrameIndex() % (m_total_frames - 1)).str();
            cv::putText(frame, msg, cv::Point(15, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(0, 102, 255), 1, CV_AA);
            cv::putText(frame, (boost::format("FPS: %.1f") % m_fps).str(), cv::Point(15, 40),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
            cv::putText(frame, "press space\\p to pause or escape\\q to quit", cv::Point(10, frame.rows - 20),
                cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
        }
	}

	void Viewer::seek_relative(int di)
	{
		m_pause = true;
		int i = (int)m_vs->getFrameIndex();
		int n = (int)m_total_frames;
		i = ((i + di) % n + n) % n;
		m_next_frame = (size_t)i;
		m_vs->seek(m_next_frame);
		m_refresh = true;
	}

}	// namespace vsal
