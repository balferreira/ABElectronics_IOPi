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
#include <stdlib.h>
#include <signal.h>
#include "./ABEIOPI.h"

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0) 

#define CONVERT_PIN_2_BIT(P)                            \
({                                                      \
    BYTE r; BYTE x = 0x00;                              \
    if (P >= P1 && P <= P32){ r = (x |= 1 << ((P-1)%8));\
    }else{r = -EINVPIN;}                                \
    r;                                                  \
})

/**
 * 
 * To compile use: 
 *              gcc IOPItest.c -o IOPItest -L. -lABEIOPI
 * 
 * To execute: 
 *              LD_LIBRARY_PATH=/home/pi/ABElectronics/IOPI/
 *              export LD_LIBRARY_PATH
 *              ./IOPItest
 *
 */

#define VAL_POL(X)      printf(" | %4s ", X);
#define VAL_POL_E(X)    printf(" | %4s |\n", X);
#define VAL_PUL(X)      printf("| %4s ", X);
#define VAL_PUL_E(X)    printf(" | %4s", X);
#define VAL_DIR(X)      printf("| %3s", X);
#define VAL_BUS(X)      printf(" |  %d  ", X);
#define VAL_BUS_E(X)    printf(" |  %d  |\n", X);
#define PIN_BUS(X,Y)    printf("|  %d  - %2d ", X, Y);
#define SPACE()         printf("|      ");

#define POL_STR(X,P) (isset(X, CONVERT_PIN_2_BIT(P))) ? "INV" : "NOR"
#define PUL_STR(X,P) (isset(X, CONVERT_PIN_2_BIT(P))) ? "UP" : "DOWN"
#define DIR_STR(X,P) (isset(X, CONVERT_PIN_2_BIT(P))) ? "IN" : "OUT"
#define DATA_STR(X,P) (isset(X, CONVERT_PIN_2_BIT(P))) ? 1 : 0

int _MAIN_SIGNAL_ = 0;

void sigHandler(int s){ _MAIN_SIGNAL_=1;}

void Spinner(void)
{
   static int step;

   if(step < 3) { step++;
   } else {step = 0; }

   printf("\b");
   switch(step)
   {
      case 0: printf("-"); break;
      case 1: printf("/"); break;
      case 2: printf("|"); break;
      case 3: printf("\\");break;
   }
   fflush(stdout);
}

