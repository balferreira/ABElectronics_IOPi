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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include "ABEIOPI.h"

/**
 * Get the specific Bit value of a given pin
 */
#define CONVERT_PIN_2_BIT(P)                            \
({                                                      \
    BYTE r; BYTE x = 0x00;                              \
    if (P >= P1 && P <= P32){ r = (x |= 1 << ((P-1)%8));\
    }else{r = -EINVPIN;}                                \
    r;                                                  \
})

/*
 * Get the port associated to a pin
 */
#define CONVERT_PIN_2_PORT(P)                           \
({                                                      \
    BYTE r;                                             \
    if (P >= P1 && P <= P16)                            \
    {if (P <= 8){ r=PORT0; }else{ r=PORT1; }            \
    }else{if(P <= 24) { r=PORT2; }else{ r=PORT3; }}     \
    r;                                                  \
})

/**
 * Get the specific I2C configuration to a given pin
 */
#define CONVERT_PIN_2_I2C_CHIP(P)                       \
({                                                      \
    BYTE r;                                             \
    if (P >= P1 && P <= P16){r=_i2cIC1;}else{r=_i2cIC2;}\
    r;                                                  \
})

#define CONVERT_PORT_2_I2C_CHIP(P)                      \
({                                                      \
    BYTE r;                                             \
    if (P == PORT0 || P == PORT1)                       \
    {r=_i2cIC1;}else{r=_i2cIC2;}                        \
    r;                                                  \
})

#define CONVERT_PORT_2_GPIO_REG(P)                      \
({                                                      \
    BYTE r;                                             \
    if (P == PORT0 || P == PORT2)                       \
    {r=GPIOA;}else{r=GPIOB;}                            \
    r;                                                  \
}) 

/**
 * Get the right register 2 a specific configuration
 */
#define CONVERT_CFG_2_REGISTER(C,P)                     \
({                                                      \
    BYTE r;                                             \
    if (C == CFGDIR)                                    \
    {                                                   \
        if (P == PORT0 || P == PORT2)                   \
        { r=IODIRA; }else{ r=IODIRB; }                  \
    }                                                   \
    if (C == CFGPUL)                                    \
    {                                                   \
        if (P == PORT0 || P == PORT2)                   \
        { r=GPPUA; }else{ r=GPPUB; }                    \
    }                                                   \
    if (C == CFGPOL)                                    \
    {                                                   \
        if (P == PORT0 || P == PORT2)                   \
        { r=IPOLA; }else{ r=IPOLB; }                    \
    }                                                   \
    if (C == CFGINT)                                    \
    {                                                   \
        if (P == PORT0 || P == PORT2)                   \
        { r=GPINTENA; }else{ r=GPINTENB; }              \
    }                                                   \
    r;                                                  \
})

/**
 * Get the right register 2 a specfic interrupt configuration
 */
#define CONVERT_PORT_2_INTERR_TYPE(T,P)                 \
({                                                      \
    BYTE r;                                             \
    if (T == INT_CAPTURE)                               \
    {                                                   \
        if (P == PORT0 || P == PORT2)                   \
        { r=INTCAPA; }else{ r=INTCAPB; }                \
    }                                                   \
    if (T == INT_STATUS)                                \
    {                                                   \
        if (P == PORT0 || P == PORT2)                   \
        { r=INTFA; }else{ r=INTFB; }                    \
    }                                                   \
    if (T == CFGINT_TYPE)                               \
    {                                                   \
        if (P == PORT0 || P == PORT2)                   \
        { r=INTCONA; }else{ r=INTCONB; }                \
    }                                                   \
    r;                                                  \
}) 
/*----------------------------- Global variables ----------------------------*/
static	INT32	_i2cbus     = 0;        /* File descriptor used to operate the device driver */
static  BOOL    _i2cbusOpen = false;    /* Is the IO already Open */
static  BYTE    _i2cIOCON   = IOCON_CFG;/* Default IOCON */
static  BYTE    _i2cIC1     = 0x00;     /* IC1 Configuration */
static  BYTE    _i2cIC2     = 0x00;     /* IC2 Configuration */

/*---------------------- Private functions declaration ----------------------*/
BOOL bLoadConfig(BYTE cfg);
INT32 iWriteToI2c(char address, char reg, char value);
INT32 iReadFromI2c(char addr, char reg);

/*----------------------- Loaded by Library Mechanism -----------------------*/
void _init()
{

}

void _fini()
{
  if(_i2cbusOpen)
    close(_i2cbus);
}

