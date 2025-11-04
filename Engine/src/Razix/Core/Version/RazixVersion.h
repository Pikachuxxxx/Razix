#pragma once

#include "Razix/Core/Utils/RZStringUtilities.h"

namespace Razix {
    /* Describes the current version details for Razix Engine */
    struct Version
    {
    public:
        /* Describes the stage of release */
        enum class Stage
        {
            Development,
            Alpha,
            Beta,
            RC    // Release candidate
        };

        /* Engine release date */
        struct Date
        {
            int Day   = 0;
            int Month = 0;
            int Year  = 0;

            Date(int dd = 0, int mm = 0, int yyyy = 0)
                : Day(dd), Month(mm), Year(yyyy) {}
        };

    public:
        /* Creates the version information of the engine */
        Version(int major, int minor, int patch, Stage stage, Date releaseDate)
            : m_Major(major), m_Minor(minor), m_Patch(patch), m_ReleaseStage(stage), m_ReleaseDate(releaseDate) {}

        static Razix::Version ParseVersionString(const std::string& versionStr)
        {
            int major = 0, minor = 0, patch = 0;
            int ret = sscanf(versionStr.c_str(), "%d.%d.%d", &major, &minor, &patch);
            (void) ret;                                                                                                // Ignore return value, we assume the format is correct
            return Razix::Version(major, minor, patch, Razix::Version::Stage::Development, Razix::Version::Date());    // TODO: Add stage and date in serialized string
        }

        /* Gets the major version of the engine */
        int getVersionMajor() const { return m_Major; }
        /* gets the minor version of the engine */
        int getVersionMinor() const { return m_Minor; }
        /* gets the patch version of the Engine */
        int getVersionPatch() const { return m_Patch; }
        /* Gets the release stage of the Engine */
        Stage getReleaseStage() const { return m_ReleaseStage; }
        /* Gets the release data of the engine */
        Date getReleaseDate() const { return m_ReleaseDate; }

        /* Returns the version as a string (Major.Minor.Patch) */
        std::string getVersionString() const { return (Utilities::to_string(m_Major) + "." + Utilities::to_string(m_Minor) + "." + Utilities::to_string(m_Patch)); }
        /* Returns the release stage as a string */
        std::string getReleaseStageString() const
        {
            switch (m_ReleaseStage) {
                case Stage::Development:
                    return "Development";
                    break;
                case Stage::Alpha:
                    return "Alpha";
                    break;
                case Stage::Beta:
                    return "Beta";
                    break;
                case Stage::RC:
                    return "RC";
                    break;
                default:
                    return "None";
                    break;
            }
        }
        /* Returns the release data as a string (dd-mm-yyyy) */
        std::string getReleaseDateString() const { return (std::to_string(m_ReleaseDate.Day) + "-" + std::to_string(m_ReleaseDate.Month) + "-" + std::to_string(m_ReleaseDate.Year)); }

    private:
        int   m_Major = 0;    /* The Major release version of the engine  */
        int   m_Minor = 0;    /* The minor release version of the engine  */
        int   m_Patch = 0;    /* The Patch release version of the engine  */
        Stage m_ReleaseStage; /* Current release stage of the engine      */
        Date  m_ReleaseDate;  /* Current version release date             */
    };

    /* The Engine's current Version and release status [Date format : DD/MM/YYYY]*/
    ////----------------------------------------------------------------------------------------------------------------////
    /**/ const Version RazixVersion = Version(0, 49, 0, Version::Stage::Development, Version::Date(23, 6, 2025));
    ////----------------------------------------------------------------------------------------------------------------////

}    // namespace Razix
