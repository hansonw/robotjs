#include <node.h>
#include <nan.h>
#include <v8.h>
#include <vector>
#include "mouse.h"
#include "deadbeef_rand.h"
#include "keypress.h"
#include "screen.h"
#include "screengrab.h"
#include "MMBitmap.h"
#include "snprintf.h"
#include "microsleep.h"

using namespace v8;

//Global delays.
int mouseDelay = 10;
int keyboardDelay = 10;

/*
 __  __                      
|  \/  | ___  _   _ ___  ___ 
| |\/| |/ _ \| | | / __|/ _ \
| |  | | (_) | |_| \__ \  __/
|_|  |_|\___/ \__,_|___/\___|

*/

NAN_METHOD(moveMouse) 
{
	NanScope();
	if (args.Length() < 2) 
	{
		return NanThrowError("Invalid number of arguments."); 
	}
	size_t x = args[0]->Int32Value();
	size_t y = args[1]->Int32Value();

	MMPoint point;
	point = MMPointMake(x, y);
	moveMouse(point);
	microsleep(mouseDelay);

	NanReturnValue(NanNew("1"));
}

NAN_METHOD(moveMouseSmooth) 
{
	NanScope();
	if (args.Length() < 2) 
	{
		return NanThrowError("Invalid number of arguments."); 
	}
	size_t x = args[0]->Int32Value();
	size_t y = args[1]->Int32Value();

	MMPoint point;
	point = MMPointMake(x, y);
	smoothlyMoveMouse(point);
	microsleep(mouseDelay);

	NanReturnValue(NanNew("1"));
}

NAN_METHOD(getMousePos) 
{
	NanScope();

	MMPoint pos = getMousePos();

 	//Return object with .x and .y.
	Local<Object> obj = NanNew<Object>();
	obj->Set(NanNew<String>("x"), NanNew<Integer>((int)pos.x));
	obj->Set(NanNew<String>("y"), NanNew<Integer>((int)pos.y));
	NanReturnValue(obj);
}

NAN_METHOD(mouseClick) 
{
	NanScope();

	MMMouseButton button = LEFT_BUTTON;
	bool doubleC = false;

	if (args.Length() > 0)
	{
		char *b;

		v8::String::Utf8Value bstr(args[0]->ToString());
		b = *bstr;

		if (strcmp(b, "left") == 0)
		{
			button = LEFT_BUTTON;
		}
		else if (strcmp(b, "right") == 0)
		{
			button = RIGHT_BUTTON;
		}
		else if (strcmp(b, "middle") == 0)
		{
			button = CENTER_BUTTON;
		}
		else
		{
			return NanThrowError("Invalid mouse button specified."); 
		}
	}
	
	if (args.Length() == 2)
	{
		doubleC = args[1]->BooleanValue();
	}
	else if (args.Length() > 2)
	{
		return NanThrowError("Invalid number of arguments.");
	}
	
	if (!doubleC)
	{
		clickMouse(button);
		
	}
	else
	{
		doubleClick(button);
	}

	microsleep(mouseDelay);

	NanReturnValue(NanNew("1"));
}

NAN_METHOD(mouseToggle) 
{
	NanScope();

	MMMouseButton button = LEFT_BUTTON;
	bool down;

	if (args.Length() > 0)
	{
		const char *d = (*v8::String::Utf8Value(args[0]->ToString()));

		if (strcmp(d, "down") == 0)
		{
			down = true;;
		}
		else if (strcmp(d, "up") == 0)
		{
			down = false;
		}
		else
		{
			return NanThrowError("Invalid mouse button state specified."); 
		}
	}

	if (args.Length() == 2)
	{
		char *b = (*v8::String::Utf8Value(args[1]->ToString()));

		if (strcmp(b, "left") == 0)
		{
			button = LEFT_BUTTON;
		}
		else if (strcmp(b, "right") == 0)
		{
			button = RIGHT_BUTTON;
		}
		else if (strcmp(b, "middle") == 0)
		{
			button = CENTER_BUTTON;
		}
		else
		{
			return NanThrowError("Invalid mouse button specified."); 
		}
	}
	else if (args.Length() > 2)
	{
		return NanThrowError("Invalid number of arguments.");
	}

	toggleMouse(down, button);
	microsleep(mouseDelay);

	NanReturnValue(NanNew("1"));
}

NAN_METHOD(setMouseDelay) 
{
	NanScope();

	if (args.Length() != 1) 
	{
		return NanThrowError("Invalid number of arguments."); 
	}

	mouseDelay = args[0]->Int32Value();

	NanReturnValue(NanNew("1"));
}

