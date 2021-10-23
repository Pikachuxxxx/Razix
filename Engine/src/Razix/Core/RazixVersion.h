#pragma once

#include <iostream>
#include <string>

namespace Razix
{
    struct Version
    {
    public :
        enum class Stage
        {
            Development,
            Alpha,
            Beta,
            RC
        };

        struct Date
        {
            int Day = 0;
            int Month = 0;
            int Year = 0;

            Date(int dd = 0, int mm = 0, int yyyy = 0) : Day(dd), Month(mm), Year(yyyy) {}
        };


        Version(int major, int  minor, int patch, Stage stage, Date releaseDate) : Major(major), Minor(minor), Patch(patch), ReleaseStage(stage), ReleaseDate(releaseDate) {}
        
        int GetVersionMajor() const { return Major; }
        int GetVersionMinor() const { return Minor; }
        int GetVersionPatch() const { return Patch; }
        Stage GetReleaseStage() const { return ReleaseStage; }
        Date GetReleaseDate() const { return ReleaseDate; }

        std::string GetVersionString() const { return (std::to_string(Major) + "." + std::to_string(Minor) + "." + std::to_string(Patch)); }
        std::string GetReleaseStageString() const
        {
            switch (ReleaseStage)
            {
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
        std::string GetReleaseDateString() const { return (std::to_string(ReleaseDate.Day) + "-" + std::to_string(ReleaseDate.Month) + "-" + std::to_string(ReleaseDate.Year)); }

    private:
        int Major = 0;
        int Minor = 0;
        int Patch = 0;

        Stage ReleaseStage;

        Date ReleaseDate;
    };

    /* The Engine's current Version and release status */
    ////-----------------------------------------------------------------------------------------------------------////
    /**/ const Version RazixVersion = Version(0, 16, 0, Version::Stage::Development, Version::Date(15, 9, 2021));  /**/
    ////-----------------------------------------------------------------------------------------------------------////

}