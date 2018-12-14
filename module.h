#ifndef module_h
#define module_h

#include "ttreex.h"

namespace RooUtil
{
    class Module
    {
        private:
            TTreeX* tx;
        public:
            virtual ~Module() = 0;
            virtual void AddOutput();
            virtual void FillOutput();
    };
}

#endif
