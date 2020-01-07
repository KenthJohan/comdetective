/*
MIT License

Copyright (c) 2019 CSC Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#pragma once

//https://misc.flogisoft.com/bash/tip_colors_and_formatting

//Stringyfier for c preproccessor
#define TCOL_STR1(x) #x
#define TCOL_STR(x) TCOL_STR1 (x)


//ANSI text color escape string formatter
#define TCOL(text, foreground, background) \
"\x1B[" TCOL_STR (text) ";3" TCOL_STR (foreground) ";4" TCOL_STR (background) "m"


//Used for text attributes
#define TCOL_NORMAL       0
#define TCOL_BOLD         1
#define TCOL_UNDERSCORE   4
#define TCOL_BLINK        5
#define TCOL_REVERSE      7
#define TCOL_CONCEAL      8


//Used for foreground and background color
#define TCOL_BLACK   0
#define TCOL_RED     1
#define TCOL_GREEN   2
#define TCOL_YELLOW  3
#define TCOL_BLUE    4
#define TCOL_MAGENTA 5
#define TCOL_CYAN    6
#define TCOL_WHITE   7
#define TCOL_DEFAULT 9


//Default text color used in terminals
//#define TCOL_DEFAULT TCOL (TCOL_NORMAL, TCOL_DEFAULT, TCOL_DEFAULT)
#define TCOL_RST "\x1B[0m"
