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
#use delay(clock=20000000)

#include "aquarium.c"

/* ----------------------------------------------------------------------- */
typedef unsigned int word;


#define CONST_SEC_IN_A_DAY (24*3600)   //secondes dans un jour


int change_cycle_timing(void){
   int value = 1;
   while(input(BUTTON_BAS)||input(BUTTON_HAUT));
   while(!input(BUTTON_BAS))
   {
      output_high(OUTPUT_LED_EAU_OSMOSEE);
      output_high(OUTPUT_LED_EAU_SOURCE);
      output_high(OUTPUT_LED_VIDANGE);
      output_high(LED_BUTTON_HAUT);
      output_high(LED_BUTTON_BAS);      
      delay_ms(300);
      restart_wdt();
      if(value == 1)
      {
         output_low(OUTPUT_LED_EAU_SOURCE);
         output_low(OUTPUT_LED_VIDANGE);
         output_low(LED_BUTTON_HAUT);
         output_low(LED_BUTTON_BAS);
      }
      if(value == 2)
      {
         output_low(OUTPUT_LED_VIDANGE);
         output_low(LED_BUTTON_HAUT);
         output_low(LED_BUTTON_BAS);
      }
      if(value == 3)
      {
         output_low(LED_BUTTON_HAUT);
         output_low(LED_BUTTON_BAS);
      }
      if(value == 4)
      {
         output_low(LED_BUTTON_BAS);
      }
      delay_ms(300);
      restart_wdt();
      if(input(BUTTON_HAUT))
         value=((value+1)%5);
      if(!value)
         value=1;
   }
   output_low(OUTPUT_LED_EAU_OSMOSEE);
   output_low(OUTPUT_LED_EAU_SOURCE);
   output_low(OUTPUT_LED_VIDANGE);
   output_low(LED_BUTTON_HAUT);
   output_low(LED_BUTTON_BAS);
   restart_wdt();
   return value;
}

void check_led_working(void){
   output_high(OUTPUT_LED_EAU_OSMOSEE);
   delay_ms(300);
   restart_wdt();
   output_high(OUTPUT_LED_EAU_SOURCE);
   delay_ms(300);
   restart_wdt();
   output_high(OUTPUT_LED_VIDANGE);
   delay_ms(300);
   restart_wdt();
   output_high(LED_BUTTON_HAUT);
   delay_ms(300);
   restart_wdt();
   output_high(LED_BUTTON_BAS);
   delay_ms(300);
   restart_wdt();
   output_low(OUTPUT_LED_EAU_OSMOSEE);
   output_low(OUTPUT_LED_EAU_SOURCE);
   output_low(OUTPUT_LED_VIDANGE);
   output_low(LED_BUTTON_HAUT);
   output_low(LED_BUTTON_BAS);
   restart_wdt();
}

void update_led_status(int value){
   if(remplissage_state != INIT_REMPLISSAGE)
      output_toggle(LED_BUTTON_HAUT);
   else
      output_low(LED_BUTTON_HAUT);
   if(vidage_state != INIT_VIDAGE)
      output_toggle(LED_BUTTON_BAS);
   else
      output_low(LED_BUTTON_BAS);
   if((remplissage_state == INIT_REMPLISSAGE)&&(vidage_state == INIT_VIDAGE))
   {
      if(value)
      {
         output_high(LED_BUTTON_HAUT);
         output_low(LED_BUTTON_BAS);
      }else{      
         output_low(LED_BUTTON_HAUT);
         output_high(LED_BUTTON_BAS);
      }
   }
}

void main() {
  
   disable_interrupts(GLOBAL);
   set_tris_a(0xf1);
   set_tris_b(0);
   set_tris_d(0);
   output_d(0);

   setup_wdt(WDT_2304MS);

   init_aquarium_state();
   update_output_cycle_remplissage();
   update_output_cycle_vidage();
   
   if(input(BUTTON_HAUT)&&input(BUTTON_BAS))
      set_default_delay_between_remplissage(change_cycle_timing()*CONST_SEC_IN_A_DAY);
   else{
      check_led_working();
      set_default_delay_between_remplissage(20);
   }   
   start_a_cycle();
   restart_wdt();
   
   while(1)
   {
      //test_add_cycle();
      cycle_remplissage();
      cycle_vidage();
      update_led_status(1);
      delay_ms(500);
      update_led_status(0);
      delay_ms(500);
      restart_wdt();
      change_timing();
      if(input(BUTTON_HAUT))
         start_a_cycle();
      if(input(BUTTON_BAS))
         stop_cycle();
      restart_wdt();
   }
}

void very_start_test(void)
{
   while(1){
      if(input(BUTTON_HAUT))
         output_high(LED_BUTTON_HAUT);
      else
         output_low(LED_BUTTON_HAUT);
      if(input(BUTTON_BAS))
         output_high(LED_BUTTON_BAS);
      else
         output_low(LED_BUTTON_BAS);
      
      
      if(input(CAPTEUR_VIDE))
         output_high(OUTPUT_EAU_OSMOSEE);
      else
         output_low(OUTPUT_EAU_OSMOSEE);
      if(input(CAPTEUR_EAU_OSMOSEE))
         output_high(OUTPUT_EAU_SOURCE);
      else
         output_low(OUTPUT_EAU_SOURCE);
      if(input(CAPTEUR_PLEIN))
         output_high(OUTPUT_VIDANGE);
      else
         output_low(OUTPUT_VIDANGE);
         
         
      delay_ms(300);
      output_toggle(PIN_B1);
      restart_wdt();
   }
}
