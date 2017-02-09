/* 
    Copyright (c) Bruno Ferreira

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/*-------------------------------- Driver Name ------------------------------*/
/** If you are using a revision 0002 or 0003 model B */
#define I2C_0 "/dev/i2c-0"

/** If you are Rasp 2 */
#define I2C_1 "/dev/i2c-1"

/*-----------------------------  Types definitions -----------------------------*/
/** PI framework types: BOOL */
#define BOOL	bool

/** PI framework types: 8 bits signed */
#define INT8	signed char

/** PI framework types: 16 bits signed */
#define INT16	signed short

/** PI framework types: 32 bits signed */
#define INT32	int
#define LONG	INT32

/** PI framework types: 64 bits signed */
#define INT64	long long

/** PI framework types: 8 bits unsigned */
#define UINT8	unsigned char
#define BYTE 	UINT8
#define UTINY	BYTE

/** PI framework types: 16 bits unsigned */
#define UINT16	unsigned short
#define WORD 	UINT16
#define BITS16	WORD

/** PI framework types: 32 bits unsigned */
#define UINT32	unsigned int
#define DWORD	UINT32
#define BITS32	DWORD

/** PI framework types: float 32 bits */
#define FLOAT32 float

/** PI framework types: signed char */
#define CHAR	char

/*--------------------------------- Bits Used -------------------------------*/
#define B0			0x1                 /* bit 0 */
#define B1			0x2                 /* bit 1 */
#define B2			0x4                 /* bit 2 */
#define B3			0x8                 /* bit 3 */
#define B4			0x10                /* bit 4 */
#define B5			0x20                /* bit 5 */
#define B6			0x40                /* bit 6 */
#define B7			0x80                /* bit 7 */

/*-----------------------------  Manipulate Bits ----------------------------*/
/******************************************************************************
 ******************     @CAUTION: DONOT change them...   **********************
 *****************************************************************************/ 
/** This macro indicates if any bit(s) of \a l is set in \a c.
* @return TRUE if any bit(s) is set; otherwise FALSE.
*/
#define isset(c,l)		(((c)&(l))!=0)      /* any bit(s) set    */

/** This macro indicates if all bits of \a l is set in \a c.
* @return TRUE if all bits are set; otherwise FALSE.
*/
#define ismask(c,l)		(((c)&(l))==(l))    /* all bit(s) set    */

/** This macro indicates if all bits of \a l are cleared in \a c.
* @return TRUE if all bits are cleared; otherwise FALSE.
*/
#define isclear(c,l)		(((c)&(l))==0)      /* bit(s) cleared    */

/** This macro sets the bit(s) \a l in \a c. */
#define set(c,l)		((c)|=(l))          /* bit(s) set        */

/** This macro clears the bit(s) \a l in \a c.*/
#define clear(c,l)		((c)&=~(l))         /* bit(s) cleared    */

/** This macro toggles bit(s) \a l in \a c. */
#define toggle(c,l)         ((c)^=(l))              /* bit(s) changed    */

/*------------------------------- Error Codes ------------------------------*/
#define SUCCESS			0                   /* Success          */
#define	EINVALUE		1                   /* Invalid value    */
#define EIOCTLFAIL		2                   /* IOCTL command    */
#define	EINVPORT		3                   /* Invalid Port     */
#define EIONOTREADY		4                   /* IO Not Ready     */
#define EINVIO			5                   /* Invalid IO       */
#define EPORTNA			6                   /* Port is not allowed */
#define EWRITE                  7                   /* FAIL to Write    */
#define EREAD                   8                   /* FAIL to Read     */
#define EINVDIR                 9                   /* Invalid Port Direction */
#define EINVPIN                 10                  /* Invalid Pin */
#define EINVVAL                 11                  /* Invalid Value */
#define EINVCFG                 12                  /* Invalid config */



