/*
	RC_ChannelB.cpp - Radio library for Arduino
	Code by Jason Short, James Goppert. DIYDrones.com
	
	This library is free software; you can redistribute it and / or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

*/

#include <math.h>
#include <avr/eeprom.h>
#include "RC_ChannelB.h"
#include <AP_Common.h>

void RC_ChannelB::readRadio(uint16_t pwmRadio) {
	// apply reverse
	if(_reverse) _pwmRadio = (_pwmNeutral - pwmRadio) + _pwmNeutral;
	else _pwmRadio = pwmRadio;

	setPwm(pwmRadio);
}

void
RC_ChannelB::setPwm(uint16_t pwm)
{
	// apply reverse
	if(_reverse) pwm = (_pwmNeutral-pwm) + _pwmNeutral;

	// apply filter
	if(_filter){
		if(_pwm == 0)
			_pwm = pwm;
		else
			_pwm = ((pwm + _pwm) >> 1);		// Small filtering
	}else{
		_pwm = pwm;
	}

	// apply deadzone
	_pwm = (abs(_pwm - _pwmNeutral) < _pwmDeadZone) ? _pwmNeutral : _pwm;
}

void
RC_ChannelB::setPosition(float position)
{
	setPwm(_positionToPwm(position));
}

void
RC_ChannelB::mixRadio(uint16_t infStart)
{
	float inf = abs(_pwmRadio - _pwmNeutral);
	inf = min(inf, infStart);
	inf = ((infStart - inf) /infStart);
	setPwm(_pwm*inf + _pwmRadio); 
}

uint16_t
RC_ChannelB::_positionToPwm(float position)
{
	if(position < 0)
		return (position / _scale) * (_pwmMin - _pwmNeutral);
	else
		return (position / _scale) * (_pwmMax - _pwmNeutral);
}

float
RC_ChannelB::_pwmToPosition(uint16_t pwm)
{
	if(_pwm < _pwmNeutral)
		return _scale * (_pwm - _pwmNeutral)/(_pwmNeutral - _pwmMin);
	else
		return _scale * (_pwm - _pwmNeutral)/(_pwmMax - _pwmNeutral);
}

// ------------------------------------------