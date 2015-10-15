#ifndef __DLMS_DEFINE_H__
#define __DLMS_DEFINE_H__

#include "dlmsClasses.h"
#include "dlmsError.h"
#include "../asn1/asn1Define.h"

#define DLMS_DEFAULT_RESPONSE_TIMEOUT           3000        // Default response timeout
#define DLMS_DEFAULT_FRAME_TIMEOUT              1500        // Frame timeout in milliseconds
#define DLMS_DEFAULT_HDLC_BUF_SIZE              512         // Default HDLC buffer size
#define DLMS_DEFAULT_COSEM_BUF_SIZE             65535       // COSEM buffer size

#define DLMS_PARAM_GROW                         20          // DLMS Parameter 증가 개수
#define DLMS_MAX_LOCAL_VARIABLE                 20          // 내부 파싱용 변수

#define DLMS_PROFILE_UNKNOWN                    0           // Unknown
#define DLMS_PROFILE_HDLC                       1           // HDLC
#define DLMS_PROFILE_MODE_E                     2           // Not support
#define DLMS_PROFILE_TCPIP                      3           // TCP/IP
#define DLMS_PROFILE_UDP                        4           // UDP

#define DLMS_DEFAULT_WINDOW_RX                  1           // Number of windows receive
#define DLMS_DEFAULT_WINDOW_TX                  1           // Number of windows transmit
#define DLMS_DEFAULT_INFO_LEN_RX                128         // Information field length receive
#define DLMS_DEFAULT_INFO_LEN_TX                128         // Information field length transmit

#define DLMS_CONTEXT_LN_NO_CIPHER               1           // LN Association with No ciphering
#define DLMS_CONTEXT_SN_NO_CIPHER               2           // SN Association with No ciphering
#define DLMS_CONTEXT_LN_CIPHER                  3           // LN Association with ciphering
#define DLMS_CONTEXT_SN_CIPHER                  4           // SN Association with ciphering

#define DLMS_AUTH_NONE                          0           // Lowest Level Security (without password)
#define DLMS_AUTH_STATIC_PASSWORD               1           // Low Level Security (with static password)
#define DLMS_AUTH_CODED_PASSWORD                2           // High Level Security (with coded password)

#define DLMSOBJ_UNKNOWN                         0
#define DLMSOBJ_DATA                            1
#define DLMSOBJ_ACTION                          2
#define DLMSOBJ_ACCESS                          3
#define DLMSOBJ_PARAM                           4

#define DLMS_CLIENT_IDENTIFIER                  0x12345601
#define DLMS_ACCESS_IDENTIFIER                  0x12345602
#define DLMS_ACTION_IDENTIFIER                  0x12345603
#define DLMS_DATA_IDENTIFIER                    0x12345604
#define DLMS_PARAM_IDENTIFIER                   0x12345605

#define DLMSOPT_RESPONSE_TIMEOUT                0x0001
#define DLMSOPT_FRAME_TIMEOUT                   0x0002
#define DLMSOPT_HDLC_BUFFER_SIZE                0x0004
#define DLMSOPT_COSEM_BUFFER_SIZE               0x0008

#define HDLC_FLAG                               0x7e
#define HDLC_FRAME_FORMAT                       0xa0        // 1010 0000    
#define HDLC_SEGMENTATION                       0x08        // Segmentation 

#define HDLC_FORMAT(x)                          (x >> 11)
#define HDLC_LENGTH(x)                          (x & 0x07FF)

#define HDLC_CTRL_I                             0x10        // Information transfer command and response
#define HDLC_CTRL_RR                            0x11        // Receive ready command and response
#define HDLC_CTRL_UI                            0x13        // Unnumbered information (UI) command and response
#define HDLC_CTRL_RNR                           0x15        // Receive not ready command and response
#define HDLC_CTRL_DM                            0x1f        // Disconnected mode (DM) response
#define HDLC_CTRL_AARE                          0x30        // AARE
#define HDLC_CTRL_SCRIPT                        0x32        // Script Table
#define HDLC_CTRL_DISC                          0x53        // Disconnect (DISC) command
#define HDLC_CTRL_UA                            0x73        // Unnumbered acknowledge (UA) response
#define HDLC_CTRL_SNRM                          0x93        // Set normal response mode (SNRM) command
#define HDLC_CTRL_FRMR                          0x97        // Frame reject (FRMR) response

