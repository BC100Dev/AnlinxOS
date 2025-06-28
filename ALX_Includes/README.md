# ALX Library
I have decided to separate the ALX (AnLinX) libraries into three separate ones:

- ALX-Shared ([`./ALX`](ALX)): The general shared code between ALX64 and ALX32
- ALX64 (`libalx64.so`, `libalx64.a`): The AnlinxOS-exclusive code, non-reusable across other
  distributions, effectively being a System Library, alike to C Library, but tailored for C++,
  giving a name of "C++ Extended Library"
- ALX32 (`libalx32.so`, `libalx32.a`): A library that can be used across other distributions,
  while these specific libraries can't be used within AnlinxOS itself (from the current vision)

This is a rough concept on how the ALX library will be structured. For now, the vision for
ALX32 is designed that it can be used for other distributions and not AnlinxOS itself. However,
if the vision changes, or by somehow programmatically making ALX32 compatible with AnlinxOS
itself, then at that point, it will turn into the ALX-Shared Library, moving the ALX32 code into
the ALX-Shared (because get it, it's shared code...?)

Other than that, I don't have anything else to report for this library, neither for the OS...
well, except that I'm still working on it. You can see this library as a WinAPI alike.