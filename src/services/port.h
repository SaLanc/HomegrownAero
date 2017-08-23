#ifndef PORT_H_
#define PORT_H_

#include "src/util.h"


#define INPUT 0
#define OUTPUT 1
#define OUTSTRONG 3

static inline PortGroup* getPort(
       const u8_t gpioPin) {
u8_t port_index = (gpio_pin / 128);
ut_t group_index = (gpio_pin / 32);
/* Array of available ports */ 
    Port *const ports[PORT_INST_NUM] = PORT_INSTS; 
 
     if (port_index < PORT_INST_NUM) { 
       return &(ports[port_index]->Group[group_index]); 
     } else { 
         return NULL; 
     } 
 }; 


inline static void pinAnalog(Pin p) { 
     if (p.pin & 1) { 
         PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXO = 0x1; 
     } else { 
         PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXE = 0x1; 
     } 
 
     PORT->Group[p.group].PINCFG[p.pin].bit.PMUXEN = 1; 
 } 
 

inline static bool pinRead(Pin p) { 
     return (PORT->Group[p.group].IN.reg & (1<<p.pin)) != 0; 
 } 

inline static void pinMux(Pin p) { 
if (p.pin & 1) { 
PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXO = p.mux; 
} else { 
PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXE = p.mux; 
} 
PORT->Group[p.group].PINCFG[p.pin].bit.PMUXEN = 1; 
} 

 
inline static void pinGpio(Pin p) { 
PORT->Group[p.group].PINCFG[p.pin].bit.PMUXEN = 0; 
} 

 
inline static void pinCfg(Pin p) { 
PORT->Group[p.group].PINCFG[p.pin].reg |= p.cfg; 
} 

 
inline static void pinIn(Pin p) { 
pinGpio(p); 
    PORT->Group[p.group].PINCFG[p.pin].bit.INEN = 1; 
     PORT->Group[p.group].DIRCLR.reg = (1<<p.pin); 
 } 
 
 
 inline static void pinOut(Pin p) { 
     pinGpio(p);   
   PORT->Group[p.group].DIRSET.reg = (1<<p.pin);  
} 
 
 
 
 
 inline static void pinHigh(Pin p) { 
     PORT->Group[p.group].OUTSET.reg = (1<<p.pin); 
 } 
 
 
 inline static void pinLow(Pin p) { 
     PORT->Group[p.group].OUTCLR.reg = (1<<p.pin); 
 } 
 
 
 inline static void pinToggle(Pin p) { 
     PORT->Group[p.group].OUTTGL.reg = (1<<p.pin); 
 } 

#endif
