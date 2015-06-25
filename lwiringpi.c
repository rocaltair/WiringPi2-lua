#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>

#if LUA_VERSION_NUM < 502
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

struct lwiringpi_macro_t{
	const char *name;
	int value;
};

#define ARRAY_LINE(KEY)	{ #KEY , KEY }

static const struct lwiringpi_macro_t macros[] = {
		// Deprecated
		ARRAY_LINE(NUM_PINS),

		ARRAY_LINE(WPI_MODE_PINS),
		ARRAY_LINE(WPI_MODE_GPIO),
		ARRAY_LINE(WPI_MODE_GPIO_SYS),
		ARRAY_LINE(WPI_MODE_PHYS),
		ARRAY_LINE(WPI_MODE_PIFACE),
		ARRAY_LINE(WPI_MODE_UNINITIALISED),

		// Pin modes

		ARRAY_LINE(INPUT),
		ARRAY_LINE(OUTPUT),
		ARRAY_LINE(PWM_OUTPUT),
		ARRAY_LINE(GPIO_CLOCK),
		ARRAY_LINE(SOFT_PWM_OUTPUT),
		ARRAY_LINE(SOFT_TONE_OUTPUT),
		ARRAY_LINE(PWM_TONE_OUTPUT),

		ARRAY_LINE(LOW),
		ARRAY_LINE(HIGH),

		// Pull up/down/none

		ARRAY_LINE(PUD_OFF),
		ARRAY_LINE(PUD_DOWN),
		ARRAY_LINE(PUD_UP),

		// PWM

		ARRAY_LINE(PWM_MODE_MS),
		ARRAY_LINE(PWM_MODE_BAL),

		// Interrupt levels

		ARRAY_LINE(INT_EDGE_SETUP),
		ARRAY_LINE(INT_EDGE_FALLING),
		ARRAY_LINE(INT_EDGE_RISING),
		ARRAY_LINE(INT_EDGE_BOTH),

		// Pi model types and version numbers
		//	Intended for the GPIO program Use at your own risk.

		ARRAY_LINE(PI_MODEL_UNKNOWN),
		ARRAY_LINE(PI_MODEL_A),
		ARRAY_LINE(PI_MODEL_B),
		ARRAY_LINE(PI_MODEL_BP),
		ARRAY_LINE(PI_MODEL_CM),
		ARRAY_LINE(PI_MODEL_AP),
		ARRAY_LINE(PI_MODEL_2),

		ARRAY_LINE(PI_VERSION_UNKNOWN),
		ARRAY_LINE(PI_VERSION_1),
		ARRAY_LINE(PI_VERSION_1_1),
		ARRAY_LINE(PI_VERSION_1_2),
		ARRAY_LINE(PI_VERSION_2),

		ARRAY_LINE(PI_MAKER_UNKNOWN),
		ARRAY_LINE(PI_MAKER_EGOMAN),
		ARRAY_LINE(PI_MAKER_SONY),
		ARRAY_LINE(PI_MAKER_QISDA),
		ARRAY_LINE(PI_MAKER_MBEST),
		{NULL, 0},
};

