//******************************************************************************************************
//
// File Name		: BUTTON.h
// Title		    : BUTTON interface header file
// software         : version 1.0
// Target MCU		: Atmel AVR Series
// Created:         : 07/06/2026
// Author:          : Yaser Rashnabadi
//
//******************************************************************************************************
#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdint.h>
//=======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Interface Function use Class <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//=======================================================================================================

enum ButtonMode
{
	BUTTON_MODE_PULL_DOWN, // حالت Pull-Down
	BUTTON_MODE_PULL_UP	   // حالت Pull-Up (با مقاومت داخلی)
};

enum ButtonStatus
{
	BUTTON_STATE_UNKNOWN = -1, // کلید نامشخص است
	BUTTON_STATE_RELEASED = 0, // کلید آزاد است
	BUTTON_STATE_PRESSED	   // کلید فشرده شده است
};

enum ButtonDispatchMode
{
	BUTTON_MODE_RAW,				 // اجرای هندلر بر اساس وضعیت لحظه‌ای کلید
	BUTTON_MODE_DELAY_ONCE,			 // اجرای یک‌باره پس از نگه داشتن کلید به مدت holdTimeMs
	BUTTON_MODE_DELAY_REPEAT,		 // اجرای مداوم پس از نگه داشتن کلید به مدت holdTimeMs
	BUTTON_MODE_RELEASE_DELAY_ONCE,	 // اجرای یک‌باره پس از رها شدن کلید و گذشت holdTimeMs
	BUTTON_MODE_RELEASE_DELAY_REPEAT // اجرای مداوم پس از رها شدن کلید و گذشت holdTimeMs
};

class BUTTON;
typedef bool (*ButtonEventHandler_t)(BUTTON &);

/*
 *کاربرد کلاس :
 *این کلاس برای مدیریت کلیدها و DIP Switchها با قابلیت حذف نویز،
 *تشخیص فشردن، رها شدن و رویدادهای مبتنی بر زمان استفاده می‌شود.
 *
 *نحوه استفاده :
 *
 *BUTTON ButtonName(
 *    PORTB,                  // رجیستر PORT
 *    DDRB,                   // رجیستر DDR
 *    PINB,                   // رجیستر PIN
 *    PB0,                    // شماره پایه
 *    BUTTON_MODE_PULL_UP,    // نوع اتصال کلید
 *    20,                     // زمان حذف نویز (ms)
 *    1000                    // زمان تأخیر رویداد (ms)
 *);
 *
 *ButtonName.bind(
 *    BUTTON_MODE_DELAY_ONCE,
 *    ButtonHandler
 *);
 *
 *while (1)
 *{
 *    ButtonName.dispatch(SysTick_GetMillis());
 *}
 *
 *توجه :
 *این کلاس از هیچ کلاس دیگری مشتق نشده است.
 */
class BUTTON
{
public:
	BUTTON(volatile uint8_t &portReg,
		   volatile uint8_t &Ddrx,
		   volatile uint8_t &Pinx,
		   uint8_t pinNumber,
		   ButtonMode mode,
		   uint32_t debounceThreshold,
		   uint16_t setHoldTime);
	~BUTTON();

	ButtonDispatchMode _dispatchMode;

	/*
	 *کاربرد تابع :
	 *تعداد نمونه‌های فعال ایجاد شده از کلاس BUTTON را برمی‌گرداند.
	 *
	 *نوع تابع :
	 *استاتیک
	 *
	 *آرگومان ورودی ندارد
	 *
	 *آرگومان خروجی :
	 *تعداد اشیاء فعال از نوع BUTTON
	 *
	 *نحوه استفاده :
	 *
	 *uint8_t count = BUTTON::GetInstanceCount();
	 *
	 *توجه :
	 *این تابع بدون نیاز به ایجاد شیء از کلاس قابل فراخوانی است.
	 */
	static uint8_t GetInstanceCount(void);

