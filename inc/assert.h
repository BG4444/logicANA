/*
 * assert.h
 *
 *  Created on: 20 окт. 2014 г.
 *      Author: bg
 */

#ifndef INC_ASSERT_H_
#define INC_ASSERT_H_


#define ASSERT(n) if(!(n)) {HardFault_Handler();}


#endif /* INC_ASSERT_H_ */
