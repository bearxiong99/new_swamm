#ifndef __DEMAND_RESPONSE_DEF_H__
#define __DEMAND_RESPONSE_DEF_H__

#define ENERGY_LEVEL_NOT_SUPPORT        0

/** level 값이 유효한지 검사 */
#define ISVALID_ENERGY_LEVEL(level)     (level>=0x01&&level<=0xF)

#endif	// __DEMAND_RESPONSE_DEF_H__