	/*
	 * کاربرد تابع :
	 * پردازش وضعیت کلید، اعمال حذف نویز (Debounce)، بررسی زمان Hold
	 * و فراخوانی هندلرهای ثبت‌شده بر اساس حالت انتخاب‌شده در bind() یا bindSwitch()
	 *
	 * نوع تابع :
	 * دینامیک
	 *
	 * آرگومان ورودی :
	 * now : زمان جاری سیستم بر حسب میلی‌ثانیه
	 *
	 * آرگومان خروجی ندارد
	 *
	 * نحوه استفاده :
	 *
	 * while (1)
	 * {
	 *     MyButton.dispatch(SysTick_GetMillis());
	 *     // سایر کدهای برنامه
	 * }
	 *
	 * توجه :
	 * این تابع باید به‌صورت دوره‌ای و با یک مرجع زمانی یکنواخت فراخوانی شود.
	 */
	void dispatch(uint32_t now);

	/*
	 * کاربرد تابع :
	 * ثبت یک هندلر برای رویدادهای تک‌حالته کلید
	 *
	 * نوع تابع :
	 * دینامیک
	 *
	 * آرگومان ورودی :
	 * mode    : نحوه تولید رویداد
	 * handler : تابعی که هنگام وقوع رویداد فراخوانی می‌شود
	 *
	 * آرگومان خروجی ندارد
	 *
	 * نحوه استفاده :
	 *
	 * bool ButtonHandler(BUTTON &btn)
	 * {
	 *     return true;
	 * }
	 *
	 * MyButton.bind(
	 *     BUTTON_MODE_DELAY_ONCE,
	 *     ButtonHandler
	 * );
	 *
	 * توجه :
	 * در حالت‌های مبتنی بر فشردن، رویداد هنگام بازگشت
	 * BUTTON_STATE_PRESSED فراخوانی می‌شود.
	 *
	 * در حالت‌های مبتنی بر رها شدن، رویداد هنگام بازگشت
	 * BUTTON_STATE_RELEASED فراخوانی می‌شود.
	 */
	void bind(ButtonDispatchMode mode, ButtonEventHandler_t handler);

	/*
	 * کاربرد تابع :
	 * ثبت دو هندلر مجزا برای مدیریت وضعیت فعال و غیرفعال کلید یا DIP Switch
	 *
	 * نوع تابع :
	 * دینامیک
	 *
	 * آرگومان ورودی :
	 * mode       : نحوه تولید رویداد
	 * onHandler  : تابع فراخوانی هنگام فعال شدن کلید
	 * offHandler : تابع فراخوانی هنگام غیرفعال شدن کلید
	 *
	 * آرگومان خروجی ندارد
	 *
	 * نحوه استفاده :
	 *
	 * bool SwitchOn(BUTTON &btn)
	 * {
	 *     return true;
	 * }
	 *
	 * bool SwitchOff(BUTTON &btn)
	 * {
	 *     return true;
	 * }
	 *
	 * DipSwitch.bindSwitch(
	 *     BUTTON_MODE_RAW,
	 *     SwitchOn,
	 *     SwitchOff
	 * );
	 *
	 * توجه :
	 * این تابع برای کلیدهای دوحالته مانند DIP Switch مناسب است.
	 * در حالت BUTTON_MODE_RAW، با هر تغییر وضعیت پایدار،
	 * هندلر متناظر با وضعیت فعلی کلید فراخوانی می‌شود.
	 */
	void bindSwitch(ButtonDispatchMode mode,
					ButtonEventHandler_t onHandler,
					ButtonEventHandler_t offHandler);

private:
	static uint8_t instanceCount; // تعداد اشیائ ساخته شده از کلاس
	volatile uint8_t *_pinReg;
	uint8_t _pin;
	ButtonMode _mode;
	uint32_t _debounceTimeMs;
	uint16_t _holdTimeMs;
	uint32_t _lastDebounceTime = 0;

	bool _debouncedState = false;
	bool _previousRawState = false;

	bool _holdTriggered;
	uint32_t _pressStartTime;

	bool _releaseReady = false;
	uint32_t _releaseStartTime;

	uint8_t _state;

	ButtonEventHandler_t _onHandler;
	ButtonEventHandler_t _offHandler;

	ButtonStatus readDebounced(uint32_t now);
	ButtonStatus ButtonLongPress(uint32_t now);
	ButtonStatus ButtonRepeatPress(uint32_t now);
	ButtonStatus ButtonRelease(uint32_t now);
	ButtonStatus ButtonReleaseRepeat(uint32_t now);
};
//======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  End Class <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//======================================================================================================
#endif //__BUTTON_H__
