#pragma once


#define RAZIX_MARK_BEGIN(name, labelColor) BeginMarker(name, labelColor);

#define RAZIX_MARK_ADD(name, labelColor) InsertMarker(name, labelColor);

#define RAZIX_MARK_END() EndMarker();


void BeginMarker(const std::string& name, glm::vec4 color);
void InsertMarker(const std::string& name, glm::vec4 color);
void EndMarker();