NAN_METHOD(scrollMouse) 
{
	NanScope();

	//Get the values of magnitude and direction from the arguments list.
	if(args.Length() == 2)	
	{
		int scrollMagnitude = args[0]->Int32Value();
		int scrollDirection = args[1]->Int32Value();
		
		scrollMouse(scrollMagnitude, scrollDirection);
		microsleep(mouseDelay);
		
		NanReturnValue(NanNew("1"));
	} 
	else 
	{
		return NanThrowError("Invalid number of arguments.");
	}
}
/*
 _  __          _                         _ 
| |/ /___ _   _| |__   ___   __ _ _ __ __| |
| ' // _ \ | | | '_ \ / _ \ / _` | '__/ _` |
| . \  __/ |_| | |_) | (_) | (_| | | | (_| |
|_|\_\___|\__, |_.__/ \___/ \__,_|_|  \__,_|
          |___/           
*/

struct KeyMapping {
	const char *str;
	MMKeyCode keyCode;
};

int CheckKeyCodes(char* k, MMKeyCode *key) 
{
	if (!key) return -1;

	static KeyMapping keyMapping[] = {
		{"alt", K_ALT},
		{"command", K_META},
		{"control", K_CONTROL},
		{"shift", K_SHIFT},
		{"backspace", K_BACKSPACE},
		{"enter", K_RETURN},
		{"tab", K_TAB},
		{"up", K_UP},
		{"down", K_DOWN},
		{"left", K_LEFT},
		{"right", K_RIGHT},
		{"escape", K_ESCAPE},
		{"delete", K_DELETE},
		{"home", K_HOME},
		{"end", K_END},
		{"pageup", K_PAGEUP},
		{"pagedown", K_PAGEDOWN},
		{"space", K_SPACE},
		{"f1", K_F1},
		{"f2", K_F2},
		{"f3", K_F3},
		{"f4", K_F4},
		{"f5", K_F5},
		{"f6", K_F6},
		{"f7", K_F7},
		{"f8", K_F8},
		{"f9", K_F9},
		{"f10", K_F10},
		{"f11", K_F11},
		{"f12", K_F12},
	};

	#if defined(USE_X11)
		static KeyMapping volumeKeyMapping[] = {
			{"volumemute", 0},
			{"volumedown", 0},
			{"volumeup", 0}
		};
	#else
		static KeyMapping volumeKeyMapping[] = {
		{"volumemute", K_VOLUMEMUTE},
		{"volumedown", K_VOLUMEDOWN},
			{"volumeup", K_VOLUMEUP}
	};
	#endif

	for (auto mapping : keyMapping) {
		if (strcmp(k, mapping.str) == 0) {
			*key = mapping.keyCode;
			return 0;
		}
	}

	for (auto mapping : volumeKeyMapping) {
		if (strcmp(k, mapping.str) == 0) {
			if (mapping.keyCode) {
				*key = mapping.keyCode;
				return 0;
			} else {
				NanThrowError("Volume keys are not supported on this system.");
			}
		}
	}

	if (strcmp(k, "printscreen") == 0) {
		#if defined(IS_WINDOWS)
			*key = K_PRINTSCREEN;
			return 0;
			#else
				NanThrowError("printscreen is only supported on Windows.");
			#endif
		}

	if (strlen(k) == 1)
	{
		*key = keyCodeForChar(*k);
	}
	else
	{
		return -2;
	}

	return 0;
}

int CheckKeyFlags(char* f, MMKeyFlags* flags) 
{
	if (!flags) return -1;

	if (strcmp(f, "alt") == 0) 
	{
    	*flags = MOD_ALT;
  	}
  	else if(strcmp(f, "command") == 0) 
	{
    	*flags = MOD_META;
  	}
  	else if(strcmp(f, "control") == 0) 
	{
    	*flags = MOD_CONTROL;
  	}
  	else if(strcmp(f, "shift") == 0) 
	{
    	*flags = MOD_SHIFT;
	}
	else if(strcmp(f, "none") == 0) 
	{
    	*flags = MOD_NONE;
  	}
 	else 
	{
    	return -2;
  	}

	return 0;
}

NAN_METHOD(keyTap) 
{
	NanScope();
	
	MMKeyFlags flags = MOD_NONE;
	MMKeyCode key;

  	char *k;
  	char *f;

  	v8::String::Utf8Value fstr(args[1]->ToString());
  	v8::String::Utf8Value kstr(args[0]->ToString());
  	k = *kstr;
  	f = *fstr;

	switch (args.Length()) 
	{
		case 2:
			break;
		case 1:
			f = NULL;
			break;
		default:
			return NanThrowError("Invalid number of arguments.");
	}

  	if (f) 
	{
		switch(CheckKeyFlags(f, &flags)) 
    	{
			case -1:
				return NanThrowError("Null pointer in key flag.");
        		break;
			case -2:
				return NanThrowError("Invalid key flag specified."); 
        		break;
    	}
	}

	switch(CheckKeyCodes(k, &key)) 
	{
		case -1:
			return NanThrowError("Null pointer in key code.");
			break;
		case -2:
			return NanThrowError("Invalid key code specified."); 
			break;
		default:
			tapKeyCode(key, flags);
			microsleep(keyboardDelay);
	}

	NanReturnValue(NanNew("1"));
}


