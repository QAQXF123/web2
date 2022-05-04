#pragma once
#include<iostream>
#include <stdarg.h>


const bool debug = false;

int Dprintf(const char *format, ...){
    if(debug){
        va_list args;
	    va_start(args, format);
        int n = vprintf(format, args);
        va_end(args);
        return n;
    }
    return -1;
}