/*---------------------------- Public functions ----------------------------*/
BOOL bOpenIO(const CHAR * i2c, BYTE cfg)
{
    if(_i2cbusOpen) return true;

    if ((_i2cbus = open(i2c, O_RDWR)) == -1)
    {
      printf("ERROR: Open IO device driver: %s\n", i2c);
      return false;
    }
    _i2cbusOpen = true;

    // Load IO Pi configuration...
    bLoadConfig(cfg);

    // Configure IOCON on both chip's
    if(iWriteToI2c(_i2cIC1, IOCON, _i2cIOCON) != SUCCESS ||
       iWriteToI2c(_i2cIC2, IOCON, _i2cIOCON) != SUCCESS)
    {
        printf("[ERROR] - Fail to configure 'IOCON'...\n");
        return false;
    }

    // Configure the IODIRA and IODIRB as INPUTS (0xFF)...
    if(iSetPortConfig(PORT0, CFGDIR, INPUT) != SUCCESS ||
       iSetPortConfig(PORT1, CFGDIR, INPUT) != SUCCESS ||
       iSetPortConfig(PORT2, CFGDIR, INPUT) != SUCCESS ||
       iSetPortConfig(PORT3, CFGDIR, INPUT) != SUCCESS)
    {
        printf("[ERROR] - Fail to set IO direction!!!\n");
        return false;
    }
    // Configure the Pull-ups
    if(iSetPortConfig(PORT0, CFGPUL, PULL_DOWN) != SUCCESS ||
       iSetPortConfig(PORT1, CFGPUL, PULL_DOWN) != SUCCESS ||
       iSetPortConfig(PORT2, CFGPUL, PULL_DOWN) != SUCCESS ||
       iSetPortConfig(PORT3, CFGPUL, PULL_DOWN) != SUCCESS)
    {
        printf("[ERROR] - Fail to set pull-up's!!!\n");
        return false;
    }
    // Configure the Polarity
    if(iSetPortConfig(PORT0, CFGPOL, POL_NORMAL) != SUCCESS ||
       iSetPortConfig(PORT1, CFGPOL, POL_NORMAL) != SUCCESS ||
       iSetPortConfig(PORT2, CFGPOL, POL_NORMAL) != SUCCESS ||
       iSetPortConfig(PORT3, CFGPOL, POL_NORMAL) != SUCCESS)
    {
        printf("[ERROR] - Fail to invert polarities!!!\n");
        return false;
    }

    return true;
}

INT32 iSetPortConfig(BYTE port, BYTE cfg, BYTE val)
{
    /* Check if the port is valid...*/
    if(port<PORT0 || port>PORT3) return -EINVPORT;

    /* Check if the configuration option is valid...*/
    if(cfg<CFGDIR || cfg>CFGINT) return -EINVCFG;

    /* Check if the value to set is valid...*/
    if(val<0x00 || val>0xff) return -EINVVAL;

    return iWriteToI2c(CONVERT_PORT_2_I2C_CHIP(port), 
                       CONVERT_CFG_2_REGISTER(cfg,port), val);
}

BYTE iGetPortConfig(BYTE port, BYTE cfg)
{
    /* Check if the port is valid...*/
    if(port<PORT0 || port>PORT3) return -EINVPORT;

    /* Read current Configuration Status */
    return iReadFromI2c(CONVERT_PORT_2_I2C_CHIP(port), 
                        CONVERT_CFG_2_REGISTER(cfg,port));
}

INT32 iSetPinConfig(BYTE pin, BYTE cfg, BYTE val)
{
    /* Check if the port is valid...*/
    if(pin<P1 || pin>P32) return -EINVPIN;

    /* Check if the configuration option is valid...*/
    if(cfg<CFGDIR || cfg>CFGINT) return -EINVCFG;

    /* Check if the value to set is valid...*/
    if(val<0x00 || val>0xff) return -EINVVAL;

    /* Read current Pin Status */
    BYTE pin2set = iReadFromI2c(CONVERT_PIN_2_I2C_CHIP(pin),
                    CONVERT_CFG_2_REGISTER(cfg, CONVERT_PIN_2_PORT(pin)));

    /* Change the right pin... */
    if(val) set(pin2set, CONVERT_PIN_2_BIT(pin));
    else clear(pin2set, CONVERT_PIN_2_BIT(pin));

    // Write to I2C bus...
    return iWriteToI2c(CONVERT_PIN_2_I2C_CHIP(pin), 
                       CONVERT_CFG_2_REGISTER(cfg, CONVERT_PIN_2_PORT(pin)), pin2set);
}

INT32 iGetPinConfig(BYTE pin, BYTE cfg)
{
    /* Check if the port is valid...*/
    if(pin<P1 || pin>P32) return -EINVPIN;

    /* Check if the configuration option is valid...*/
    if(cfg<CFGDIR || cfg>CFGINT) return -EINVCFG;

    /* Read current Pin Status */
    BYTE pin2get = iReadFromI2c(CONVERT_PIN_2_I2C_CHIP(pin),
                    CONVERT_CFG_2_REGISTER(cfg, CONVERT_PIN_2_PORT(pin)));

    return isset(pin2get, CONVERT_PIN_2_BIT(pin)) ? 1 : 0;
}

