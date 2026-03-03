#pragma once
#include "RZEvent.h"

namespace Razix {

    class RAZIX_API RZFileChangedEvent : public RZEvent
    {
    public:
        RZFileChangedEvent(const RZString& path)
            : m_Path(path) {}

        const RZString& GetPath() const { return m_Path; }

        RZString ToString() const override
        {
            char buffer[512];
            rz_snprintf(buffer, sizeof(buffer), "FileChangedEvent: %s", m_Path.c_str());
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kFileChanged)
    private:
        RZString m_Path;
    };

    class RAZIX_API RZFileCreatedEvent : public RZEvent
    {
    public:
        RZFileCreatedEvent(const RZString& path)
            : m_Path(path) {}

        const RZString& GetPath() const { return m_Path; }

        RZString ToString() const override
        {
            char buffer[512];
            rz_snprintf(buffer, sizeof(buffer), "FileCreatedEvent: %s", m_Path.c_str());
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kFileCreated)
    private:
        RZString m_Path;
    };

    class RAZIX_API RZFileDeletedEvent : public RZEvent
    {
    public:
        RZFileDeletedEvent(const RZString& path)
            : m_Path(path) {}

        const RZString& GetPath() const { return m_Path; }

        RZString ToString() const override
        {
            char buffer[512];
            rz_snprintf(buffer, sizeof(buffer), "FileDeletedEvent: %s", m_Path.c_str());
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kFileDeleted)
    private:
        RZString m_Path;
    };

}    // namespace Razix