void BUSView()
{
    // Configs: CFGPOL | CFGPUL | CFGDIR
    BYTE p0_pol = iGetPortConfig(PORT0, CFGPOL);
    BYTE p1_pol = iGetPortConfig(PORT1, CFGPOL);
    BYTE p2_pol = iGetPortConfig(PORT2, CFGPOL);
    BYTE p3_pol = iGetPortConfig(PORT3, CFGPOL);
    
    BYTE p0_pul = iGetPortConfig(PORT0, CFGPUL);
    BYTE p1_pul = iGetPortConfig(PORT1, CFGPUL);
    BYTE p2_pul = iGetPortConfig(PORT2, CFGPUL);
    BYTE p3_pul = iGetPortConfig(PORT3, CFGPUL);
    
    BYTE p0_dir = iGetPortConfig(PORT0, CFGDIR);
    BYTE p1_dir = iGetPortConfig(PORT1, CFGDIR);
    BYTE p2_dir = iGetPortConfig(PORT2, CFGDIR);
    BYTE p3_dir = iGetPortConfig(PORT3, CFGDIR);
    
    BYTE p0_data = bReadPort(PORT0);
    BYTE p1_data = bReadPort(PORT1);
    BYTE p2_data = bReadPort(PORT2);
    BYTE p3_data = bReadPort(PORT3);
    

    system("clear");
    printf (" +------+------+-----+-----+-----+-----+-----+-----+------+------+-------+------+------+-----+-----+-----+-----+-----+-----+------+------+\n");
    printf (" |                                                             BUS Information                                                           |\n");
    printf (" +------+------+-----+-----+-----+-----+-----+-----+------+------+-------+------+------+-----+-----+-----+-----+-----+-----+------+------+\n");
    printf (" |                          BUS 1 [0x%02x]                         |       |                         BUS 2 [0x%02x]                          |\n", 0x20, 0x21);
    printf (" +------+------+-----+-----+-----+-----+-----+-----+------+------+-------+------+------+-----+-----+-----+-----+-----+-----+------+------+\n");
    printf (" |  POL |  PUL | DIR | VAL |    PIN    | VAL | DIR |  PUL |  POL |       |  POL |  PUL | DIR | VAL |    PIN    | VAL | DIR |  PUL |  POL |\n");
    printf (" +------+------+-----+-----+-----+-----+-----+-----+------+------+-------+------+------+-----+-----+-----+-----+-----+-----+------+------+\n");
    /*
    printf ("\r | %4s | %4s | %3s |  %d  |  %d  - %2d  |  %d  | %3s | %4s | %4s |       | %4s | %4s | %3s |  %d  |  %d  - %2d  |  %d  | %3s | %4s | %4s |\n",
            "NOR", "UP", "IN",1,1,9,1, "OUT", "DOWN", "INV",
            "NOR", "DOWN", "OUT",1,1,9,1, "OUT", "DOWN", "NOR");
    */
    int pin=P1;
    for(pin; pin<=P8; pin++)
    {
        
        VAL_POL(POL_STR(p0_pol,pin)); VAL_PUL(PUL_STR(p0_pul,pin)); VAL_DIR(DIR_STR(p0_dir,pin));
        //VAL_BUS(bReadPin(pin));  PIN_BUS(pin,(pin+8));  VAL_BUS(bReadPin(pin+(8*1)));
        VAL_BUS(DATA_STR(p0_data,pin));  PIN_BUS(pin,(pin+8));  VAL_BUS(DATA_STR(p1_data,(pin+(8*1))));
        VAL_DIR(DIR_STR(p1_dir,(pin+(8*1)))); VAL_PUL(PUL_STR(p1_pul,(pin+(8*1)))); VAL_POL(POL_STR(p1_pol,(pin+(8*1))));
        SPACE();
        VAL_POL(POL_STR(p2_pol,(pin+(8*2)))); VAL_PUL(PUL_STR(p2_pul,(pin+(8*2)))); VAL_DIR(DIR_STR(p2_dir,(pin+(8*2))));
        //VAL_BUS(bReadPin(pin+(8*2)));  PIN_BUS(pin,(pin+8));  VAL_BUS(bReadPin(pin+(8*3)));
        VAL_BUS(DATA_STR(p2_data,(pin+(8*2))));  PIN_BUS(pin,(pin+8));  VAL_BUS(DATA_STR(p3_data,(pin+(8*3))));
        VAL_DIR(DIR_STR(p3_dir,(pin+(8*3)))); VAL_PUL(PUL_STR(p3_pul,(pin+(8*3)))); VAL_POL_E(POL_STR(p3_pol,pin+(8*3)));

    }
    printf (" +------+------+-----+-----+-----+-----+-----+-----+------+------+-------+------+------+-----+-----+-----+-----+-----+-----+------+------+\n");
    printf ("\nPress 'ctrl+c' to escape...\n");
}

void SetupCfg(BYTE cfg)
{
    int pin_port, pin, port, fport;
    BYTE val;

    // Select the Pin or Port option
    system("clear");
    printf(" __________________________________________ \n");
    printf("|                  Menu                    |\n");
    switch(cfg)
    {
        case CFGDIR: printf("|___ABElectronics IOPi - Port Direction____|\n"); break;
        case CFGPUL: printf("|______ABElectronics IOPi - Pull-Ups_______|\n"); break;
        case CFGPOL: printf("|____ABElectronics IOPi - Port Polarity____|\n");break;
        case CFGINT: printf("|___ABElectronics IOPi - Port Interrupts___|\n");break;
    }
    printf("|                                          |\n");
    printf("|   1. Change Port                         |\n");
    printf("|   2. Change Pin                          |\n");
    printf("|   0. Back                                |\n");
    printf("|__________________________________________|\n");
    printf("|__________________________________________|\n");
    printf("\n");
    printf("Choose an option: ");
    do
    {
        scanf("%d", &pin_port);
    } while(pin_port<0 || pin_port>2);
    if(pin_port==0) return;

    // Select the pin or port
    if(pin_port==1)
    {
        printf("Please choose the PORT (0-3): ");
        do 
        {
            scanf("%d", &port);
        } while (port < 0 || port > 3);
        fport = port;
    }else{
        printf("Please choose the PIN (1-32): ");
        do 
        {
            scanf("%d", &pin);
        } while (pin < P1 || pin > P32); 
    }

    // Select the configuration parameter
    switch(cfg)
    {
        case CFGDIR:
            printf("Please choose the Direction (INPUT[1]=0xFF or OUTPUT[0]=0x00): ");
            break;
        case CFGPUL:
            printf("Please choose the Pull-up value (PULL_UP[1]=0xFF or PULL_DOWN[0]=0x00): ");
            break;
        case CFGPOL:
            printf("Please choose the Polarity value (POL_INVERT[1]=0xFF or POL_NORMAL[0]=0x00): ");
            break;
        case CFGINT:
            printf("Please choose the Polarity value (ENABLE[1]=0xFF or DISABLE[0]=0x00): ");
            break;
    }    
    do
    {
        scanf("%d", &val);
    } while (val < 0 || val > 1);

    if(pin_port==1)
    {
        if(val==0) val=0x00; else val=0xff;   
        if(iSetPortConfig(fport, cfg, val) == SUCCESS)
        {
            printf("Operation performed with success!!!\n");
        }else{
            printf("[ERROR] - Fail to perform the operation!!!\n");
        }
    }else{
        if(iSetPinConfig(pin, cfg, val) == SUCCESS)
        {
            printf("Operation performed with success!!!\n");
        }else{
            printf("[ERROR] - Fail to perform the operation!!!\n");
        }
    }
    sleep(1);
}

