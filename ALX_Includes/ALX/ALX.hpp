// Remember "Windows.h"? Yeah, this is the equivalent for this one.

#ifndef ANLINXOS_ALX_HPP
#define ANLINXOS_ALX_HPP

#if __has_include(<ALX64/ALX64.hpp>)
#include <ALX64/ALX64.hpp>
#define ALX64_FOUND
#endif

#if __has_include(<ALX32/ALX32.hpp>)
#include <ALX32/ALX32.hpp>
#define ALX32_FOUND
#endif

#if !defined(ALX64_FOUND) && !defined(ALX32_FOUND)
#error "No ALX-specific library headers found"
#endif

#endif //ANLINXOS_ALX_HPP
