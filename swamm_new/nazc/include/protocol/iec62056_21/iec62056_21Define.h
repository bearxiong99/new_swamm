#ifndef __IEC62056_21_DEFINE_H__
#define __IEC62056_21_DEFINE_H__

#define IEC_21_START                0x2F        /*      /       */
#define IEC_21_END                  0x21        /*      !       */
#define IEC_21_REQUEST              0x3F        /*      ?       */
#define IEC_21_CR                   0x0D        /*      CR      */
#define IEC_21_LF                   0x0A        /*      LF      */

/** Protocol Mode A Baud rate identification */
#define IEC_21_BAUD_A_ANY           0x5A        /*      Z       */
/** Protocol Mode B Baud rate identification */
#define IEC_21_BAUD_B_600           0x41        /*      A       */
#define IEC_21_BAUD_B_1200          0x42        /*      B       */
#define IEC_21_BAUD_B_2400          0x43        /*      C       */
#define IEC_21_BAUD_B_4800          0x44        /*      D       */
#define IEC_21_BAUD_B_9600          0x45        /*      E       */
#define IEC_21_BAUD_B_19200         0x46        /*      F       */
/** Protocol Mode C/E Baud rate identification */
#define IEC_21_BAUD_CE_300          0x30        /*      0       */
#define IEC_21_BAUD_CE_600          0x31        /*      1       */
#define IEC_21_BAUD_CE_1200         0x32        /*      2       */
#define IEC_21_BAUD_CE_2400         0x33        /*      3       */
#define IEC_21_BAUD_CE_4800         0x34        /*      4       */
#define IEC_21_BAUD_CE_9600         0x35        /*      5       */
#define IEC_21_BAUD_CE_19200        0x36        /*      6       */
/** Protocol Mode D Baud rate identification */
#define IEC_21_BAUD_D_2400          0x33        /*      3       */


#define IEC_21_SOH                  0x01
#define IEC_21_STX                  0x02
#define IEC_21_ETX                  0x03
#define IEC_21_ACK                  0x06
#define IEC_21_NAK                  0x15

#define IEC_21_CMD_PASSWORD         0x50        /*      P       */
#define IEC_21_CMD_WRITE            0x57        /*      W       */
#define IEC_21_CMD_READ             0x52        /*      R       */
#define IEC_21_CMD_EXECUTE          0x45        /*      E       */
#define IEC_21_CMD_BREAK            0x42        /*      B       */

#define IEC_21_FRONT                0x50        /*      (       */
#define IEC_21_REAR                 0x51        /*      )       */


#endif	// __IEC62056_21_DEFINE_H__

