#pragma once
#include <Wire.h>
#include <CUU_Interface.h>
#include <Noritake_VFD_CUU.h>

class VFDInterface : public Print
{
	virtual void ClearScreen() = 0;
	virtual void Home() = 0;
	virtual void SetCursor(byte pos) = 0;
	virtual void SetCursor(byte col, byte line) = 0;
	virtual void DisplayOn() = 0;
	virtual void DisplayOff() = 0;
	virtual void CursorOn() = 0;
	virtual void CursorOff() = 0;
	virtual void BlinkOn() = 0;
	virtual void BlinkOff() = 0;
	virtual void ScrollDisplayLeft() = 0;
	virtual void ScrollDisplayRight() = 0;
	virtual void LeftToRight() = 0;
	virtual void RightToLeft() = 0;
	virtual void Autoscroll() = 0;
	virtual void NoAutoscroll() = 0;
	virtual void Brightness(byte brightness) = 0;
};

typedef enum VFD_ACTION
{
	VFD_CLEAR_SCREEN,
	VFD_HOME,
	VFD_SETCURSOR_POS,
	VFD_SETCURSOR_XY,
	VFD_DISPLAY_ON,
	VFD_DISPLAY_OFF,
	VFD_CURSOR_ON,
	VFD_CURSOR_OFF,
	VFD_BLINK_ON,
	VFD_BLINK_OFF,
	VFD_SCROLL_DISPLAY_LEFT,
	VFD_SCROLL_DISPLAY_RIGHT,
	VFD_LEFT_TO_RIGHT,
	VFD_RIGHT_TO_LEFT,
	VFD_AUTO_SCROLL,
	VFD_NO_AUTO_SCROLL,
	VFD_BRIGHTNESS,
	VFD_PRINT_CHAR,
	VFD_PRINT_STRING,
	VFD_PRINT_BUFFER
};

class VFDi2cSlave : public VFDInterface
{
  private:

	Noritake_VFD_CUU vfd;

	void ProcessCommand(VFD_ACTION action)
	{
		switch (action)
		{
			case VFD_HOME:
			{
				Home();
				break;
			}
			case VFD_CLEAR_SCREEN:
			{
				ClearScreen();
				break;
			}
			case VFD_SETCURSOR_POS:
			{
				byte c = GetByte();
				SetCursor(c);
				break;
			}
			case VFD_SETCURSOR_XY:
			{
				byte x = GetByte();
				byte y = GetByte();
				SetCursor(x, y);
				break;
			}
			case VFD_DISPLAY_ON:
			{
				DisplayOn();
				break;
			}
			case VFD_DISPLAY_OFF:
			{
				DisplayOff();
				break;
			}
			case VFD_CURSOR_ON:
			{
				CursorOn();
				break;
			}
			case VFD_CURSOR_OFF:
			{
				CursorOff();
				break;
			}
			case VFD_BLINK_ON:
			{
				BlinkOn();
				break;
			}
			case VFD_BLINK_OFF:
			{
				BlinkOff();
				break;
			}
			case VFD_SCROLL_DISPLAY_LEFT:
			{
				ScrollDisplayLeft();
				break;
			}
			case VFD_SCROLL_DISPLAY_RIGHT:
			{
				ScrollDisplayRight();
				break;
			}
			case VFD_LEFT_TO_RIGHT:
			{
				LeftToRight();
				break;
			}
			case VFD_RIGHT_TO_LEFT:
			{
				RightToLeft();
				break;
			}
			case VFD_AUTO_SCROLL:
			{
				Autoscroll();
				break;
			}
			case VFD_NO_AUTO_SCROLL:
			{
				NoAutoscroll();
				break;
			}
			case VFD_BRIGHTNESS:
			{
				byte bright = GetByte();
				Brightness(bright);
				break;
			}
			case VFD_PRINT_CHAR:
			{
				byte c = GetByte();
				write(c);
				break;
			}
			case VFD_PRINT_STRING:
			{
				char * psz = GetString();
				write(psz);
				free(psz);
				break;
			}
			case VFD_PRINT_BUFFER:
			{
				size_t cb = GetByte();
				byte * pv = GetByteBuffer(cb);
				write(pv, cb);
				free(pv);
				break;
			}
		}
	}
	
	byte * GetByteBuffer(size_t cb)
	{
		byte * pb = (byte *) calloc(cb, 1);
		byte * pc = pb;

		while (cb)
		{
			*pc++ = GetByte();
			cb--;
		}
		return pb;
	}

	// GetString - Pull a long indicating length and then the string data, with NO nul on the wire

	char * GetString()
	{
		size_t len = GetByte();
		char * psz = (char *) malloc(len + 1);
		for (int i = 0; i < len; i++)
			psz[i] = GetByte();
		psz[len] = '\0';
	}

	byte GetByte()
	{
		byte c = Wire.read();
		return c;
	}

	virtual size_t write(byte data) override
	{
		vfd.print(data);
		return data != 0;
	}

	virtual size_t write(const char *str)  
	{
		if (str == NULL) 
			return 0;
		vfd.print(str);
		return strlen(str);
	}

	virtual size_t write(const byte *buffer, size_t size) override
	{
		vfd.print(buffer, size);
	}
	
public:

	
	int Init(CUU_Interface & interface, byte width, byte height)
	{
		vfd.begin(width, height);		// Tell module its size
		vfd.interface(interface);		// Select which interface to use
		vfd.CUU_brightness(100);
		vfd.brightnessBoost();			// In case module has brightness boost
		vfd.CUU_init();					// Initialize module
	}