/*----------------------- Port Expander definitions  -----------------------*/
/** The MCP23017 contains three address select pins which can be tied to Vss, Vdd.
This gives 8 possible I2C addresses for each chip.  The IO Pi contains two MCP23017 
chips so you can stack up to 4 IO Pi boards on a single Raspberry Pi */
#define DFLT_CFG                CFG_1
#define DFLT_IC1                CFG_1_IC1
#define DFLT_IC2                CFG_1_IC2
// Configuration 1 (default): (IC1 = 0x20, IC2 = 0x21)
#define CFG_1                   1                   /* Configuration 1 */
#define CFG_1_IC1               0x20
#define CFG_1_IC2               0x21
// Configuration 2: (IC1 = 0x22, IC2 = 0x23)
#define CFG_2                   2                   /* Configuration 2 */
#define CFG_2_IC1               0x22
#define CFG_2_IC2               0x23 
// Configuration 3: (IC1 = 0x24, IC2 = 0x25)
#define CFG_3                   3                   /* Configuration 3 */
#define CFG_3_IC1               0x24
#define CFG_3_IC2               0x25 
// Configuration 4: (IC1 = 0x26, IC2 = 0x27)
#define CFG_4                   4                   /* Configuration 4 */
#define CFG_4_IC1               0x26
#define CFG_4_IC2               0x27 

/** MCP23017 [http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf]
    register address mapping...
 */ 

/**
    IO direction A - 1= input 0 = output
 */
#define IODIRA                  0x00

/**
    Bank 0 = pins 1 to 8 
    IO direction B - 1= input 0 = output
 */
#define IODIRB                  0x01

/**
    Input polarity A - If a bit is set, the corresponding GPIO register bit will 
    reflect the inverted value on the pin.
 */
#define IPOLA                   0x02

/** 
    Input polarity B - If a bit is set, the corresponding GPIO register bit
    will reflect the inverted value on the pin
 */
#define IPOLB                   0x03

/** 
    The GPINTEN register controls the interrupt-onchange feature for each
    pin on port A
 */
#define GPINTENA                0x04

/*
    The GPINTEN register controls the interrupt-onchange feature for each
    pin on port B
 */
#define GPINTENB                0x05
    
/**
    Default value for port A - These bits set the compare value for pins
    configured for interrupt-on-change. If the associated pin level is the
    opposite from the register bit, an interrupt occurs
 */
#define DEFVALA                 0x06

/**
    Default value for port B - These bits set the compare value for pins
    configured for interrupt-on-change. If the associated pin level is the
    opposite from the register bit, an interrupt occurs
 */
#define DEFVALB                 0x07

/**
    Interrupt control register for port A.  If 1 interrupt is fired when the
    pin matches the default value, if 0 the interrupt is fired on state change
 */
#define INTCONA                 0x08

/**
    Interrupt control register for port B.  If 1 interrupt is fired when the
    pin matches the default value, if 0 the interrupt is fired on state change 
 */
#define INTCONB                 0x09

/**
    See datasheet for configuration register
 */
#define IOCON                   0x0A

/**
    Pull-up resistors for port A
 */
#define GPPUA                   0x0C

/**
    Pull-up resistors for port B
 */
#define GPPUB                   0x0D

/**
    The INTF register reflects the interrupt condition on the port A pins of
    any pin that is enabled for interrupts. A set bit indicates that the
    associated pin caused the interrupt
 */
#define INTFA                   0x0E

/**
    The INTF register reflects the interrupt condition on the port B pins of
    any pin that is enabled for interrupts. A set bit indicates that the
    associated pin caused the interrupt
 */
#define INTFB                   0x0F

/**
    The INTCAP register captures the GPIO port A value at the time the
    interrupt occurred
 */
#define INTCAPA                 0x10

/**
    The INTCAP register captures the GPIO port B value at the time the
    interrupt occurred
 */
#define INTCAPB                 0x11

/**
    Data port A
 */
#define GPIOA                   0x12
    
/**
    Data port B
 */
#define GPIOB                   0x13

/**
    Output latches A
 */
#define OLATA                   0x14

/**
    Output latches B
 */
#define OLATB                   0x15

/**
 The IOCON register configuring the device:
    R/W-0   R/W-0   R/W-0   R/W-0   R/W-0   R/W-0   R/W-0   U-0
    BANK    MIRROR  SEQOP   DISSLW  HAEN    ODR     INTPOL  —
    bit 7                   ----                            bit 0

bit 7 BANK: Controls how the registers are addressed
    1 = The registers associated with each port are separated into different banks
    0 = The registers are in the same bank (addresses are sequential)
bit 6 MIRROR: INT Pins Mirror bit
    1 = The INT pins are internally connected
    0 = The INT pins are not connected. INTA is associated with PortA and INTB is associated with PortB
bit 5 SEQOP: Sequential Operation mode bit.
    1 = Sequential operation disabled, address pointer does not increment.
    0 = Sequential operation enabled, address pointer increments.
bit 4 DISSLW: Slew Rate control bit for SDA output.
    1 = Slew rate disabled.
    0 = Slew rate enabled.
bit 3 HAEN: Hardware Address Enable bit (MCP23S17 only). Address pins are always enabled on MCP23017.
    1 = Enables the MCP23S17 address pins.
    0 = Disables the MCP23S17 address pins.
bit 2 ODR: This bit configures the INT pin as an open-drain output.
    1 = Open-drain output (overrides the INTPOL bit).
    0 = Active driver output (INTPOL bit sets the polarity).
bit 1 INTPOL: This bit sets the polarity of the INT output pin.
    1 = Active-high.
    0 = Active-low.
bit 0 Unimplemented: 
    Read as ‘0’.
 */
