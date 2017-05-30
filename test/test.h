#pragma once

#include <cassert>
#include <cstdio>


void test(bool condition, const char* error_info = "")
{
    if(!condition){
        fputs(error_info, stderr);
        assert(false);
    }
}


#include <inner/type_traits.h>

using namespace mystd;