void IOSetup()
{
    char opt;
    do 
    {
        system("clear");
        printf(" __________________________________________ \n");
        printf("|                  Menu                    |\n");
        printf("|______ABElectronics IOPi - IO Setup_______|\n");
        printf("|                                          |\n");
        printf("|   1. Change Polarity                     |\n");
        printf("|   2. Change Pull-Ups                     |\n");
        printf("|   3. Change Direction                    |\n");
        printf("|   4. Change Interrupts                    |\n");
        printf("|   0. Back                                |\n");
        printf("|__________________________________________|\n");
        printf("|__________________________________________|\n");
        printf("\n");
        printf("Choose an option: ");

        do
        {
            fflush(stdin);
            opt=getc(stdin);
        } while(opt<'0' || opt>'4');
        system("clear");

        switch (opt)
        {
            case '0':
                return;
            case '1':
                SetupCfg(CFGPOL);
                break;
            case '2':
                SetupCfg(CFGPUL);
                break;
            case '3':
                SetupCfg(CFGDIR);
                break;
            case '4':
                SetupCfg(CFGINT);
                break;
        }
    }while(1);  
}

void IOInfo()
{
    system("clear");
    printf (" +-----+-----+-----+-----+-------+-----+-----+-----+-----+\n");
    printf (" |                     BUS Information                   |\n");
    printf (" +-----+-----+-----+-----+-------+-----+-----+-----+-----+\n");
    printf (" |      BUS 1 [0x%02x]     |       |      BUS 2 [0x%02x]     |\n", 0x20, 0x21);
    printf (" +-----+-----+-----+-----+-------+-----+-----+-----+-----+\n");
    printf (" | VAL |    PIN    | VAL |   +   | VAL |    PIN    | VAL |\n");
    printf (" +-----+-----+-----+-----+-------+-----+-----+-----+-----+\n");

    int pin=P1;
    for(pin; pin<=P8; pin++)
    {
        VAL_BUS(bReadPin(pin)); PIN_BUS(pin,(pin+8));  VAL_BUS(bReadPin(pin+(8*1)));
        SPACE();
        VAL_BUS(bReadPin(pin+(8*2)));  PIN_BUS(pin,(pin+8));  VAL_BUS_E(bReadPin(pin+(8*3)));
    }

    printf (" +-----+-----+-----+-----+-------+-----+-----+-----+-----+\n");
    printf ("\nPress 'ctrl+c' to escape...\n");
    fflush(stdout);
}

void IOChange() 
{
    int pin;
    printf("Please choose the PIN (1-32): ");
    do 
    {
        scanf("%d", &pin);
    } while (pin < 1 || pin > 32);

    char opt;
    printf("Please choose the Output Value (0 or 1): ");
    do 
    {
        fflush(stdin);
        opt = getc(stdin);
    } while (opt < '0' || opt > '1');

    switch (opt)
    {
        case '0':
            if (iWritePin(pin, 0) == SUCCESS)
                printf("Write '0' to IO '%d' - with Success...\n", pin);
            else
                printf("Fail to write '0' to IO '%d'...\n", pin);
            break;
        case '1':
            if (iWritePin(pin, 1) == SUCCESS)
                printf("Write '1' to IO '%d' - with Success...\n", pin);
            else
                printf("Fail to write '1' to IO '%d'...\n", pin);
            break;
    }
    sleep(1);
}

void IOToggle()
{
    int pin, toggTimes;
    printf("Please choose the PIN (1-32): ");
    do 
    {
        scanf("%d", &pin);
    } while (pin < 1 || pin > 32);

    printf("Check out the PIN[%d] (Press 'ctrl+c' to escape):  ", pin);
    while(1)
    {        
        if (iWritePin(pin, 0) != SUCCESS)
        {
            printf("Fail to write '0' to PIN '%d'...\n", pin);
            break;
        }

        Spinner();
        usleep(500000);
        
        if (iWritePin(pin, 1) != SUCCESS)
        {
            printf("Fail to write '1' to PIN '%d'...\n", pin);
            break;
        }

        if(_MAIN_SIGNAL_ == 1) { _MAIN_SIGNAL_ = 0; break; }
        Spinner();
        usleep(500000);
    }
    sleep(1);
}

