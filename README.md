# TFTP C++ Library
![ProjectLogo.svg](ProjectLogo.svg)

This repository provides a C++ Library which implements the TFTP Protocol.
It also provides a commandline TFTP Server and Client.

## Dependencies
First level dependencies:
 - [Helper Library](https://git.thomas-vogt.de/thomas-vogt/helper)
 - Boost Library
   - Boost::Log
   - Boost::Program Options
 - Qt (Version 5 or 6) - Optionally

## Building
The library uses CMake to handle build configuration.
CMake Presets are provided to generate builds compiling with:
- GNU GCC,
- Clang, and
- MSVC.

For each compiler and environment, following variants can be built:
 - Static debug,
 - Static release,
 - Shared debug, and
 - Shared release.

Test Environments are:
- Linux,
- Windows MinGW, and
- Windows MSVC.

**Note:**
For managing dependencies, i.e. Windows MSVC, a VCPKG configuration is provided.

## License
This project is licensed under the terms of the [*Mozilla Public License Version 2.0* (MPL)](LICENSE).

## References
- [RFC 1350 The TFTP Protocol (Revision 2)](http://tools.ietf.org/html/rfc1350)
- [RFC 2347 TFTP Option Extension](http://tools.ietf.org/html/rfc2347)
- [RFC 2348 TFTP Blocksize Option](http://tools.ietf.org/html/rfc2348)
- [RFC 2349 TFTP Timeout Interval and Transfer Size Options](http://tools.ietf.org/html/rfc2349)
