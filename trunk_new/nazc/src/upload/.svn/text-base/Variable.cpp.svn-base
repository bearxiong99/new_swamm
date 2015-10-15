#include "common.h"
#include "varapi.h"
#include "Variable.h"

VAROBJECT	m_Root_node[] =
{
	{   1,	"smi", 			{ {1,0,0}, VARSMI_OID, 3 },          VARATTR_RW, m_smi_node, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_smi_node[] =
{
	{   1,	"smiEntry", 	{ {1,1,0}, VARSMI_OID, 3 },          VARATTR_RW, NULL, NULL, NULL },
	{   2,	"unknownEntry", { {1,2,0}, VARSMI_BYTE, 0 },         VARATTR_RW, NULL, NULL, NULL },
	{   3,	"boolEntry", 	{ {1,3,0}, VARSMI_BOOL, 1 },         VARATTR_RW, NULL, NULL, NULL },
	{   4,	"byteEntry", 	{ {1,4,0}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, NULL },
	{   5,	"wordEntry", 	{ {1,5,0}, VARSMI_WORD, 2 },         VARATTR_RW, NULL, NULL, NULL },
	{   6,	"uintEntry", 	{ {1,6,0}, VARSMI_UINT, 4 },         VARATTR_RW, NULL, NULL, NULL },
	{   7,	"charEntry", 	{ {1,7,0}, VARSMI_CHAR, 1 },         VARATTR_RW, NULL, NULL, NULL },
	{   8,	"shortEntry", 	{ {1,8,0}, VARSMI_SHORT, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   9,	"intEntry", 	{ {1,9,0}, VARSMI_INT, 4 },          VARATTR_RW, NULL, NULL, NULL },
	{  10,	"oidEntry", 	{ {1,10,0}, VARSMI_OID, 3 },         VARATTR_RW, m_oidEntry_node, NULL, NULL },
	{  11,	"stringEntry", 	{ {1,11,0}, VARSMI_STRING, 0 },      VARATTR_RW, NULL, NULL, NULL },
	{  12,	"streamEntry", 	{ {1,12,0}, VARSMI_STREAM, 0 },      VARATTR_RW, NULL, NULL, NULL },
	{  13,	"opaqueEntry", 	{ {1,13,0}, VARSMI_OPAQUE, 0 },      VARATTR_RW, NULL, NULL, NULL },
	{  14,	"eui64Entry", 	{ {1,14,0}, VARSMI_EUI64, 8 },       VARATTR_RW, m_eui64Entry_node, NULL, NULL },
	{  15,	"ipEntry", 		{ {1,15,0}, VARSMI_IPADDR, 4 },      VARATTR_RW, m_ipEntry_node, NULL, NULL },
	{  16,	"timeEntry", 	{ {1,16,0}, VARSMI_TIMESTAMP, 7 },   VARATTR_RW, m_timeEntry_node, NULL, NULL },
	{  17,	"dayEntry", 	{ {1,17,0}, VARSMI_TIMEDATE, 4 },    VARATTR_RW, m_dayEntry_node, NULL, NULL },
	{  18,	"verEntry", 	{ {1,18,0}, VARSMI_STRING, 16 },     VARATTR_RW, m_verEntry_node, NULL, NULL },
	{  19,	"smivarEntry", 	{ {1,19,0}, VARSMI_STREAM, 0 },      VARATTR_RW, m_smivarEntry_node, NULL, NULL },
	{  20,	"pageEntry", 	{ {1,20,0}, VARSMI_STREAM, 0 },      VARATTR_RW, m_pageEntry_node, NULL, NULL },
	{  21,	"gmtEntry", 	{ {1,21,0}, VARSMI_STREAM, 11 },     VARATTR_RW, m_gmtEntry_node, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_oidEntry_node[] =
{
	{   1,	"oidDot1", 		{ {1,10,1}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"oidDot2", 		{ {1,10,2}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"oidDot3", 		{ {1,10,3}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eui64Entry_node[] =
{
	{   1,	"eui64High", 	{ {1,14,1}, VARSMI_UINT, 4 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"eui64Low", 	{ {1,14,2}, VARSMI_UINT, 4 },        VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_ipEntry_node[] =
{
	{   1,	"ipDot1", 		{ {1,15,1}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"ipDot2", 		{ {1,15,2}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"ipDot3", 		{ {1,15,3}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   4,	"ipDot4", 		{ {1,15,4}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_timeEntry_node[] =
{
	{   1,	"timeYear", 	{ {1,16,1}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"timeMon", 		{ {1,16,2}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"timeDay", 		{ {1,16,3}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   4,	"timeHour", 	{ {1,16,4}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   5,	"timeMin", 		{ {1,16,5}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   6,	"timeSec", 		{ {1,16,6}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_dayEntry_node[] =
{
	{   1,	"dayYear", 		{ {1,17,1}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"dayMon", 		{ {1,17,2}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"dayDay", 		{ {1,17,3}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_verEntry_node[] =
{
	{   1,	"verPrefix", 	{ {1,18,1}, VARSMI_STRING, 3 },      VARATTR_RW, NULL, NULL, NULL },
	{   2,	"verFixed1", 	{ {1,18,2}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"verHwVersion", { {1,18,3}, VARSMI_STRING, 2 },      VARATTR_RW, NULL, NULL, NULL },
	{   4,	"verFixed2", 	{ {1,18,4}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   5,	"verSwVersion", { {1,18,5}, VARSMI_STRING, 4 },      VARATTR_RW, NULL, NULL, NULL },
	{   6,	"verFixed3", 	{ {1,18,6}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   7,	"verService", 	{ {1,18,7}, VARSMI_STRING, 4 },      VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_smivarEntry_node[] =
{
	{   1,	"smivarOid", 	{ {1,19,1}, VARSMI_OID, 3 },         VARATTR_RW, NULL, NULL, NULL },
	{   2,	"smivarLength", { {1,19,2}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"smivarData", 	{ {1,19,3}, VARSMI_STREAM, 0 },      VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_pageEntry_node[] =
{
	{   1,	"pageNumber", 	{ {1,20,1}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"pageLength", 	{ {1,20,2}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"pageData", 	{ {1,20,3}, VARSMI_STREAM, 0 },      VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_gmtEntry_node[] =
{
	{   1,	"gmtTimezone", 	{ {1,21,1}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"gmtDstValue", 	{ {1,21,2}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"gmtYear", 		{ {1,21,3}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   4,	"gmtMon", 		{ {1,21,4}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   5,	"gmtDay", 		{ {1,21,5}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   6,	"gmtHour", 		{ {1,21,6}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   7,	"gmtMin", 		{ {1,21,7}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   8,	"gmtSec", 		{ {1,21,8}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

