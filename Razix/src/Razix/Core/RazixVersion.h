#pragma once

#include "Razix/Core/Log.h"

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

		std::string GetVersionString() const { return (std::to_string(Major) + "." + std::to_string(Minor) + "." + std::to_string(Patch)); }
		std::string GetReleaseStage() const
		{
			switch (ReleaseStage)
			{
			case Razix::Version::Stage::Development:
				return "Development";
				break;
			case Razix::Version::Stage::Alpha:
				return "Alpha";
				break;
			case Razix::Version::Stage::Beta:
				return "Beta";
					break;
			case Razix::Version::Stage::RC:
				return "RC";
					break;
			default:
				return "None";
				break;
			}
		}
		std::string GetReleaseDate() const { return (std::to_string(ReleaseDate.Day) + "-" + std::to_string(ReleaseDate.Month) + "-" + std::to_string(ReleaseDate.Year)); }

	private:
		int Major = 0;
		int Minor = 0;
		int Patch = 0;

		Stage ReleaseStage;

		Date ReleaseDate;
	};

	///	The Engine's current Version and release status
	const Version RazixVersion = Version(0, 16, 0, Version::Stage::Development, Version::Date(25, 06, 2021));
}