#define IOCON_CFG               0x22

/* Port 0 = pins 1 to 8  */
#define PORT0                   0

/* Port 1 = pins 9 to 16 */
#define PORT1                   1

/* Port 2 = pins 17 to 24 */
#define PORT2                   2

/* Port 3 = pins 25 to 32 */
#define PORT3                   3

/****************************************************************************************************/
#define CFGDIR                  1                   /* Configure the Pin/Port Direction  */
#define CFGPUL                  2                   /* Configure the Pin/Port Pull-Ups   */
#define CFGPOL                  3                   /* Configure the Pin/Port Polarity   */
#define CFGINT                  4                   /* Configure the Pin/Port Interrupts */
    
#define CFGINT_POL              5                   /* Configure the Interrupts Polarity */
#define CFGINT_TYPE             6                   /* Configure the Interrupts Type     */


#define INPUT                   0xFF                /* Input direction */
#define OUTPUT                  0x00                /* Ouput directon  */
#define PULL_UP                 0xFF                /* Pull Up */
#define PULL_DOWN               0x00                /* Pull Down */
#define POL_INVERT              0xFF                /* Polarity Inverted */
#define POL_NORMAL              0x00                /* Polarity Normal */
    
/* Read the value from the selected port at the time of the last interrupt trigger*/
#define INT_CAPTURE             1
/* Read the interrupt status for the pins on the selected port */
#define INT_STATUS              2


#define P1                      1                   /* Pin 1           */
#define P2                      2                   /* Pin 2           */
#define P3                      3                   /* Pin 3           */
#define P4                      4                   /* Pin 4           */
#define P5                      5                   /* Pin 5           */
#define P6                      6                   /* Pin 6           */
#define P7                      7                   /* Pin 7           */
#define P8                      8                   /* Pin 8           */
#define P9                      9                   /* Pin 9           */
#define P10                     10                  /* Pin 10          */
#define P11                     11                  /* Pin 11          */
#define P12                     12                  /* Pin 12          */
#define P13                     13                  /* Pin 13          */
#define P14                     14                  /* Pin 14          */
#define P15                     15                  /* Pin 15          */
#define P16                     16                  /* Pin 16          */
#define P17                     17                  /* Pin 17          */
#define P18                     18                  /* Pin 18          */
#define P19                     19                  /* Pin 19          */
#define P20                     20                  /* Pin 20          */
#define P21                     21                  /* Pin 21          */
#define P22                     22                  /* Pin 22          */
#define P23                     23                  /* Pin 23          */
#define P24                     24                  /* Pin 24          */
#define P25                     25                  /* Pin 25          */
#define P26                     26                  /* Pin 26          */
#define P27                     27                  /* Pin 27          */
#define P28                     28                  /* Pin 28          */
#define P29                     29                  /* Pin 29          */
#define P30                     30                  /* Pin 30          */
#define P31                     31                  /* Pin 31          */
#define P32                     32                  /* Pin 32          */

/*---------------------------- Header Functions ----------------------------*/
/**
 * Open
 * @param i2c - If you are using a revision 0002 or 0003 model B [I2C_0] 
    If you are Rasp 2 [I2C_1]
 * @param cfg - The IO Pi contains two MCP23017 chips so you can stack up to 4 
    IO Pi boards on a single Raspberry Pi (DFLT_CFG | CFG_1 | CFG_2 | CFG_3 | CFG_4)
 * @return:
 *      true: if the IO was open with success
 *      false: if fail to open the IO
 */
BOOL bOpenIO(const CHAR * i2c, BYTE cfg);

