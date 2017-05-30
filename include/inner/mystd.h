#pragma once


#define MYSTD_NS_BEGIN namespace mystd {
#define MYSTD_NS_END } /* end of mystd */

#define MYSTD_DETAIL_NS_BEGIN namespace detail {
#define MYSTD_DETAIL_NS_END } /* end of detail */

MYSTD_NS_BEGIN
MYSTD_DETAIL_NS_BEGIN

struct _Wrap_int
{   // wraps int so that int argument is favored over _Wrap_int
    _Wrap_int(int)
        {   // do nothing
        }
};

MYSTD_DETAIL_NS_END
MYSTD_NS_END