#define LSAP_DEST_ADDRESS                       0xe6
#define LSAP_SRC_ADDRESS                        0xe6

#define DLMS_DATATYPE_NULL                      0            // NULL
#define DLMS_DATATYPE_ARRAY                     1            // Array
#define DLMS_DATATYPE_STRUCTURE                 2            // Structure
#define DLMS_DATATYPE_BOOLEAN                   3            // TRUE or FALSE
#define DLMS_DATATYPE_BIT_STRING                4            // An ordered sequence of boolean values
#define DLMS_DATATYPE_INT32                     5            // Integer32
#define DLMS_DATATYPE_UINT32                    6            // Unsigned32
#define DLMS_DATATYPE_OCTET_STRING              9            // An ordered sequence of octets (8 bit bytes)
#define DLMS_DATATYPE_VISIBLE_STRING            10           // An ordered sequence of ASCII characters
#define DLMS_DATATYPE_BCD                       13           // Binary coded decimal
#define DLMS_DATATYPE_INT8                      15           // Integer8
#define DLMS_DATATYPE_INT16                     16           // Integer16
#define DLMS_DATATYPE_UINT8                     17
#define DLMS_DATATYPE_UINT16                    18
#define DLMS_DATATYPE_COMPACT_ARRAY             19
#define DLMS_DATATYPE_INT64                     20
#define DLMS_DATATYPE_UINT64                    21
#define DLMS_DATATYPE_ENUM                      22
#define DLMS_DATATYPE_FLOAT32                   23
#define DLMS_DATATYPE_DATE_TIME                 25
#define DLMS_DATATYPE_DATE                      26
#define DLMS_DATATYPE_TIME                      27
#define DLMS_DATATYPE_GROUP                     0x80

// xDLMS APDUs used with LN referencing
// with no ciphering

#define DLMS_GET_REQUEST                        0xc0
#define DLMS_SET_REQUEST                        0xc1
#define DLMS_EVENT_NOTIFICATION                 0xc2
#define DLMS_ACTION_REQUEST                     0xc3
#define DLMS_GET_RESPONSE                       0xc4
#define DLMS_SET_RESPONSE                       0xc5
#define DLMS_ACTION_RESPONSE                    0xc7

// with global ciphering

#define DLMS_GLO_GET_REQUEST                    0xc8
#define DLMS_GLO_SET_REQUEST                    0xc9
#define DLMS_GLO_EVENT_NOTIFICATION             0xca
#define DLMS_GLO_ACTION_REQUEST                 0xcb
#define DLMS_GLO_GET_RESPONSE                   0xcc
#define DLMS_GLO_SET_RESPONSE                   0xcd
#define DLMS_GLO_ACTION_RESPONSE                0xcf

// with dedicated ciphering

#define DLMS_DED_GET_REQUEST                    0xd0
#define DLMS_DED_SET_REQUEST                    0xd1
#define DLMS_DED_EVENT_NOTIFICATION             0xd2
#define DLMS_DED_ACTION_REQUEST                 0xd3
#define DLMS_DED_GET_RESPONSE                   0xd4
#define DLMS_DED_SET_RESPONSE                   0xd5
#define DLMS_DED_ACTION_RESPONSE                0xd7

// the exception response pdu

#define DLMS_EXCEPTION_RESPONSE                 0xd8

#define DLMS_GET                                0xc0
#define DLMS_SET                                0xc1
#define DLMS_ACTION                             0xc2
#define DLMS_METHOD_MASK                        0xc3

#define DLMS_REQUEST                            0x00
#define DLMS_RESPONSE                           0x04

#define DLMS_NORMAL                             0x01
#define DLMS_DATABLOCK                          0x02
#define DLMS_LIST                               0x03

#define DLMS_INVOKE_ID                          0x80
#define DLMS_INVOKE_PRIORITY                    0x01

// Modem Protocol

#define PMPFLOW_REQUEST                         0x00            // Request
#define PMPFLOW_RESPONSE                        0x80

#define PMPACTION_READ                          0x00
#define PMPACTION_WRITE                         0x40

#define PMPTYPE_COMMAND                         0x01
#define PMPTYPE_DATA                            0x02
#define PMPTYPE_LOOPBACK                        0x03
#define PMPTYPE_EVENT                           0x04

