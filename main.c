/**
 * \file main.c
 * \brief Main file of the aquariumux application
 * \author Aurelien BOUIN
 * \version 1.0.0
 * \date 12 septembre 2012
 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2012  Aurelien BOUIN (a_bouin@yahoo.fr)                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  This program is free software; you can redistribute it and/or              *
 *  modify it under the terms of the GNU General Public License                *
 *  as published by the Free Software Foundation; either version 3             *
 *  of the License, or (at your option) any later version.                     *
 *                                                                             *
 *  This program is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 *  GNU General Public License for more details.                               *
 *                                                                             *
 *  You should have received a copy of the GNU General Public License          *
 *  along with this program; if not, write to the Free Software                *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* ----------------------------------------------------------------------- */
/* Fichier source compilable avec PICC CCS */
#include <16F877a.h>
#fuses HS, WDT, NOPROTECT, BROWNOUT, PUT, NOLVP
#use delay(clock=4000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#include "flex_lcd_16x1.c"
#include "aquarium.c"

/* ----------------------------------------------------------------------- */
typedef unsigned int word;

#define OUTPUT_HEARTBEAT   PIN_B3

#define TEST   (1)

void main() {
  
   disable_interrupts(GLOBAL);
   lcd_init();
   set_tris_a(0xf1);
   set_tris_b(0);
   set_tris_d(0);
   output_d(0);

   lcd_init();

   switch ( restart_cause() )
   {
      case WDT_TIMEOUT:
      {
         printf("\r\nRestarted processor because of watchdog timeout!\r\n");
         break;
      }
      case NORMAL_POWER_UP:
      {
         printf("\r\nNormal power up!\r\n");
         break;
      }
   }
   setup_wdt(WDT_2304MS);

   printf(lcd_putc,"\fAquariumux v0.9\n");
   printf("\r\n");
   printf("**************************************************************\r\n");
   printf("* Aquariumux v0.9 by Aurelien BOUIN =>http://www.bouinux.com *\r\n");
   printf("**************************************************************\r\n");
   if(!input(PIN_A4))
      set_tempo_into_eeprom();
   
   get_tempo_from_eeprom();
   
   init_aquarium_state();
   update_output_cycle_remplissage();
   update_output_cycle_vidage();
   while(1)
   {
#ifdef   TEST
      test_add_cycle();
#endif
      cycle_remplissage();
      cycle_vidage();
      output_toggle(OUTPUT_HEARTBEAT);
      delay_ms(500);
      output_toggle(OUTPUT_HEARTBEAT);
      delay_ms(500);
      restart_wdt();
      incremente_tempo();
   }
}