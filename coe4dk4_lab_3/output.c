
/*
 * Simulation_Run of the ALOHA Protocol
 * 
 * Copyright (C) 2014 Terence D. Todd Hamilton, Ontario, CANADA
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/*******************************************************************************/

#include <stdio.h>
#include "simlib.h"
#include "simparameters.h"
#include "main.h"
#include "output.h"

/*******************************************************************************/

void
output_blip_to_screen(Simulation_Run_Ptr simulation_run)
{
  Simulation_Run_Data_Ptr data;

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);

  data->blip_counter++;

  if(data->blip_counter >= BLIPRATE) {
    data->blip_counter = 0;
    fprintf(stderr, "Completed %ld packets at time %.0f\r",
      data->number_of_packets_processed,
      simulation_run_get_time(simulation_run));
    fflush(stderr);
  }
}

/**********************************************************************/

void output_results(Simulation_Run_Ptr this_simulation_run)
{
  double mean_delay;
  Simulation_Run_Data_Ptr sim_data;

  sim_data = (Simulation_Run_Data_Ptr) simulation_run_data(this_simulation_run);
  mean_delay = 0.0;
  if(sim_data->arrival_count > 0) {
    mean_delay = sim_data->accumulated_delay /
      sim_data->arrival_count;
  }

  printf("%u,%.3f,%.3f,%.3f,%.1f,%.3f,%ld,%ld,%ld,%.6f,%.3f,%ld,%ld,%ld\n",
    sim_data->random_seed,
    sim_data->packet_arrival_rate,
    (double) MEAN_PACKET_DURATION,
    sim_data->reservation_slot_duration,
    MEAN_PACKET_DURATION / sim_data->reservation_slot_duration,
    (double) RESERVATION_ATTEMPT_PROBABILITY,
    sim_data->arrival_count,
    sim_data->number_completed_in_window,
    sim_data->number_of_packets_processed,
    (double) sim_data->number_completed_in_window / SIMULATION_DURATION,
    mean_delay,
    sim_data->reservation_successes,
    sim_data->reservation_collisions,
    sim_data->backlog_at_cutoff);
}



