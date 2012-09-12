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
typedef enum {INIT_VIDAGE = 0, DEBUT_VIDAGE, VIDAGE, FIN_VIDAGE, ATTENTE_PROCHAIN_CYCLE_VIDAGE} vidage_states;

#define OUTPUT_EAU_OSMOSEE PIN_B0
#define OUTPUT_EAU_SOURCE  PIN_B1
#define OUTPUT_VIDANGE     PIN_B2

#define ADD_TEMPO_ALLUMAGE_VIDAGE      (0)   //en secondes
#define ADD_TEMPO_ATTENTE_ENTRE_VIDAGE   (2)   //en secondes
#define D_TEMPO_ALLUMAGE_VIDAGE      (10)   //en secondes
#define D_TEMPO_ATTENTE_ENTRE_VIDAGE   (120)   //en secondes
volatile int16 value_tempo_allumage_vidage = 0;
volatile int16 value_tempo_attente_entre_vidage = 0;

//entrees
volatile char capteur_niveau_vide = 1;
volatile char capteur_niveau_eau_osmosee = 0;
volatile char capteur_niveau_plein = 0;

volatile int16 tempo_allumage_vidage = 0;
volatile int16 tempo_attente_entre_vidage = 0;

//1 remplissage automatique activée
char remplissage_auto = 1;

remplissage_states remplissage_state;
vidage_states vidage_state;

void get_tempo_from_eeprom(void)
{
   value_tempo_allumage_vidage = read_program_eeprom(ADD_TEMPO_ALLUMAGE_VIDAGE);
   printf("read %ld at address:%d\r\n",value_tempo_allumage_vidage,ADD_TEMPO_ALLUMAGE_VIDAGE);
   
   value_tempo_attente_entre_vidage = read_program_eeprom(ADD_TEMPO_ATTENTE_ENTRE_VIDAGE);
   printf("read %ld at address:%d\r\n",value_tempo_attente_entre_vidage,ADD_TEMPO_ATTENTE_ENTRE_VIDAGE);
}

void set_tempo_into_eeprom(void)
{
   write_program_eeprom(ADD_TEMPO_ALLUMAGE_VIDAGE, D_TEMPO_ALLUMAGE_VIDAGE);
   value_tempo_allumage_vidage = D_TEMPO_ALLUMAGE_VIDAGE;
   printf("write %ld at address:%d\r\n",value_tempo_allumage_vidage,ADD_TEMPO_ALLUMAGE_VIDAGE);
   
   
   write_program_eeprom(ADD_TEMPO_ATTENTE_ENTRE_VIDAGE, D_TEMPO_ATTENTE_ENTRE_VIDAGE);
   value_tempo_attente_entre_vidage = D_TEMPO_ATTENTE_ENTRE_VIDAGE;
   printf("write %ld at address:%d\r\n",value_tempo_attente_entre_vidage,ADD_TEMPO_ATTENTE_ENTRE_VIDAGE);
}

int cycle = 0;
void test_add_cycle (void){
   if(cycle == 1){
         capteur_niveau_vide=0;
         printf("TEST set capteur_niveau_vide=0\r\n");
   }
   if(cycle == 10){
         capteur_niveau_eau_osmosee=1;
         printf("TEST set capteur_niveau_eau_osmosee=1\r\n");
         
   }
   if(cycle == 30){
         capteur_niveau_plein=1;
         printf("TEST set capteur_niveau_plein\r\n");
   }
   cycle++;
}

void printf_remplissage_state(remplissage_states s){
   switch(s)
   {
      case INIT_REMPLISSAGE :
         printf("REMPLISSAGE:INIT_REMPLISSAGE\r\n");
      break;
      case REMPLISSAGE_EAU_OSMOSEE :
         printf("REMPLISSAGE:REMPLISSAGE_EAU_OSMOSEE\r\n");
      break;
      case FIN_EAU_OSMOSEE :
         printf("REMPLISSAGE:FIN_EAU_OSMOSEE\r\n");
      break;
      case REMPLISSAGE_EAU_SOURCE :
         printf("REMPLISSAGE:REMPLISSAGE_EAU_SOURCE\r\n");
      break;
      case FIN_EAU_SOURCE :
         printf("REMPLISSAGE:FIN_EAU_SOURCE\r\n");
      break;
      case FIN_REMPLISSAGE :
         printf("REMPLISSAGE:FIN_REMPLISSAGE\r\n");
      break;
      default :
         printf("REMPLISSAGE:UNKNOW_REMPLISSAGE_STATE\r\n");
      break;
   }
}

