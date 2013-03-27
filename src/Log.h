/**
Copyright (C) 2012-2014 Robin Skånberg

Permission is hereby granted, free of charge,
to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef Log_H
#define Log_H

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

void printErrorsGL(const char *func, int line);

#define logError(...) {printf("ERROR (%s:%i): ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n");}
#define logWarning(...) {printf("WARNING (%s:%i): ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n");}
#define logNote(...) {printf("NOTE: ");printf(__VA_ARGS__);printf("\n");}
#define logErrorsGL() printErrorsGL(__FUNCTION__, __LINE__)

#endif
