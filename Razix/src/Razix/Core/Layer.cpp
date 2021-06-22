#include "rzxpch.h"
#include "Layer.h"

namespace Razix
{

    Layer::Layer(const std::string& name /*= "Layer"*/)
        #ifndef RZX_DIST
        : m_DebugName(name)
        #endif
    {

    }

    Layer::~Layer()
    {

    }

}