INT32 iWritePort(BYTE port, BYTE val)
{
    /* Check if the port is valid...*/
    if(port<PORT0 || port>PORT3) return -EINVPORT;

    /*	If the direction is not valid...*/
    if(val<0 || val>255) return -EINVVAL;

    // Write to I2C bus...
    return iWriteToI2c( CONVERT_PORT_2_I2C_CHIP(port), 
                        CONVERT_PORT_2_GPIO_REG(port), 
                        val);
}

INT32 iWritePin(BYTE pin, BYTE val)
{
    /*	If the pin is not valid...*/
    if(pin<P1 || pin>P32) return -EINVPIN;

    /* If the value isn't valid... */
    if(val<0 || val>1) return -EINVVAL;

    /* Read current Pin Status */
    BYTE pin2set = iReadFromI2c(CONVERT_PIN_2_I2C_CHIP(pin),
                    CONVERT_PORT_2_GPIO_REG(CONVERT_PIN_2_PORT(pin)));

    if(val) set(pin2set, CONVERT_PIN_2_BIT(pin));
    else clear(pin2set, CONVERT_PIN_2_BIT(pin));

    // Write to I2C bus...
    return iWriteToI2c(CONVERT_PIN_2_I2C_CHIP(pin), 
                       CONVERT_PORT_2_GPIO_REG(CONVERT_PIN_2_PORT(pin)), pin2set);
}

BYTE bReadPort(BYTE port)
{
    /* Check if the port is valid...*/
    if(port<PORT0 || port>PORT3) return -EINVPORT;

    return iReadFromI2c(CONVERT_PORT_2_I2C_CHIP(port), 
                        CONVERT_PORT_2_GPIO_REG(port));
}

BYTE bReadPin(BYTE pin)
{
    /*	If the pin is not valid...*/
    if(pin<P1 || pin>P32) return -EINVPIN;

    /* Read current Pin Status */
    BYTE pin2get = iReadFromI2c(CONVERT_PIN_2_I2C_CHIP(pin),
                    CONVERT_PORT_2_GPIO_REG(CONVERT_PIN_2_PORT(pin)));    

    return isset(pin2get, CONVERT_PIN_2_BIT(pin)) ? 1 : 0;
}

BYTE bSetInterrupt(BYTE port, BYTE type, BYTE val)
{
    BYTE ret, ref = IOCON_CFG;
    switch(type)
    {
        case CFGINT_POL:
            if(val) set(ref, B1);
            else clear(ref, B1);

            // Write to I2C bus...
            ret = iWriteToI2c(CONVERT_PORT_2_I2C_CHIP(port), IOCON, ref);
            break;
        case CFGINT_TYPE:
            // Write to I2C bus...
            ret = iWriteToI2c(CONVERT_PORT_2_I2C_CHIP(port), 
                    CONVERT_PORT_2_INTERR_TYPE(type, port), val);
            break;
        default:
            ret = 0;
    }
    return ret;
}

BYTE bReadInterrupt(BYTE port, BYTE type)
{
    /* Check if the port is valid...*/
    if(port<PORT0 || port>PORT3) return -EINVPORT;

    return iReadFromI2c(CONVERT_PORT_2_I2C_CHIP(port), 
                        CONVERT_PORT_2_INTERR_TYPE(type, port));
}


/*---------------------- Private functions definition ----------------------*/
INT32 iReadFromI2c(char addr, char reg)
{
    if(!_i2cbusOpen) return -EIONOTREADY;
    if (ioctl(_i2cbus, I2C_SLAVE, addr) < 0) return -EIOCTLFAIL;

    BYTE buf[10];
    buf[0] = reg; // Register to Read

    if ((write(_i2cbus, buf, 1)) != 1) return -EWRITE;
    if (read(_i2cbus, buf, 1) != 1) return -EREAD;
    return buf[0];
}

INT32 iWriteToI2c(char addr, char reg, char val)
{
    if(!_i2cbusOpen) return -EIONOTREADY;
    if (ioctl(_i2cbus, I2C_SLAVE, addr) < 0) return -EIOCTLFAIL;

    BYTE buf[10];
    buf[0] = reg; // Register to Write
    buf[1] = val; // Data to Write

    if ((write(_i2cbus, buf, 2)) != 2) return -EWRITE;
    return SUCCESS;
}

BOOL bLoadConfig(BYTE cfg)
{
    switch(cfg)
    {
        case CFG_1:
            _i2cIC1 = CFG_1_IC1;
            _i2cIC2 = CFG_1_IC2;
            break;
        case CFG_2:
            _i2cIC1 = CFG_2_IC1;
            _i2cIC2 = CFG_2_IC2;
            break;
        case CFG_3:
            _i2cIC1 = CFG_3_IC1;
            _i2cIC2 = CFG_3_IC2;
            break;
        case CFG_4:
            _i2cIC1 = CFG_4_IC1;
            _i2cIC2 = CFG_4_IC2;
            break;
        default:
            _i2cIC1 = DFLT_IC1;
            _i2cIC2 = DFLT_IC2;
            break;
    }
    return true;
}
