#pragma once

#include "RZEvent.h"


namespace Razix {

	class RAZIX_API RZKeyEvent : public RZEvent
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryKeyboard | (int)EventCategory::EventCategoryInput)
	protected:
		RZKeyEvent(int keycode)
			: m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	class RAZIX_API RZKeyPressedEvent : public RZKeyEvent
	{
	public:
		RZKeyPressedEvent(int keycode, int repeatCount)
			: RZKeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

	class RAZIX_API RZKeyReleasedEvent : public RZKeyEvent
	{
	public:
		RZKeyReleasedEvent(int keycode)
			: RZKeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}