/**
 * Set Port Configuration...
 * @param port - Pins to set direction
        PORT0 = pins 1 to 8
        PORT1 = pins 9 to 16
        PORT2 = pins 17 to 24
        PORT3 = pins 25 to 32
 * @param cfg - Configuration Option
        CFGDIR = Configure the Pin/Port Direction
        CFGPUL = Configure the Pin/Port Pull-Ups
        CFGPOL = Configure the Pin/Port Polarity
 * @param val - Configuration Value
        INPUT       = Input Direction
        OUTPUT      = Output Direction
        PULL_UP     = Pull-up
        PULL_DOWN   = Pull-down
        POL_INVERT  = Polarity Inverted
        POL_NORMAL  = Polarity Normal
 * @return SUCCESS || EIONOTREADY || EIOCTLFAIL || EWRITE || EINVCFG || EINVVAL
 */
INT32 iSetPortConfig(BYTE port, BYTE cfg, BYTE val);

/**
 * Get a specific port configuration
 * @param port - Pins to set direction
        PORT0 = pins 1 to 8
        PORT1 = pins 9 to 16
        PORT2 = pins 17 to 24
        PORT3 = pins 25 to 32
 * @param cfg - Configuration Option
        CFGDIR = Configure the Pin/Port Direction
        CFGPUL = Configure the Pin/Port Pull-Ups
        CFGPOL = Configure the Pin/Port Polarity
        CFGINT = Configure the Pin/Port Interrupts
 * @return the current port configuration
 */
BYTE iGetPortConfig(BYTE port, BYTE cfg);

/**
 * Set Pint Configuration...
 * @param pin - Pint to set configuration
        P1 - P32
 * @param cfg - Configuration Option
        CFGDIR = Configure the Pin/Port Direction
        CFGPUL = Configure the Pin/Port Pull-Ups
        CFGPOL = Configure the Pin/Port Polarity
        CFGINT = Configure the Pin/Port Interrupts
 * @param val - Configuration Value
        INPUT       = Input Direction
        OUTPUT      = Output Direction
        PULL_UP     = Pull-up
        PULL_DOWN   = Pull-down
        POL_INVERT  = Polarity Inverted
        POL_NORMAL  = Polarity Normal
 * @return SUCCESS || EIONOTREADY || EIOCTLFAIL || EWRITE || EINVCFG || EINVVAL
 */
INT32 iSetPinConfig(BYTE pin, BYTE cfg, BYTE val);

/**
 * Get the Pin configuration
 * @param pin - Pint to set configuration
        P1 - P32
 * @param cfg - Configuration Option
        CFGDIR = Configure the Pin/Port Direction
        CFGPUL = Configure the Pin/Port Pull-Ups
        CFGPOL = Configure the Pin/Port Polarity
        CFGINT = Configure the Pin/Port Interrupts
 * @return the current pin configuration
 */
INT32 iGetPinConfig(BYTE pin, BYTE cfg);

/**
 * Write for an IO port
 * @param port - Pins to set direction
        PORT0 = pins 1 to 8
        PORT1 = pins 9 to 16
        PORT2 = pins 17 to 24
        PORT3 = pins 25 to 32
 * @param val - number between 0 and 255 or 0x00 and 0xFF
 * @return SUCCESS || EIONOTREADY || EIOCTLFAIL || EWRITE
 */
INT32 iWritePort(BYTE port, BYTE val);

/**
 * Write for an pint port
 * @param pin - Pint to set direction
        P1 - P32
 * @param val - 1 | 0
 * @return SUCCESS || EIONOTREADY || EIOCTLFAIL || EWRITE
 */
INT32 iWritePin(BYTE pin, BYTE val);

/**
 * Read the Port value
 * @param port - Pins to set direction
        PORT0 = pins 1 to 8
        PORT1 = pins 9 to 16
        PORT2 = pins 17 to 24
        PORT3 = pins 25 to 32
 * @return port value
 */
BYTE bReadPort(BYTE port);

/**
 * Read the Pin value
 * @param pin - Pint to set direction
        P1 - P32
 * @return pin value
 */
BYTE bReadPin(BYTE pin);


BYTE bSetInterrupt(BYTE port, BYTE type, BYTE val);

BYTE bReadInterrupt(BYTE port, BYTE type);


/**
 * This function print a detailed explanation of an error code.
 * @param:
 *      eCode: the error code identifier
 */
void vPrintISAIOErrorCodes(INT32 eCode);
        
#ifdef __cplusplus
}
#endif
