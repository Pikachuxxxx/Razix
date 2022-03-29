#include "rzxpch.h"
#include "MTLContext.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZLog.h"

namespace Razix {
    namespace Graphics{
        
        MTLContext::MTLContext(RZWindow* windowHandle)
            : m_Window(windowHandle)
        {
            RAZIX_CORE_ASSERT(windowHandle, "[Metal] Window Handle is NULL!");
        }
    
        void MTLContext::Init()
        {
            // Initialize the device
        }
    
        void MTLContext::Destroy()
        {
        
        }
    }
}