// Core wiringPi functions
static int lua__wiringPiSetup(lua_State *L)
{
	int ret = wiringPiSetup();
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__wiringPiSetupSys(lua_State *L)
{
	int ret = wiringPiSetupSys();
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__wiringPiSetupGpio(lua_State *L)
{
	int ret = wiringPiSetupGpio();
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__wiringPiSetupPhys(lua_State *L)
{
	int ret = wiringPiSetupPhys();
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__pinModeAlt(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int mode = luaL_checkinteger(L, 2);
	pinModeAlt(pin, mode);
	return 0;
}

static int lua__pinMode(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int mode = luaL_checkinteger(L, 2);
	pinMode(pin, mode);
	return 0;
}

static int lua__pullUpDnControl(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int pud = luaL_checkinteger(L, 2);
	pullUpDnControl(pin, pud);
	return 0;
}

static int lua__digitalRead(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int ret = digitalRead(pin);
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__digitalWrite(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int value = luaL_checkinteger(L, 2);
	digitalWrite(pin, value);
	return 0;
}

static int lua__pwmWrite(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int value = luaL_checkinteger(L, 2);
	pwmWrite(pin, value);
	return 0;
}

static int lua__analogRead(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int ret = analogRead(pin);
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__analogWrite(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int value = luaL_checkinteger(L, 2);
	analogWrite(pin, value);
	return 0;
}


// PiFace specifics(Deprecated)

/*
static int lua__wiringPiSetupPiFace(lua_State *L)
{
	int ret = wiringPiSetupPiFace();
	lua_pushinteger(L, ret);
	return 1;
}
*/

// On-Board Raspberry Pi hardware specific stuff

static int lua__piBoardRev(lua_State *L)
{
	int ret = piBoardRev();
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__piBoardId(lua_State *L)
{
	int model, rev, mem, maker, overVolted;
	piBoardId(&model, &rev, &mem, &maker, &overVolted);
	lua_newtable(L);

	lua_pushstring(L, "model");
	lua_pushstring(L, piModelNames[model]);
	lua_settable(L, -3);

	lua_pushstring(L, "rev");
	lua_pushstring(L, piRevisionNames[rev]);
	lua_settable(L, -3);

	lua_pushstring(L, "mem");
	lua_pushnumber(L, mem);
	lua_settable(L, -3);

	lua_pushstring(L, "maker");
	lua_pushstring(L, piMakerNames[maker]);
	lua_settable(L, -3);

	lua_pushstring(L, "overVolted");
	lua_pushnumber(L, overVolted);
	lua_settable(L, -3);
	return 1;
}

static int lua__wpiPinToGpio(lua_State *L)
{
	int wpiPin = luaL_checkinteger(L, 1);
	int ret = wpiPinToGpio(wpiPin);
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__physPinToGpio(lua_State *L)
{
	int physPin = luaL_checkinteger(L, 1);
	int ret = physPinToGpio(physPin);
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__setPadDrive(lua_State *L)
{
	int group = luaL_checkinteger(L, 1);
	int value = luaL_checkinteger(L, 2);
	setPadDrive(group, value);
	return 0;
}

static int lua__getAlt(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int ret = getAlt(pin);
	lua_pushinteger(L, ret);
	return 1;
}

static int lua__pwmToneWrite(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int freq = luaL_checkinteger(L, 2);
	pwmToneWrite(pin, freq);
	return 0;
}

static int lua__digitalWriteByte(lua_State *L)
{
	int value = luaL_checkinteger(L, 1);
	digitalWriteByte(value);
	return 0;
}

static int lua__pwmSetMode(lua_State *L)
{
	int mode = luaL_checkinteger(L, 1);
	pwmSetMode(mode);
	return 0;
}

static int lua__pwmSetRange(lua_State *L)
{
	double range = luaL_checknumber(L, 1);
	pwmSetRange((unsigned int)range);
	return 0;
}

static int lua__pwmSetClock(lua_State *L)
{
	int divisor = luaL_checkinteger(L, 1);
	pwmSetClock(divisor);
	return 0;
}


static int lua__gpioClockSet(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int freq = luaL_checkinteger(L, 2);
	gpioClockSet(pin, freq);
	return 0;
}


// Interrupts
//(Also Pi hardware specific)

static int lua__waitForInterrupt(lua_State *L)
{
	int pin = luaL_checkinteger(L, 1);
	int mS = luaL_checkinteger(L, 2);
	waitForInterrupt(pin, mS);
	return 0;
}

// TODO 
// int wiringPiISR(int pin, int mode, void(*function)());

// Threads

/* TODO
int piThreadCreate(void *(*fn)(void *));

static int lua__piLock(lua_State *L)
{
	int key = luaL_checkinteger(L, 1);
	piLock(key);
	return 0;
}

static int lua__piUnlock(lua_State *L)
{
	int key = luaL_checkinteger(L, 1);
	piUnlock(key);
	return 0;
}
*/

// Schedulling priority

static int lua__piHiPri(lua_State *L)
{
	int pri = luaL_checkinteger(L, 1);
	int ret = piHiPri(pri);
	lua_pushinteger(L, ret);
	return 1;
}

// Extras from arduino land

static int lua__delay(lua_State *L)
{
	lua_Number howLong = luaL_checknumber(L, 1);
	delay((unsigned int)howLong);
	return 0;
}

static int lua__delayMicroseconds(lua_State *L)
{
	lua_Number howLong = luaL_checknumber(L, 1);
	delayMicroseconds((unsigned int)howLong);
	return 0;
}

static int lua__millis(lua_State *L)
{
	unsigned int ret = millis();
	lua_pushnumber(L, (lua_Number)ret);
	return 1;
}

static int lua__micros(lua_State *L)
{
	unsigned int ret = micros();
	lua_pushnumber(L, (lua_Number)ret);
	return 1;
}

int luaopen_lwiringpi(lua_State* L)
{
	int i = 0;
	luaL_Reg lfuncs[] = {
		{"wiringPiSetup", lua__wiringPiSetup},
		{"wiringPiSetupSys", lua__wiringPiSetupSys},
		{"wiringPiSetupGpio", lua__wiringPiSetupGpio},
		{"wiringPiSetupPhys", lua__wiringPiSetupPhys},
		{"pinModeAlt", lua__pinModeAlt},
		{"pinMode", lua__pinMode},
		{"pullUpDnControl", lua__pullUpDnControl},
		{"digitalWrite", lua__digitalWrite},
		{"pwmWrite", lua__pwmWrite},
		{"analogWrite", lua__analogWrite},
		// {"wiringPiSetupPiFace", lua__wiringPiSetupPiFace},
		{"piBoardRev", lua__piBoardRev},
		{"setPadDrive", lua__setPadDrive},
		{"pwmToneWrite", lua__pwmToneWrite},
		{"gpioClockSet", lua__gpioClockSet},
		{"waitForInterrupt", lua__waitForInterrupt},
		{"millis", lua__millis},
		{"micros", lua__micros},
		{"digitalRead", lua__digitalRead},
		{"analogRead", lua__analogRead},
		{"wpiPinToGpio", lua__wpiPinToGpio},
		{"physPinToGpio", lua__physPinToGpio},
		{"getAlt", lua__getAlt},
		{"digitalWriteByte", lua__digitalWriteByte},
		{"pwmSetMode", lua__pwmSetMode},
		{"pwmSetRange", lua__pwmSetRange},
		{"pwmSetClock", lua__pwmSetClock},
		/*
		{"piLock", lua__piLock},
		{"piUnlock", lua__piUnlock},
		*/
		{"piHiPri", lua__piHiPri},
		{"delayMicroseconds", lua__delayMicroseconds},
		{"delay", lua__delay},
		{"piBoardId", lua__piBoardId},
		{NULL, NULL},
	};
	luaL_newlib(L, lfuncs);
	for (;;) {
		const char *name = macros[i].name;
		int value = macros[i].value;
		i++;
		if (name == NULL) {
			break;
		}
		lua_pushstring(L, name);
		lua_pushinteger(L, value);
		lua_settable(L, -3);
	}
	return 1;
}