NAN_METHOD(keyToggle) 
{
	NanScope();

	MMKeyFlags flags = MOD_NONE;
	MMKeyCode key;
  
	char *k;
	bool down;
	char *f;

	v8::String::Utf8Value kstr(args[0]->ToString());
	v8::String::Utf8Value fstr(args[2]->ToString());
	down = args[1]->BooleanValue();
	k = *kstr;
	f = *fstr;

	switch (args.Length()) 
	{
    	case 3:
      		break;
    	case 2:
      		f = NULL;
      		break;
    	default:
      		return NanThrowError("Invalid number of arguments.");
	}

	if (f) 
	{
		switch(CheckKeyFlags(f, &flags)) 
		{
			case -1:
        		return NanThrowError("Null pointer in key flag.");
        		break;
			case -2:
        		return NanThrowError("Invalid key flag specified."); 
        		break;
		}
	}

	switch(CheckKeyCodes(k, &key)) 
	{
		case -1:
    		return NanThrowError("Null pointer in key code.");
			break;
		case -2:
			return NanThrowError("Invalid key code specified."); 
			break;
		default:
			toggleKeyCode(key, down, flags);
      		microsleep(keyboardDelay);
	}

	NanReturnValue(NanNew("1"));
}

NAN_METHOD(typeString) 
{
	NanScope();

	char *str;
	NanUtf8String string(args[0]);

	str = *string;

	typeString(str);

	NanReturnValue(NanNew("1"));
}

NAN_METHOD(setKeyboardDelay) 
{
	NanScope();
	
	if (args.Length() != 1) 
	{
		return NanThrowError("Invalid number of arguments."); 
	}

	keyboardDelay = args[0]->Int32Value();

	NanReturnValue(NanNew("1"));
}

/*
  ____                           
 / ___|  ___ _ __ ___  ___ _ __  
 \___ \ / __| '__/ _ \/ _ \ '_ \ 
  ___) | (__| | |  __/  __/ | | |
 |____/ \___|_|  \___|\___|_| |_|
                                 
*/

NAN_METHOD(getPixelColor) 
{
	NanScope();

	MMBitmapRef bitmap;
	MMRGBHex color;

	size_t x = args[0]->Int32Value();
	size_t y = args[1]->Int32Value();

	bitmap = copyMMBitmapFromDisplayInRect(MMRectMake(x, y, 1, 1));

	color = MMRGBHexAtPoint(bitmap, 0, 0);
	
	char hex [7];

	//Length needs to be 7 because snprintf includes a terminating null.
	//Use %06x to pad hex value with leading 0s. 
	snprintf(hex, 7, "%06x", color);

	destroyMMBitmap(bitmap);

	NanReturnValue(NanNew(hex));
}

NAN_METHOD(getScreenSize) 
{
	NanScope();
	
	//Get display size.
	MMSize displaySize = getMainDisplaySize();

	//Create our return object.
	Local<Object> obj = NanNew<Object>();
	obj->Set(NanNew<String>("width"), NanNew<Number>(displaySize.width));
	obj->Set(NanNew<String>("height"), NanNew<Number>(displaySize.height));

	//Return our object with .width and .height.
	NanReturnValue(obj);
}

void init(Handle<Object> target) 
{

	target->Set(NanNew<String>("moveMouse"),
		NanNew<FunctionTemplate>(moveMouse)->GetFunction());

	target->Set(NanNew<String>("moveMouseSmooth"),
		NanNew<FunctionTemplate>(moveMouseSmooth)->GetFunction());

	target->Set(NanNew<String>("getMousePos"),
		NanNew<FunctionTemplate>(getMousePos)->GetFunction());

	target->Set(NanNew<String>("mouseClick"),
		NanNew<FunctionTemplate>(mouseClick)->GetFunction());

	target->Set(NanNew<String>("mouseToggle"),
		NanNew<FunctionTemplate>(mouseToggle)->GetFunction());
		
	target->Set(NanNew<String>("scrollMouse"),
		NanNew<FunctionTemplate>(scrollMouse)->GetFunction());

	target->Set(NanNew<String>("setMouseDelay"),
		NanNew<FunctionTemplate>(setMouseDelay)->GetFunction());

	target->Set(NanNew<String>("keyTap"),
		NanNew<FunctionTemplate>(keyTap)->GetFunction());
	
	target->Set(NanNew<String>("keyToggle"),
		NanNew<FunctionTemplate>(keyToggle)->GetFunction());

	target->Set(NanNew<String>("typeString"),
		NanNew<FunctionTemplate>(typeString)->GetFunction());

	target->Set(NanNew<String>("setKeyboardDelay"),
		NanNew<FunctionTemplate>(setKeyboardDelay)->GetFunction());

	target->Set(NanNew<String>("getPixelColor"),
		NanNew<FunctionTemplate>(getPixelColor)->GetFunction());

	target->Set(NanNew<String>("getScreenSize"),
		NanNew<FunctionTemplate>(getScreenSize)->GetFunction());

}

NODE_MODULE(robotjs, init)
