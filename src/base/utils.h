#ifndef FEATHER_BASE_UTILS_H
#define FEATHER_BASE_UTILS_H

#include <feather/types.h>

#include <sstream>

namespace feather {

	/*
	 * Using this macro you can do things like STR("foo" << 5 << "bar" << 3.14)
	 * which will return a "foo5bar3.14" string
	 */ 

	#define STR(message) \
		static_cast<std::ostringstream&>(std::ostringstream().flush() << message).str()

	/*
	 * Throws an exception that includes filename and linenumber
	 * Example: FEATHER_THROW("Invalid range: " << a << "," << b)
	 */

	#define FEATHER_THROW(message) \
		throw FeatherException( STR(__FILE__ << ":" << __LINE__ << " (" << __func__ << ") " << message) )

} // namespace feather

#endif