void printf_vidage_state(vidage_states s){
   switch(s)
   {
      case INIT_VIDAGE :
         printf("VIDAGE:INIT_VIDAGE\r\n");
      break;
      case DEBUT_VIDAGE :
         printf("VIDAGE:DEBUT_VIDAGE\r\n");
      break;
      case VIDAGE :
         printf("VIDAGE:VIDAGE\r\n");
      break;
      case FIN_VIDAGE :
         printf("VIDAGE:FIN_VIDAGE\r\n");
      break;
      case ATTENTE_PROCHAIN_CYCLE_VIDAGE :
         printf("VIDAGE:ATTENTE_PROCHAIN_CYCLE_VIDAGE\r\n");
      break;
      default :
         printf("VIDAGE:UNKNOW_VIDAGE_STATE\r\n");
      break;
   }
}


char get_capteur_niveau_vide(void){
   return capteur_niveau_vide;
}
char get_capteur_niveau_eau_osmosee(void){
   return capteur_niveau_eau_osmosee;
}
char get_capteur_niveau_plein(void){
   return capteur_niveau_plein;
}

void set_prise_electrique_eau_osmosee(char state){
   if(state)
   {
      printf("TURN ON:eau_osmosee\r\n");
      output_high(OUTPUT_EAU_OSMOSEE);
   }
   else
   {
      printf("TURN OFF:eau_osmosee\r\n");
      output_low(OUTPUT_EAU_OSMOSEE);
   }
}
void set_prise_electrique_eau_source(char state){
   if(state)
   {
      printf("TURN ON:eau_source\r\n");
      output_high(OUTPUT_EAU_SOURCE);
   }
   else
   {
      printf("TURN OFF:eau_source\r\n");
      output_low(OUTPUT_EAU_SOURCE);
   }
}
void set_prise_electrique_vidage(char state){
   if(state)
   {
      printf("TURN ON:vidage\r\n");
      output_high(OUTPUT_VIDANGE);
   }
   else
   {
      printf("TURN OFF:vidage\r\n");
      output_low(OUTPUT_VIDANGE);
   }
}

void init_aquarium_state(void)
{
    remplissage_state = INIT_REMPLISSAGE;
    vidage_state = INIT_VIDAGE;
    set_prise_electrique_eau_osmosee(0);
    set_prise_electrique_eau_source(0);
    set_prise_electrique_vidage(0);
}

void incremente_tempo(void){
   tempo_allumage_vidage++;
   tempo_attente_entre_vidage++;
   switch(vidage_state)
   {
      case VIDAGE :
         printf("TICK=>tempo_allumage_vidage:%ld to %ld\r\n",tempo_allumage_vidage,value_tempo_allumage_vidage);
         break;
      case ATTENTE_PROCHAIN_CYCLE_VIDAGE :
         printf("TICK=>tempo_attente_entre_vidage:%ld to %ld\r\n",tempo_attente_entre_vidage,value_tempo_attente_entre_vidage);
         break;
      default :
         printf("TICK\r\n");
         break;
   }
}

void update_output_cycle_remplissage(void){
   printf_remplissage_state(remplissage_state);
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
   printf_vidage_state(vidage_state);
   switch(vidage_state)
   {
      case INIT_VIDAGE :
         //rien
      break;
      case DEBUT_VIDAGE :
         tempo_allumage_vidage = 0;
         set_prise_electrique_vidage(1);
      break;
      case VIDAGE :
         //rien
      break;
      case FIN_VIDAGE :
         set_prise_electrique_vidage(0);
         tempo_attente_entre_vidage = 0;
      break;
      case ATTENTE_PROCHAIN_CYCLE_VIDAGE :
         //rien
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
         if(get_capteur_niveau_vide() && !get_capteur_niveau_eau_osmosee() && !get_capteur_niveau_plein()){
            remplissage_state = REMPLISSAGE_EAU_OSMOSEE;
            update_cycle = 1;
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
         if(!capteur_niveau_vide && (remplissage_state==INIT_REMPLISSAGE) ){
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
         if(capteur_niveau_vide || (tempo_allumage_vidage >= value_tempo_allumage_vidage) ){
            vidage_state = FIN_VIDAGE;
            update_cycle = 1;
         }
      break;
      case FIN_VIDAGE :
         if(capteur_niveau_vide){
            vidage_state = INIT_VIDAGE;
         }else{
            vidage_state = ATTENTE_PROCHAIN_CYCLE_VIDAGE;
         }
         update_cycle = 1;
      break;
      case ATTENTE_PROCHAIN_CYCLE_VIDAGE :
         if(tempo_attente_entre_vidage >= value_tempo_attente_entre_vidage){
            vidage_state = INIT_VIDAGE;
            update_cycle = 1;
         }
      break;
      default :
      break;
   }
   if(update_cycle)
      update_output_cycle_vidage();
   return 0;
}