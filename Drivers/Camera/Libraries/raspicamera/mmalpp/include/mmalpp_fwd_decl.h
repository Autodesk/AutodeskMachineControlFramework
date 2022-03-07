#ifndef MMALPP_FWD_DECL_H
#define MMALPP_FWD_DECL_H

#include "macros.h"

MMALPP_BEGIN

class Connection;
// Need to forward declare its constructor too inorder to use std::unique_ptr
struct ConnectionDeleter
{
    void operator()(Connection* c);
};

MMALPP_END

#endif // MMALPP_FWD_DECL_H
