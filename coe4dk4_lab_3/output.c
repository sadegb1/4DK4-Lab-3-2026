
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
#include "simparameters.h"
#include "main.h"
#include "output.h"

/*******************************************************************************/

void
output_blip_to_screen(Simulation_Run_Ptr simulation_run)
{
  double percentagedone;
  Simulation_Run_Data_Ptr data;

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);

  data->blip_counter++;

  if((data->blip_counter >= BLIPRATE)
     ||
     (data->number_of_packets_processed >= RUNLENGTH)) {

    data->blip_counter = 0;

    percentagedone =
      100 * (double) data->number_of_packets_processed/RUNLENGTH;

    fprintf(stderr, "%3.0f%% ", percentagedone);

    fprintf(stderr, "Successfully Xmtted Pkts  = %ld (Arrived Pkts = %ld) \r", 
	   data->number_of_packets_processed, 
	   data->arrival_count);

    fflush(stderr);
  }
}

/**********************************************************************/

void output_results(Simulation_Run_Ptr this_simulation_run)
{
  int i;
  double service_fraction;
  double throughput;
  double mean_delay;
  double station_mean_delay;
  const char *backoff_mode;
  Simulation_Run_Data_Ptr sim_data;

  sim_data = (Simulation_Run_Data_Ptr) simulation_run_data(this_simulation_run);
#if BINARY_EXPONENTIAL_BACKOFF
  backoff_mode = "binary_exponential";
#else
  backoff_mode = "fixed_mean";
#endif

  service_fraction = (double) sim_data->number_of_packets_processed /
    sim_data->arrival_count;
  throughput = (double) sim_data->number_of_packets_processed /
    simulation_run_get_time(this_simulation_run);
  mean_delay = sim_data->accumulated_delay /
    sim_data->number_of_packets_processed;

  for(i=0; i<NUMBER_OF_STATIONS; i++) {
    if((sim_data->stations+i)->packet_count > 0) {
      station_mean_delay = (sim_data->stations+i)->accumulated_delay /
        (sim_data->stations+i)->packet_count;
    } else {
      station_mean_delay = 0.0;
    }

    printf("%u,%.6f,%s,%d,%d,%ld,%ld,%.5f,%.7f,%.3f,%d,%.3f,%.3f\n",
      sim_data->random_seed,
      PACKET_ARRIVAL_RATE,
      backoff_mode,
      MEAN_BACKOFF_DURATION,
      NUMBER_OF_STATIONS,
      sim_data->arrival_count,
      sim_data->number_of_packets_processed,
      service_fraction,
      throughput,
      mean_delay,
      i,
      station_mean_delay,
      (double) sim_data->number_of_collisions /
        sim_data->number_of_packets_processed);
  }
}



