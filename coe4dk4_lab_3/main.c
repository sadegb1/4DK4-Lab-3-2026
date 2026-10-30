
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "output.h"
#include "trace.h"
#include "simparameters.h"
#include "cleanup.h"
#include "packet_arrival.h"
#include "packet_transmission.h"
#include "main.h"

/*******************************************************************************/

static void
simulation_stop_event(Simulation_Run_Ptr simulation_run, void * ptr)
{
  Simulation_Run_Data_Ptr data;
  int i;

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);
  data->arrivals_done = 1;
  data->backlog_at_cutoff = fifoqueue_size(data->data_buffer);
  for(i = 0; i < NUMBER_OF_STATIONS; i++) {
    data->backlog_at_cutoff += fifoqueue_size(data->stations[i].buffer);
  }
  if(data->backlog_at_cutoff == 0 && !data->data_channel_busy) {
    data->simulation_done = 1;
  }
}

int
main(void)
{
  unsigned random_seed;
  unsigned RANDOM_SEEDS[] = {RANDOM_SEED_LIST, 0};
  double arrival_rates[] = {PACKET_ARRIVAL_RATE_LIST};
  double reservation_ratios[] = {RESERVATION_RATIO_LIST};

  Simulation_Run_Ptr simulation_run;
  Simulation_Run_Data data;
  int i, seed_index;
  size_t rate_index, ratio_index;

  printf("seed,arrival_rate,mean_packet_duration,reservation_slot_duration,X_over_Xr,attempt_probability,arrivals,completed_by_horizon,completed_after_drain,throughput,mean_delay,reservations,collision_slots,backlog_at_horizon\n");

  for(rate_index = 0;
      rate_index < sizeof(arrival_rates) / sizeof(arrival_rates[0]);
      rate_index++) {
    for(ratio_index = 0;
        ratio_index < sizeof(reservation_ratios) / sizeof(reservation_ratios[0]);
        ratio_index++) {
      seed_index = 0;
      while((random_seed = RANDOM_SEEDS[seed_index++]) != 0) {
        random_generator_initialize(random_seed);

        simulation_run = (Simulation_Run_Ptr) simulation_run_new();
        simulation_run_set_data(simulation_run, (void *) &data);

        data.stations = (Station_Ptr) xcalloc(NUMBER_OF_STATIONS,
                                              sizeof(Station));
        data.data_buffer = fifoqueue_new();
        data.blip_counter = 0;
        data.arrival_count = 0;
        data.number_of_packets_processed = 0;
        data.number_completed_in_window = 0;
        data.reservation_successes = 0;
        data.reservation_collisions = 0;
        data.backlog_at_cutoff = 0;
        data.accumulated_delay = 0.0;
        data.packet_arrival_rate = arrival_rates[rate_index];
        data.reservation_slot_duration = MEAN_PACKET_DURATION /
          reservation_ratios[ratio_index];
        data.data_channel_busy = 0;
        data.arrivals_done = 0;
        data.simulation_done = 0;
        data.random_seed = random_seed;

        for(i = 0; i < NUMBER_OF_STATIONS; i++) {
          data.stations[i].id = i;
          data.stations[i].buffer = fifoqueue_new();
        }

        {
          double first_arrival = exponential_generator(
            1.0 / data.packet_arrival_rate);
          if(first_arrival < SIMULATION_DURATION) {
            schedule_packet_arrival_event(simulation_run, first_arrival);
          }
        }
        schedule_reservation_slot_event(simulation_run,
          data.reservation_slot_duration);

        {
          Event stop_event;
          stop_event.description = "Simulation Stop";
          stop_event.function = simulation_stop_event;
          stop_event.attachment = NULL;
          simulation_run_schedule_event(simulation_run, stop_event,
                                        SIMULATION_DURATION);
        }

        while(!data.simulation_done) {
          simulation_run_execute_event(simulation_run);
        }

        output_results(simulation_run);
        cleanup(simulation_run);
      }
    }
  }

  return 0;
}












