#pragma once

#include <iostream>
#include <string>

namespace Razix
{
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
            RC
        };

        /* Engine release date */
        struct Date
        {
            int Day = 0;
            int Month = 0;
            int Year = 0;

            Date(int dd = 0, int mm = 0, int yyyy = 0) : Day(dd), Month(mm), Year(yyyy) {}
        };

    public:
        /* Creates the version information of the engine */
        Version(int major, int  minor, int patch, Stage stage, Date releaseDate) : Major(major), Minor(minor), Patch(patch), ReleaseStage(stage), ReleaseDate(releaseDate) {}
        
        /* Gets the major version of the engine */
        int getVersionMajor() const { return Major; }
        /* gets the minor version of the engine */
        int getVersionMinor() const { return Minor; }
        /* gets the patch version of the Engine */
        int getVersionPatch() const { return Patch; }
        /* Gets the release stage of the Engine */
        Stage getReleaseStage() const { return ReleaseStage; }
        /* Gets the release data of the engine */
        Date getReleaseDate() const { return ReleaseDate; }

        /* Returns the version as a string (Major.Minor.Patch) */
        std::string getVersionString() const { return (std::to_string(Major) + "." + std::to_string(Minor) + "." + std::to_string(Patch)); }
        /* Returns the release stage as a string */
        std::string getReleaseStageString() const
        {
            switch (ReleaseStage)
            {
            case Stage::Development:
                return "Dev";
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
        std::string getReleaseDateString() const { return (std::to_string(ReleaseDate.Day) + "-" + std::to_string(ReleaseDate.Month) + "-" + std::to_string(ReleaseDate.Year)); }

    private:
        int Major = 0;
        int Minor = 0;
        int Patch = 0;

        Stage ReleaseStage;

        Date ReleaseDate;
    };

    /* The Engine's current Version and release status */
    ////-----------------------------------------------------------------------------------------------------------////
    /**/ const Version RazixVersion = Version(0, 16, 0, Version::Stage::Development, Version::Date(15, 9, 2021));
    ////-----------------------------------------------------------------------------------------------------------////

}