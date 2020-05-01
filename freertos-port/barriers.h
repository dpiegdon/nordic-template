#ifndef FREERTOS_PORT_H
#define FREERTOS_PORT_H

/*
 * define a few things the latest version of freertos needs,
 * but that are not defined in the nordic SDK port.
 *
 * added by dpiegdon on 2020-05-01
 */
#define portSOFTWARE_BARRIER(...)

// as suggested on https://gcc.gnu.org/onlinedocs/gcc/Volatiles.html
#define portMEMORY_BARRIER(...) __asm__ volatile("": : :"memory")

#endif // FREERTOS_PORT_H
