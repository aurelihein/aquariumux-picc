/**
 * \file aquarium.c
 * \brief States file of the aquariumux application
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

typedef enum {INIT_REMPLISSAGE = 0, REMPLISSAGE_EAU_OSMOSEE, FIN_EAU_OSMOSEE, REMPLISSAGE_EAU_SOURCE, FIN_EAU_SOURCE, FIN_REMPLISSAGE} remplissage_states;
typedef enum {INIT_VIDAGE = 0, DEBUT_VIDAGE, VIDAGE, FIN_VIDAGE} vidage_states;

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

long default_delay_between_remplissage = (20);//CHANGE

long delay_between_remplissage = 0;

//1 remplissage automatique activée
char remplissage_auto = 1;

remplissage_states remplissage_state;
vidage_states vidage_state;

void set_default_delay_between_remplissage(long value){
   default_delay_between_remplissage = value;
}

char get_capteur_niveau_vide(void){
   //return capteur_niveau_vide;
   return !input(CAPTEUR_VIDE);
}
char get_capteur_niveau_eau_osmosee(void){
   //return capteur_niveau_eau_osmosee;
   return input(CAPTEUR_EAU_OSMOSEE);
}
char get_capteur_niveau_plein(void){
   //return capteur_niveau_plein;
   return input(CAPTEUR_PLEIN);
}

void set_prise_electrique_eau_osmosee(char state){
   if(state)
   {
      output_high(OUTPUT_EAU_OSMOSEE);
      output_high(OUTPUT_LED_EAU_OSMOSEE);
   }
   else
   {
      output_low(OUTPUT_EAU_OSMOSEE);
      output_low(OUTPUT_LED_EAU_OSMOSEE);
   }
}
void set_prise_electrique_eau_source(char state){
   if(state)
   {
      output_high(OUTPUT_EAU_SOURCE);
      output_high(OUTPUT_LED_EAU_SOURCE);
   }
   else
   {
      output_low(OUTPUT_EAU_SOURCE);
      output_low(OUTPUT_LED_EAU_SOURCE);
   }
}
void set_prise_electrique_vidage(char state){
   if(state)
   {
      output_high(OUTPUT_VIDANGE);
      output_high(OUTPUT_LED_VIDANGE);
   }
   else
   {
      output_low(OUTPUT_VIDANGE);
      output_low(OUTPUT_LED_VIDANGE);
   }
}

void init_aquarium_state(void)
{
    remplissage_state = INIT_REMPLISSAGE;
    vidage_state = INIT_VIDAGE;
    set_prise_electrique_eau_osmosee(0);
    set_prise_electrique_eau_source(0);
    set_prise_electrique_vidage(0);
    delay_between_remplissage = default_delay_between_remplissage;
}

void change_timing(void){
   if(remplissage_state == INIT_REMPLISSAGE)
      delay_between_remplissage--;  
}

void start_a_cycle(void){
   delay_between_remplissage = 0;
}

void stop_cycle(void){
   init_aquarium_state();
}

void update_output_cycle_remplissage(void){
   //printf_remplissage_state(remplissage_state);
   switch(remplissage_state)
   {
      case INIT_REMPLISSAGE :
         //rien
      break;
      case REMPLISSAGE_EAU_OSMOSEE :
         set_prise_electrique_eau_osmosee(1);
      break;
      case FIN_EAU_OSMOSEE :
         set_prise_electrique_eau_osmosee(0);
      break;
      case REMPLISSAGE_EAU_SOURCE :
         set_prise_electrique_eau_source(1);
      break;
      case FIN_EAU_SOURCE :
         set_prise_electrique_eau_source(0);
      break;
      case FIN_REMPLISSAGE :
         //rien
      break;
      default :
      break;
   }
}

void update_output_cycle_vidage(void){
   //printf_vidage_state(vidage_state);
   switch(vidage_state)
   {
      case INIT_VIDAGE :
         //rien
      break;
      case DEBUT_VIDAGE :
         set_prise_electrique_vidage(1);
      break;
      case VIDAGE :
         //rien
      break;
      case FIN_VIDAGE :
         set_prise_electrique_vidage(0);
      break;
      default :
      break;
   }
}

int cycle_remplissage(void){
   int update_cycle = 0;
   switch(remplissage_state)
   {
      case INIT_REMPLISSAGE :      
         if(delay_between_remplissage<=0) 
            {
               delay_between_remplissage = default_delay_between_remplissage;
               if(get_capteur_niveau_vide() && !get_capteur_niveau_eau_osmosee() && !get_capteur_niveau_plein()){
                  remplissage_state = REMPLISSAGE_EAU_OSMOSEE;
                  update_cycle = 1;
               }
            }
      break;
      case REMPLISSAGE_EAU_OSMOSEE :
         if(get_capteur_niveau_eau_osmosee()){
            remplissage_state = FIN_EAU_OSMOSEE;
            update_cycle = 1;
         }
      break;
      case FIN_EAU_OSMOSEE :
         if(1){
            remplissage_state = REMPLISSAGE_EAU_SOURCE;
            update_cycle = 1;
         }
      break;
      case REMPLISSAGE_EAU_SOURCE :
         if(get_capteur_niveau_plein()){
            remplissage_state = FIN_EAU_SOURCE;
            update_cycle = 1;
         }
      break;
      case FIN_EAU_SOURCE :
         if(remplissage_auto){
            remplissage_state = INIT_REMPLISSAGE;
            update_cycle = 1;
         }else{
            remplissage_state = FIN_REMPLISSAGE;
            update_cycle = 1;
         }
      break;
      case FIN_REMPLISSAGE :
         //rien
      break;
      default :
      break;
   }
   if(update_cycle)
      update_output_cycle_remplissage();
   return 0;
}

int cycle_vidage(void){
   int update_cycle = 0;
   switch(vidage_state)
   {
      case INIT_VIDAGE :
         if(!get_capteur_niveau_vide() && (remplissage_state==INIT_REMPLISSAGE) ){
            vidage_state = DEBUT_VIDAGE;
            update_cycle = 1;
         }
      break;
      case DEBUT_VIDAGE :
         if(1){
            vidage_state = VIDAGE;
            update_cycle = 1;
         }
      break;
      case VIDAGE :
         if(get_capteur_niveau_vide()){
            vidage_state = FIN_VIDAGE;
            update_cycle = 1;
         }
      break;
      case FIN_VIDAGE :
         
            vidage_state = INIT_VIDAGE;
            update_cycle = 1;
      break;
      default :
      break;
   }
   if(update_cycle)
      update_output_cycle_vidage();
   return 0;
}
