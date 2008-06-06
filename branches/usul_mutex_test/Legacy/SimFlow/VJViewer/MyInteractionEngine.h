
#ifndef __MY_INTERACTION_ENGINE_H__
#define __MY_INTERACTION_ENGINE_H__

#include "vjCore/OsgInteractionEngine.h"

class MyInteractionEngine : public OsgInteractionEngine
{
public:
	
	MyInteractionEngine() : 
	OsgInteractionEngine(), 
	mButton1Pressed	( false ), 
	mButton3Pressed	( false ), 
	mButton11Pressed( true  ), 	
	mButton12Pressed( false ), 
	mButton13Pressed( false ), 
	mButton14Pressed( false  )
	{		
	}	

	virtual ~MyInteractionEngine()
	{
	}

	const bool& getButton1Status() const			{ return mButton1Pressed;		}
	void setButton1Pressed( const bool& value )		{ mButton1Pressed = value;		}

	const bool& getButton3Status() const			{ return mButton3Pressed;		}
	void setButton3Pressed( const bool& value )		{  mButton3Pressed = value;		}

	const bool& getButton11Status() const			{ return mButton11Pressed;		}
	void setButton11Pressed( const bool& value )	{ mButton11Pressed = value;		}

	const bool& getButton12Status() const			{ return mButton12Pressed;		}
	void setButton12Pressed( const bool& value )	{ mButton12Pressed = value;		}

	const bool& getButton13Status()					{ return mButton13Pressed;		}
	void setButton13Pressed( const bool& value )	{ mButton13Pressed = value;		}

	const bool& getButton14Status()					{ return mButton14Pressed;		}
	void setButton14Pressed( const bool& value )	{ mButton14Pressed = value;		}

	virtual void joystickButton1GlobalAction( DeviceData d_data )
	{		
		mButton1Pressed = true;
	}

	/// Move selected object down. 
	virtual void joystickButton3GlobalAction( DeviceData d_data )
	{   			
		mButton3Pressed = true;
	}

	virtual void joystickButton11GlobalAction( DeviceData d_data )
	{   			
		mButton11Pressed = !mButton11Pressed;
	}

	/// Move selected object up. 
	virtual void joystickButton12GlobalAction( DeviceData d_data )
	{
		mButton12Pressed = true;
	}

	virtual void joystickButton13GlobalAction( DeviceData d_data )
	{
		mButton13Pressed = true;
	}

	/// Collision test ON/OFF. 
	virtual void joystickButton14GlobalAction( DeviceData d_data )
	{
		mButton14Pressed = true;
	}

private:

	bool mButton1Pressed;
	bool mButton3Pressed;
	bool mButton11Pressed;
	bool mButton12Pressed;
	bool mButton13Pressed;
	bool mButton14Pressed;
};

#endif // __MY_INTERACTION_ENGINE_H_

