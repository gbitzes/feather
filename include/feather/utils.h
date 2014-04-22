#ifndef ___FEATHER_BASE_UTILS_H
#define ___FEATHER_BASE_UTILS_H

#include <feather/types.h>

#include <sstream>
#include <iostream>

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

	/*
	 * Log a message
	 */

	#define LOG(message) \
	 	std::cout << __FILE__ << ":" << __LINE__ << " " << message << std::endl;

	/*
	 * Assertion - only active on a DEBUG build
	 */

	#ifdef DEBUG	
		#define FEATHER_ASSERT(assertion) \
	 		if(assertion == false) throw FeatherException( STR(__FILE__ << ":" << __LINE__ << " (" << __func__ << "): Assertion " << #assertion << " failed." )  )
	#else
	 	#define FEATHER_ASSERT(assertion) 
	#endif

	/*
	 * Disallow copying and assigning
	 */

	#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
		TypeName(const TypeName&); \
		void operator=(const TypeName&)





} // namespace feather

#endif
