/** @file
@brief Video Stream Abstraction Layer utility functions.
*/

#ifndef __VSAL_UTILITIES__
#define __VSAL_UTILITIES__

#include <string>
#include <cctype>

namespace vsal
{
	const std::string IMAGE_FILTER =
		"(.*\\.(bmp|dib|jpeg|jpg|jpe|jp2|png|pbm|pgm|ppm|sr|ras))";

	/**	If the specified string is a number in [0, 9] return that number
	else return -1.
	*/
	inline int getDeviceID(const std::string& s)
	{
		return (s.size() == 1 && std::isdigit(s[0])) ? (int)(s[0] - '0') : -1;
	}

	bool is_image(const std::string& path);

	bool is_pattern(const std::string& path);

	bool is_valid_pattern(const std::string& path);

}   // namespace vsal

#endif	// __VSAL_UTILITIES__