	void ReadAndProcessCommand()
	{
		byte command = Wire.read();
		ProcessCommand((VFD_ACTION)command);
	}

	virtual void ClearScreen() override
	{
		vfd.CUU_clearScreen();
	}

	virtual void Home() override
	{
		vfd.CUU_home();
	}

	virtual void SetCursor(byte pos) override
	{
		vfd.CUU_setCursor(pos);
	}

	virtual void SetCursor(byte col, byte line) override
	{
		vfd.CUU_setCursor(col, line);
	}

	virtual void DisplayOn() override
	{
		vfd.CUU_displayOn();
	}

	virtual void DisplayOff() override
	{
		vfd.CUU_displayOff();
	}

	virtual void CursorOn() override
	{
		vfd.CUU_cursorOn();
	}

	virtual void CursorOff() override
	{
		vfd.CUU_cursorOff();
	}

	virtual void BlinkOn() override
	{
		vfd.CUU_blinkOn();
	}

	virtual void BlinkOff() override
	{
		vfd.CUU_blinkOff();
	}

	virtual void ScrollDisplayLeft() override
	{
		vfd.CUU_scrollDisplayLeft();
	}

	virtual void ScrollDisplayRight() override
	{ 
		vfd.CUU_scrollDisplayRight();
	}

	virtual void LeftToRight() override
	{
		vfd.CUU_leftToRight();
	}

	virtual void RightToLeft() override
	{
		vfd.CUU_rightToLeft();
	}

	virtual void Autoscroll() override
	{
		vfd.CUU_autoscroll();
	}

	virtual void NoAutoscroll() override
	{
		vfd.CUU_noAutoscroll();
	}

	virtual void Brightness(byte brightness) override
	{
		vfd.CUU_brightness(brightness);
	}
};

class VFDi2cMaster : public VFDInterface
{
private:

	byte _deviceAddress = 0;

	void SendCommand(VFD_ACTION action)
	{
		Wire.write((byte)action);
	}

public:

	int Init(byte deviceAddress)
	{
		_deviceAddress = deviceAddress;
		Wire.begin();
	}
	
	// Print interface

	virtual size_t write(uint8_t c) override
	{
		PrintChar(c);
	}

	// VFDInterface

	virtual void ClearScreen() override
	{
		Serial.print("Sending ClearScreen to Address ");
		Serial.println(_deviceAddress);

		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_CLEAR_SCREEN);
		Wire.endTransmission();
	}
	virtual void Home() override
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_HOME);
		Wire.endTransmission();
	}
	virtual void SetCursor(byte pos) override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_SETCURSOR_POS);
		Wire.write(pos);
		Wire.endTransmission();
	}
	virtual void SetCursor(byte col, byte line) override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_SETCURSOR_XY);
		Wire.write(col);
		Wire.write(line);
		Wire.endTransmission();

	}
	virtual void DisplayOn() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_DISPLAY_ON);
		Wire.endTransmission();
	}
	virtual void DisplayOff() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_DISPLAY_OFF);
		Wire.endTransmission();
	}
	virtual void CursorOn() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_CURSOR_ON);
		Wire.endTransmission();
	}
	virtual void CursorOff() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_CURSOR_OFF);
		Wire.endTransmission();
	}
	virtual void BlinkOn() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_BLINK_ON);
		Wire.endTransmission();
	}
	virtual void BlinkOff() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_BLINK_OFF);
		Wire.endTransmission();
	}
	virtual void ScrollDisplayLeft() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_SCROLL_DISPLAY_LEFT);
		Wire.endTransmission();
	}
	virtual void ScrollDisplayRight() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_SCROLL_DISPLAY_RIGHT);
		Wire.endTransmission();
	}
	virtual void LeftToRight() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_LEFT_TO_RIGHT);
		Wire.endTransmission();
	}
	virtual void RightToLeft() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_RIGHT_TO_LEFT);
		Wire.endTransmission();
	}
	virtual void Autoscroll() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_AUTO_SCROLL);
		Wire.endTransmission();
	}
	virtual void NoAutoscroll() override 
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_NO_AUTO_SCROLL);
		Wire.endTransmission();
	}
	virtual void Brightness(byte brightness) override
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_BRIGHTNESS);
		Wire.write(brightness);
		Wire.endTransmission();
	}
	
	void PrintChar(char c)  
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_PRINT_CHAR);
		Wire.write(c);
		Wire.endTransmission();
	}
	void PrintString(const char * psz)  
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_PRINT_STRING);
		Wire.write((byte)strlen(psz));
		for (int i = 0; i < strlen(psz); i++)
			Wire.write(psz[i]);
		Wire.endTransmission();
	}
	void PrintBuffer(byte cb, byte * pb)
	{
		Wire.beginTransmission(_deviceAddress);
		SendCommand(VFD_PRINT_BUFFER);
		Wire.write(cb);
		for (int i = 0; i < cb; i++)
			Wire.write(pb[i]);
		Wire.endTransmission();
	}

	void printf(const char *szFormat, ...)
	{
		char szBuffer[256];

		va_list  vaList;
		va_start(vaList, szFormat);
		vsnprintf(szBuffer, 256, szFormat, vaList);
		va_end(vaList);

		PrintString(szBuffer);
	}
};
