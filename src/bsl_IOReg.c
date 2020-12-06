/**Author: Pierre-Alexis Barras
 * ID: 40022016
 *
 * Produced for academic purposes.
 * All rights reseved. 2020
 */

#include "bsl_IOReg.h"

//Return value found at given address
u32  bsl_IOReg_Read (u32 ioreg){
	char * p = (char *) ioreg;	//find register via pointer
	return p[0];	//return value from register
}

//Write value found at given address
void bsl_IOReg_Write(u32 ioreg, u32 value){
	char * p = (char *) ioreg;	//find register via pointer
	p[0] = value;	//write value to register
}