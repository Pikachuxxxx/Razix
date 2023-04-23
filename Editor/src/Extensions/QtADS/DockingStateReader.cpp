//============================================================================
/// \file   DockingStateReader.cpp
/// \author Uwe Kindler
/// \date   29.11.2019
/// \brief  Implementation of CDockingStateReader
//============================================================================
// clang-format off
#include "rzepch.h"
// clang-format on
//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockingStateReader.h"

namespace ads
{

//============================================================================
void CDockingStateReader::setFileVersion(int FileVersion)
{
	m_FileVersion = FileVersion;
}

//============================================================================
int CDockingStateReader::fileVersion() const
{
	return m_FileVersion;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockingStateReader.cpp
