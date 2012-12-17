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
#fuses HS, NOWDT, NOPROTECT, BROWNOUT, PUT, NOLVP
#use delay(clock=20000000)

#include "aquarium.c"

/* ----------------------------------------------------------------------- */
typedef unsigned int word;


#define CONST_SEC_IN_A_DAY (24*3600)   //secondes dans un jour

int is_button_haut_pressed(void)
{
   return !input(BUTTON_HAUT);
}

int is_button_bas_pressed(void)
{
   return !input(BUTTON_BAS);
}

void set_cycle_timing_in_eeprom(int value){
   write_program_eeprom(0, value);
   restart_wdt();
}

int get_cycle_timing_from_eeprom(void){
   restart_wdt();
   return read_program_eeprom(0);
   
}

void show_cycle_timing(int value){
   if(value == 1)
   {
      output_high(OUTPUT_LED_EAU_OSMOSEE);
   }
   if(value == 2)
   {
      output_high(OUTPUT_LED_EAU_OSMOSEE);
      output_high(OUTPUT_LED_EAU_SOURCE);
   }
   if(value == 3)
   {
      output_high(OUTPUT_LED_EAU_OSMOSEE);
      output_high(OUTPUT_LED_EAU_SOURCE);
      output_high(OUTPUT_LED_VIDANGE);
   }
   if(value == 4)
   {
      output_high(OUTPUT_LED_EAU_OSMOSEE);
      output_high(OUTPUT_LED_EAU_SOURCE);
      output_high(OUTPUT_LED_VIDANGE);
      output_high(LED_BUTTON_HAUT);
   }
   if(value == 5)
   {
      output_high(OUTPUT_LED_EAU_OSMOSEE);
      output_high(OUTPUT_LED_EAU_SOURCE);
      output_high(OUTPUT_LED_VIDANGE);
      output_high(LED_BUTTON_HAUT);
      output_high(LED_BUTTON_BAS);
   }
   delay_ms(500);
   restart_wdt();
   delay_ms(500);
   restart_wdt();
   delay_ms(500);
   restart_wdt();
   delay_ms(500);
   restart_wdt();
   output_low(OUTPUT_LED_EAU_OSMOSEE);
   output_low(OUTPUT_LED_EAU_SOURCE);
   output_low(OUTPUT_LED_VIDANGE);
   output_low(LED_BUTTON_HAUT);
   output_low(LED_BUTTON_BAS);
   restart_wdt();
}

int change_cycle_timing(void){
   int value = 1;
   while(is_button_haut_pressed()||is_button_bas_pressed())
      restart_wdt();
   while(!is_button_bas_pressed())
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
      if(is_button_haut_pressed())
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

void init_io(void){
/*
#define OUTPUT_EAU_OSMOSEE PIN_B1
#define OUTPUT_EAU_SOURCE  PIN_B2
#define OUTPUT_VIDANGE     PIN_B3

#define OUTPUT_LED_EAU_OSMOSEE PIN_B5
#define OUTPUT_LED_EAU_SOURCE  PIN_B4
#define OUTPUT_LED_VIDANGE     PIN_A0

#define CAPTEUR_VIDE          PIN_D1
#define CAPTEUR_EAU_OSMOSEE   PIN_D2
#define CAPTEUR_PLEIN         PIN_D3

#define BUTTON_HAUT           PIN_A1
#define LED_BUTTON_HAUT       PIN_A2
#define BUTTON_BAS            PIN_A4
#define LED_BUTTON_BAS        PIN_A3
*/

   set_tris_a(0b00010010);
   set_tris_b(0);
   set_tris_d(0b00001111);
   //Entrées
   input(BUTTON_HAUT);
   input(BUTTON_BAS);
   input(CAPTEUR_VIDE);
   input(CAPTEUR_EAU_OSMOSEE);
   input(CAPTEUR_PLEIN);
   //LEDs
   output_high(OUTPUT_LED_EAU_OSMOSEE);
   output_high(OUTPUT_LED_EAU_SOURCE);
   output_high(OUTPUT_LED_VIDANGE);
   output_high(LED_BUTTON_HAUT);
   output_high(LED_BUTTON_BAS);
   output_low(OUTPUT_LED_EAU_OSMOSEE);
   output_low(OUTPUT_LED_EAU_SOURCE);
   output_low(OUTPUT_LED_VIDANGE);
   output_low(LED_BUTTON_HAUT);
   output_low(LED_BUTTON_BAS);
   //relais
   output_low(OUTPUT_EAU_OSMOSEE);
   output_low(OUTPUT_EAU_SOURCE);
   output_low(OUTPUT_VIDANGE);

}

void main() {
   int tmp_cycle_timing_value = 0;
   disable_interrupts(GLOBAL);
   init_io();
   //setup_wdt(WDT_2304MS);

   init_aquarium_state();
   update_output_cycle_remplissage();
   update_output_cycle_vidage();
   
   if(is_button_haut_pressed()&&is_button_bas_pressed())
      set_cycle_timing_in_eeprom(change_cycle_timing());
   restart_wdt();
   
   tmp_cycle_timing_value = get_cycle_timing_from_eeprom();
   if((tmp_cycle_timing_value > 5)||(tmp_cycle_timing_value < 1))
   {
      tmp_cycle_timing_value = 2;
      set_cycle_timing_in_eeprom(tmp_cycle_timing_value);
   }
   restart_wdt();
   show_cycle_timing(tmp_cycle_timing_value);
   set_default_delay_between_remplissage(CONST_SEC_IN_A_DAY*tmp_cycle_timing_value);
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
      if(is_button_haut_pressed())
      {
         start_a_cycle();
      }
      if(is_button_bas_pressed())
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

