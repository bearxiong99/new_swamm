#ifndef __ASN1_DEFINE_H__
#define __ASN1_DEFINE_H__


/** ASN.1 Class */
#define ASN_CLASS_MASK              0xC0        // Class mask

#define ASN_CLASS_UNIVERSAL         0x00        
#define ASN_CLASS_APPLICATION       0x40
#define ASN_CLASS_CONTEXT           0x80
#define ASN_CLASS_PRIVATE           0xC0

/** ASN.1 Primitive */
#define ASN_PRIMITIVE_MASK          0x20        // primitive/constructed mask

#define ASN_PRIMITIVE               0x00
#define ASN_CONSTRUCTED             0x20

/** ASN.1 Universal Class Type */
#define ASN_TYPE_MASK               0x1F        // Type mask

#define ASN_TYPE_EOC                0x00        // P (End of Content)
#define ASN_TYPE_BOOLEAN            0x01        // P
#define ASN_TYPE_INTEGER            0x02        // P
#define ASN_TYPE_BIT_STRING         0x03        // P,C
#define ASN_TYPE_OCTET_STRING       0x04        // P,C
#define ASN_TYPE_NULL               0x05        // P
#define ASN_TYPE_OID                0x06        // P
#define ASN_TYPE_OBJ_DESC           0x07        // P
#define ASN_TYPE_EXTERNAL           0x08        // C
#define ASN_TYPE_REAL               0x09        // P
#define ASN_TYPE_ENUMERATED         0x0A        // P
#define ASN_TYPE_EMBEDDED_PDV       0x0B        // C
#define ASN_TYPE_UTF8_STRING        0x0C        // P,C
#define ASN_TYPE_REALTIVE_OID       0x0D        // P
#define ASN_TYPE_SEQUENCE           0x10        // C
#define ASN_TYPE_SET                0x11        // C
#define ASN_TYPE_NUMERIC_STRING     0x12        // P,C
#define ASN_TYPE_PRINTABLE_STRING   0x13        // P,C
#define ASN_TYPE_T61_STRING         0x14        // P,C
#define ASN_TYPE_VIDEOTEX_STRING    0x15        // P,C
#define ASN_TYPE_IA5_STRING         0x16        // P,C
#define ASN_TYPE_UTC_TIME           0x17        // P,C
#define ASN_TYPE_GENERALIZED_TIME   0x18        // P,C
#define ASN_TYPE_GRAPHIC_STRING     0x19        // P,C
#define ASN_TYPE_VISIBLE_STRING     0x1A        // P,C
#define ASN_TYPE_GENERAL_STRING     0x1B        // P,C
#define ASN_TYPE_UNIVERSAL_STRING   0x1C        // P,C
#define ASN_TYPE_CHARACTER_STRING   0x1D        // P,C
#define ASN_TYPE_BMP_STRING         0x1E        // P,C

#endif	// __ASN1_DEFINE_H__