void IOManipulation()
{
    char opt;
    int port;
    do 
    {
            system("clear");
            printf(" __________________________________________ \n");
            printf("|                  Menu                    |\n");
            printf("|___ABElectronics IOPi - IO Manipulation___|\n");
            printf("|                                          |\n");
            printf("|   1. View IO State                       |\n");
            printf("|   2. IO Change                           |\n");
            printf("|   3. IO Toggle                           |\n");
            printf("|   0. Back                                |\n");
            printf("|__________________________________________|\n");
            printf("|__________________________________________|\n");
            printf("\n");
            printf("Choose an option: ");

            do
            {
                fflush(stdin);
                opt=getc(stdin);
            } while(opt<'0' || opt>'3');
            system("clear");

            switch (opt)
            {
                case '0':
                    return;
                case '1':
                    while(1)
                    {
                        IOInfo();
                        if(_MAIN_SIGNAL_ == 1) { _MAIN_SIGNAL_ = 0; break; }
                        usleep(200000);
                    }
                    break;
                case '2':
                    IOChange();
                    break;
                case '3':
                    IOToggle();
                    break;
            }
    }while(1);  
}

void InterruptsManipulation()
{
    char opt;
    int port;
    do 
    {
            system("clear");
            printf(" __________________________________________ \n");
            printf("|                  Menu                    |\n");
            printf("|__ABElectronics IOPi - Interrupts Manip.__|\n");
            printf("|                                          |\n");
            printf("|   1. View Interrupts Status              |\n");
            printf("|   2. Interrupts Change                   |\n");
            printf("|   3. Interrupts Reset                    |\n");
            printf("|   0. Back                                |\n");
            printf("|__________________________________________|\n");
            printf("|__________________________________________|\n");
            printf("\n");
            printf("Choose an option: ");

            do
            {
                fflush(stdin);
                opt=getc(stdin);
            } while(opt<'0' || opt>'3');
            system("clear");

            switch (opt)
            {
                case '0':
                    return;
                case '1':

                    break;
                case '2':
                    
                    break;
                case '3':
                    
                    break;
            }
    }while(1);  
}

void Menu()
{
    char opt;
    do 
    {
            system("clear");
            printf(" __________________________________________ \n");
            printf("|                  Menu                    |\n");
            printf("|_________ABElectronics IOPi Test__________|\n");
            printf("|                                          |\n");
            printf("|   1. BUS View                            |\n");
            printf("|   2. IO Setup                            |\n");
            printf("|   3. IO Manipulation                     |\n");
            printf("|   4. Interrupts Manipulation             |\n");
            printf("|   0. Exit                                |\n");
            printf("|__________________________________________|\n");
            printf("|__________________________________________|\n");
            printf("\n");
            printf("Choose an option: ");

            do
            {
                fflush(stdin);
                opt=getc(stdin);
            } while(opt<'0' || opt>'4');
            system("clear");

            switch (opt)
            {
                case '0':
                    exit(0);
                    break;
                case '1':
                    while(1)
                    {
                        BUSView();
                        if(_MAIN_SIGNAL_ == 1) { _MAIN_SIGNAL_ = 0; break; }
                        usleep(200000);
                    }
                    break;
                case '2':
                    IOSetup();
                    break;
                case '3':
                    IOManipulation();
                    break;
                case '4':
                    InterruptsManipulation();
                    break;
            }
    }while(1);  
}

int main(int argc, char* argv[]) 
{

	struct sigaction signal;

	signal.sa_handler = sigHandler;
	sigemptyset(&signal.sa_mask);
	signal.sa_flags = 0;

	sigaction(SIGINT, &signal, 0);

        if(!bOpenIO(I2C_1, DFLT_CFG))
        {
            printf("[ERROR] - Fail to open I2C bus!!!\n");
            exit(1);
        }
        
        if( iSetPortConfig(PORT0, CFGDIR, OUTPUT) != SUCCESS ||
            iSetPortConfig(PORT1, CFGDIR, OUTPUT) != SUCCESS ||
            iSetPortConfig(PORT2, CFGDIR, OUTPUT) != SUCCESS ||
            iSetPortConfig(PORT3, CFGDIR, OUTPUT) != SUCCESS)
        {
            printf("[ERROR] - Fail to set Port's direction (as OUTPUT)!!!\n");
            exit(1);
        }
        
        Menu();

	return 0;
} 



