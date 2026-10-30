
/*
 * 
 * Simulation_Run of the ALOHA Protocol
 * 
 * Copyright (C) 2014 Terence D. Todd
 * Hamilton, Ontario, CANADA
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 * 
 */

/**********************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

/**********************************************************************/

#include "simlib.h"
#include "simparameters.h"

/**********************************************************************/

typedef double Time;
typedef Fifoqueue_Ptr Buffer_Ptr;

/**********************************************************************/

typedef struct _station_
{
  int id;
  Buffer_Ptr buffer;
} Station, * Station_Ptr;

/**********************************************************************/

typedef struct _packet_ 
{
  double arrive_time;
  double service_time;
  int station_id;
} Packet, * Packet_Ptr;

typedef struct _simulation_run_data_
{
  Station_Ptr stations;
  Buffer_Ptr data_buffer;
  long int blip_counter;
  long int arrival_count;
  long int number_of_packets_processed;
  long int number_completed_in_window;
  long int reservation_successes;
  long int reservation_collisions;
  long int backlog_at_cutoff;
  double accumulated_delay;
  double packet_arrival_rate;
  double reservation_slot_duration;
  int data_channel_busy;
  int arrivals_done;
  int simulation_done;
  unsigned random_seed;
} Simulation_Run_Data, * Simulation_Run_Data_Ptr;

/**********************************************************************/

/*
 * Function prototypes
 */

int
main(void);

/**********************************************************************/

#endif /* main.h */