#define PMPCMD_TYPE                             0x01            // Modem type
#define PMPCMD_VERSION                          0x02            // Modem version
#define PMPCMD_MANUFACTURER                     0x03            // Manufacturer ID
#define PMPCMD_CHIPID                           0x04            // Modem chip id/model
#define PMPCMD_ENCRIPTION                       0x05            // Encription type
#define PMPCMD_BAUDRATE                         0x20            // Modem serial speed
#define PMPCMD_ALB                              0x80            // Analog loop back
#define PMPCMD_DLB                              0x81            // Digital loop back

/** DLMS Response Code */
#define DLMS_RES_SUCCESS                        0
#define DLMS_RES_HARDWARE_FAULT                 1
#define DLMS_RES_TEMPORARY_FAILURE              2
#define DLMS_RES_READ_WRITE_DENIED              3
#define DLMS_RES_OBJECT_UNDEFINED               4
#define DLMS_RES_OBJECT_CLASS_INCONSISTENT      9
#define DLMS_RES_OBJECT_UNAVAILABLE             11
#define DLMS_RES_TYPE_UNMATCHED                 12
#define DLMS_RES_SCOPE_OF_ACCESS_VIOLATED       13
#define DLMS_RES_DATA_BLOCK_UNAVAILABLE         14
#define DLMS_RES_LONG_GET_ABORTED               15
#define DLMS_RES_NO_LONG_GET_IN_PROGRESS        16
#define DLMS_RES_LONG_SET_ABORTED               17
#define DLMS_RES_NO_LONG_SET_IN_PROGRESS        18
#define DLMS_RES_OTHER_REASON                   250

/** Proposed Conformance (3bytes BIG Endian)
  * Little endian system에서는 변환시킨 후 bit 연산을 해야 한다.
  */
#define CONF_LN_ATTR0_SUPPORTED_SET             0x00008000      // attribute0-supported-with-set
#define CONF_LN_PRORITY_MGMT_SUPPORTED          0x00004000      // priority-mgmt-supported
#define CONF_LN_ATTR0_SUPPORTED_GET             0x00002000      // attribute0-supported-with-get
#define CONF_LN_BLOCK_TRANSFER_GET              0x00001000      // block-transfer-with-get
#define CONF_LN_BLOCK_TRANSFER_SET              0x00000800      // block-transfer-with-set
#define CONF_LN_BLOCK_TRANSFER_ACTION           0x00000400      // block-transfer-with-action
#define CONF_LN_MULTIPLE_REFERENCES             0x00000200      // multiple-references
#define CONF_LN_GET                             0x00000010      // get
#define CONF_LN_SET                             0x00000008      // set
#define CONF_LN_SELECTIVE_ACCESS                0x00000004      // selective-access
#define CONF_LN_EVENT_NOTIFICATION              0x00000002      // event-notification
#define CONF_LN_ACTION                          0x00000001      // action

#define CONF_SN_READ                            0x00100000      // read
#define CONF_SN_WRITE                           0x00080000      // write
#define CONF_SN_UNCONFIRMED_WRITE               0x00040000      // unconfirmed-write
#define CONF_SN_MULTIPLE_REFERENCES             0x00000200      // multiple-references
#define CONF_SN_INFORMATION_REPORT              0x00000100      // information-report
#define CONF_SN_PARAMETERIZED_ACCESS            0x00000020      // parameterized-access

/** AARQ application context name */
#define DLMS_ASSOCIATION_LN                     0x00
#define DLMS_ASSOCIATION_SN                     0x01

#define DLMS_OID_LN     { 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01 }    // LN 2.16.756.5.8.1.1
#define DLMS_OID_SN     { 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x02 }    // LN 2.16.756.5.8.1.2

/** AARQ mechanism name */
// default-COSEM-low-level-security-mechanism-name 2.16.756.5.8.2.1
#define DLMS_DEFAULT_SECURITY_MACHANISM     { 0x60, 0x85, 0x74, 0x05, 0x08, 0x02, 0x01 }

/** Authentication value */
typedef enum { DLMS_AUTH_VALUE_GRAPHICSTRING = ASN_CLASS_CONTEXT | 0x00 } DLMS_AUTH_VALUE;

#endif    // __DLMS_DEFINE_H__

