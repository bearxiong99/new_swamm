//////////////////////////////////////////////////////////////////////
//
//	units.h: AIMIR MCU  Unit definition
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2012 NURITELECOM Telecom Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __VALUE_UNITS_H__
#define __VALUE_UNITS_H__

#define UNIT_TYPE_UNDEFINED             0x00
#define UNIT_TYPE_YEAR                  0x01
#define UNIT_TYPE_MONTH                 0x02
#define UNIT_TYPE_WEEK                  0x03
#define UNIT_TYPE_DAY                   0x04
#define UNIT_TYPE_HOUR                  0x05
#define UNIT_TYPE_MINUTE                0x06
#define UNIT_TYPE_SECOND                0x07
#define UNIT_TYPE_ANGLE                 0x08
#define UNIT_TYPE_TEMPERATURE           0x09
#define UNIT_TYPE_CURRENCY              0x0A
#define UNIT_TYPE_METER                 0x0B
#define UNIT_TYPE_METER_SEC             0x0C
#define UNIT_TYPE_CUBICMETER            0x0D
#define UNIT_TYPE_C_CUBICMETER          0x0E    // corrected volume
#define UNIT_TYPE_CUBICMETER_HOUR       0x0F
#define UNIT_TYPE_C_CUBICMETER_HOUR     0x10    // corrected volume flux
#define UNIT_TYPE_CUBICMETER_DAY        0x11
#define UNIT_TYPE_C_CUBICMETER_DAY      0x12    // corrected volume flux
#define UNIT_TYPE_LITER                 0x13
#define UNIT_TYPE_KILOGRAM              0x14
#define UNIT_TYPE_NEWTON                0x15
#define UNIT_TYPE_NEWTONMETER           0x16
#define UNIT_TYPE_PASCAL                0x17
#define UNIT_TYPE_BAR                   0x18
#define UNIT_TYPE_JOULE                 0x19
#define UNIT_TYPE_JOULE_HOUR            0x1A
#define UNIT_TYPE_WATT                  0x1B
#define UNIT_TYPE_VOLTAMPERE            0x1C
#define UNIT_TYPE_VOLTAMPEREREACTIVE    0x1D
#define UNIT_TYPE_WATT_HOUR             0x1E
#define UNIT_TYPE_VOLTAMPERE_HOUR       0x1F
#define UNIT_TYPE_VOLTAMPEREREACTIVE_HOUR   0x20
#define UNIT_TYPE_AMPERE                0x21
#define UNIT_TYPE_COULOMB               0x22
#define UNIT_TYPE_VOLT                  0x23
#define UNIT_TYPE_VOLT_METER            0x24
#define UNIT_TYPE_FARAD                 0x25
#define UNIT_TYPE_OHMER                 0x26
#define UNIT_TYPE_OHMER_METER           0x27
#define UNIT_TYPE_WEBER                 0x28
#define UNIT_TYPE_TESLA                 0x29
#define UNIT_TYPE_AMPERE_METER          0x2A
#define UNIT_TYPE_HENRY                 0x2B
#define UNIT_TYPE_HERTZ                 0x2C
#define UNIT_TYPE_R_ACTIVE              0x2D
#define UNIT_TYPE_R_REACTIVE            0x2E
#define UNIT_TYPE_R_APPARENT            0x2F
#define UNIT_TYPE_VOLTSQUARE_HOUR       0x30
#define UNIT_TYPE_AMPERESQUARE_HOUR     0x31
#define UNIT_TYPE_KILOGRAM_SEC          0x32
#define UNIT_TYPE_SIEMENS               0x33
#define UNIT_TYPE_KELVIN                0x34
#define UNIT_TYPE_R_VOLTSQUARE_HOUR     0x35
#define UNIT_TYPE_R_AMPERESQUARE_HOUR   0x36
#define UNIT_TYPE_R_CUBICMETER          0x37
#define UNIT_TYPE_PERCENT               0x38
#define UNIT_TYPE_AMPERE_HOUR           0x39
#define UNIT_TYPE_WATTHOUR_CUBICMETER   0x3C
#define UNIT_TYPE_JOULE_CUBICMETER      0x3D
#define UNIT_TYPE_MOL_PERCENT           0x3E
#define UNIT_TYPE_GRAM_CUBICMETER       0x3F
#define UNIT_TYPE_PASCAL_SECOND         0x40
#define UNIT_TYPE_OTHER                 0xFE
#define UNIT_TYPE_COUNT                 0xFF

#endif 	// __VALUE_UNITS_H__
