# flttestlib

A header-only library providing a few general utilities for testing the accuracy of floating point functions.

## Getting Started
Since the library is header-only, it doesn't need to be built. However you can build the examples and tests using CMake.

### Dependencies
Some utilities are provided for use with the MPFR library, so you must link against MPFR to use these features. However, you can define `FTL_NO_MPFR` to remove this dependency if you are not using them.

## Documentation