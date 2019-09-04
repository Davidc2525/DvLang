#include "process.h"
#include <iostream>
#include <unistd.h>

namespace PROCESS
{

int native_u_sleep(intdv time)
{
    usleep(time * 1000);
    return time;
}

intdv native_sleep(intdv time)
{
    sleep(time);
    return time;
}

intdv println(dv_string adr)
{
    std::cout << STRING::String::dv_str_to_c_srt(Native::getInstance()->heap, adr) << std::endl;
    return 0;
}

intdv print(dv_string adr)
{
    std::cout << STRING::String::dv_str_to_c_srt(Native::getInstance()->heap, adr);
    return 0;
}

intdv sum(dv_int a, dv_int b)
{
    std::cout << a << " + " << b << endl;
    return a + b;
}

intdv native_itos(dv_int integer)
{
    return STRING::String::c_str_to_dv_srt(Native::getInstance()->heap, std::to_string(integer));
}

} // namespace PROCESS
