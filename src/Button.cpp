//****************************************************************************************************************************
//
// File Name		: BUTTON.cpp
// Title		    : BUTTON C++ Code file
// software         : version 1.0
// Target MCU		: Atmel AVR Series
// Created:         : 07/06/2026
// Author:          : Yaser Rashnabadi
//
//****************************************************************************************************************************
#include "Button.h"

#define BUTTON_BIT_ON_PRESS 2
#define BUTTON_BIT_ON_RELEASE 4

#define REGISTER_BIT_SET(_byte_, _bit_) ((_byte_) |= (1UL << (_bit_)))
#define REGISTER_BIT_CLEAR(_byte_, _bit_) ((_byte_) &= ~(1UL << (_bit_)))
#define BUTTON_BIT_READ(_byte_, _bit_) (((_byte_) >> (_bit_)) & 0x01)
#define BUTTON_BIT_WRITE(_byte_, _bit_, _value_) (_value_ ? REGISTER_BIT_SET(_byte_, _bit_) : REGISTER_BIT_CLEAR(_byte_, _bit_))

uint8_t BUTTON::instanceCount = 0;
//****************************************************************************************************************************
//---------------------------------------------------- Button Function codes -------------------------------------------------
//****************************************************************************************************************************
//---------------------------- Constructor: GPIO and Button Mode Initialization
//=============================================================================
BUTTON::BUTTON(volatile uint8_t &portReg,
			   volatile uint8_t &ddrReg,
			   volatile uint8_t &pinReg,
			   uint8_t pinNumber,
			   ButtonMode mode,
			   uint32_t debounceTimeMs,
			   uint16_t holdTimeMs)
{
	_pinReg = &pinReg;
	_pin = pinNumber;
	_mode = mode;
	_debounceTimeMs = debounceTimeMs;
	_holdTimeMs = holdTimeMs;

	_onHandler = nullptr;
	_offHandler = nullptr;

	// Set port pin as pull-up or pull-down
	REGISTER_BIT_CLEAR(portReg, pinNumber);
	if (BUTTON_MODE_PULL_UP == mode)
	{
		// Enable internal pull-up
		REGISTER_BIT_SET(portReg, pinNumber);
	}

	// Set ddr pin as input
	REGISTER_BIT_CLEAR(ddrReg, pinNumber);

	if (instanceCount < 255)
	{
		instanceCount++;
	}
};
//=============================================================================
//-------------------------------------------------------- Function: Destructor
//=============================================================================
BUTTON::~BUTTON()
{
	if (instanceCount > 0)
		instanceCount--;
};
//=============================================================================
//---------------------------------------------- Get number of Button instances
//=============================================================================
uint8_t BUTTON::GetInstanceCount(void)
{
	return instanceCount;
};
//=============================================================================
//----------------------------------- Read Button State with Debounce Filtering
//=============================================================================
ButtonStatus BUTTON::readDebounced(uint32_t now)
{
	bool rawState;

	if (_mode == BUTTON_MODE_PULL_UP)
		rawState = !(*_pinReg & (1 << _pin));
	else
		rawState = (*_pinReg & (1 << _pin));

	if (rawState != _previousRawState)
	{
		_lastDebounceTime = now;
		_previousRawState = rawState;
	}

	if ((now - _lastDebounceTime) >= _debounceTimeMs)
		_debouncedState = rawState;

	return _debouncedState ? BUTTON_STATE_PRESSED : BUTTON_STATE_RELEASED;
};
//=============================================================================
//----------------------------------- One Key Press (Hold-time based detection)
//=============================================================================
ButtonStatus BUTTON::ButtonLongPress(uint32_t now)
{
	if (readDebounced(now) == BUTTON_STATE_PRESSED)
	{
		if (_pressStartTime == 0)
			_pressStartTime = now;

		if (!_holdTriggered && (now - _pressStartTime >= _holdTimeMs))
		{
			_holdTriggered = true;
			return BUTTON_STATE_PRESSED;
		}
	}
	else
	{
		_pressStartTime = 0;
		_holdTriggered = false;
	}

	return BUTTON_STATE_RELEASED;
};
//=============================================================================
//------------------------------------------ Hold-time based repeat press event
//=============================================================================
ButtonStatus BUTTON::ButtonRepeatPress(uint32_t now)
{
	if (readDebounced(now) == BUTTON_STATE_PRESSED)
	{
		if (_pressStartTime == 0)
			_pressStartTime = now;

		if (now - _pressStartTime >= _holdTimeMs)
		{
			return BUTTON_STATE_PRESSED;
		}
	}
	else
	{
		_pressStartTime = 0;
	}

	return BUTTON_STATE_RELEASED;
};
//=============================================================================
//----------------------------------- One Key Release (hold-time delayed event)
//=============================================================================
ButtonStatus BUTTON::ButtonRelease(uint32_t now)
{
	if (readDebounced(now) == BUTTON_STATE_PRESSED)
	{
		_pressStartTime = now;
		_releaseReady = false;
		return BUTTON_STATE_PRESSED;
	}

	if (_pressStartTime != 0)
	{
		if (!_releaseReady)
		{
			if (now - _pressStartTime >= _holdTimeMs)
				_releaseReady = true;
		}
		else
		{
			_pressStartTime = 0;
			_releaseReady = false;
			return BUTTON_STATE_RELEASED; // event واقعی
		}
	}

	return BUTTON_STATE_PRESSED;
};
//=============================================================================
//----------------------------- Hold-time based release event (continuous mode)
//=============================================================================
ButtonStatus BUTTON::ButtonReleaseRepeat(uint32_t now)
{
	if (readDebounced(now) == BUTTON_STATE_PRESSED)
	{
		_releaseStartTime = now;
		return BUTTON_STATE_PRESSED;
	}

	if (_releaseStartTime != 0)
	{
		if (now - _releaseStartTime >= _holdTimeMs)
			return BUTTON_STATE_RELEASED;
	}

	return BUTTON_STATE_PRESSED;
};
//=============================================================================
//---------------------------- Button event dispatcher (debounced + mode-based)
//=============================================================================
//=============================================================================
//---------------------------- Button event dispatcher (debounced + mode-based)
//=============================================================================
void BUTTON::dispatch(uint32_t now)
{
    ButtonStatus state;
    ButtonStatus triggerState;

    switch (_dispatchMode)
    {
    case BUTTON_MODE_RAW:
        state = readDebounced(now);
        triggerState = BUTTON_STATE_PRESSED;
        break;

    case BUTTON_MODE_DELAY_ONCE:
        state = ButtonLongPress(now);
        triggerState = BUTTON_STATE_PRESSED;
        break;

    case BUTTON_MODE_DELAY_REPEAT:
        state = ButtonRepeatPress(now);
        triggerState = BUTTON_STATE_PRESSED;
        break;

    case BUTTON_MODE_RELEASE_DELAY_ONCE:
        state = ButtonRelease(now);
        triggerState = BUTTON_STATE_RELEASED;
        break;

    case BUTTON_MODE_RELEASE_DELAY_REPEAT:
        state = ButtonReleaseRepeat(now);
        triggerState = BUTTON_STATE_RELEASED;
        break;

    default:
        return;
    }

    // Dual-handler mode (e.g. DIP switch)
    if (_offHandler != nullptr)
    {
        if (state == BUTTON_STATE_PRESSED)
        {
            if (_onHandler)
                _onHandler(*this);
        }
        else
        {
            _offHandler(*this);
        }

        return;
    }

    // Single-handler mode
    if (_onHandler && state == triggerState)
    {
        _onHandler(*this);
    }
};
//=============================================================================
//------------------------------------- Register handler for button press event
//=============================================================================
void BUTTON::bind(ButtonDispatchMode mode, ButtonEventHandler_t handler)
{
	_dispatchMode = mode;
    _onHandler = handler;
    _offHandler = nullptr;
};
//=============================================================================
//------------------------------- Register ON and OFF handlers for DIP switches
//=============================================================================
void BUTTON::bindSwitch(ButtonDispatchMode mode,
						ButtonEventHandler_t onHandler,
						ButtonEventHandler_t offHandler)
{
	_dispatchMode = mode;
	_onHandler = onHandler;
	_offHandler = offHandler;
};
//****************************************************************************************************************************
//----------------------------------------------------------  END  -----------------------------------------------------------
//****************************************************************************************************************************
