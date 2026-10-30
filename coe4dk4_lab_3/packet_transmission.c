
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
#include "output.h"
#include "packet_transmission.h"

/*******************************************************************************/

long int
schedule_reservation_slot_event(Simulation_Run_Ptr simulation_run,
				Time event_time)
{
  Event event;

  event.description = "Reservation Mini-Slot";
  event.function = reservation_slot_event;
  event.attachment = NULL;

  return simulation_run_schedule_event(simulation_run, event, event_time);
}

/*******************************************************************************/

void
reservation_slot_event(Simulation_Run_Ptr simulation_run, void * ptr)
{
  Packet_Ptr packet;
  int i, attempts, selected_station, packets_waiting;
  Simulation_Run_Data_Ptr data;
  Time now;

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);
  now = simulation_run_get_time(simulation_run);
  attempts = 0;
  selected_station = -1;

  for(i = 0; i < NUMBER_OF_STATIONS; i++) {
    if(fifoqueue_size(data->stations[i].buffer) > 0 &&
       uniform_generator() < RESERVATION_ATTEMPT_PROBABILITY) {
      attempts++;
      selected_station = i;
    }
  }

  if(attempts == 1) {
    packet = (Packet_Ptr) fifoqueue_get(data->stations[selected_station].buffer);
    fifoqueue_put(data->data_buffer, packet);
    data->reservation_successes++;

    if(!data->data_channel_busy) {
      data->data_channel_busy = 1;
      schedule_data_transmission_end_event(simulation_run,
        now + packet->service_time, packet);
    }
  } else if(attempts > 1) {
    data->reservation_collisions++;
  }

  packets_waiting = 0;
  for(i = 0; i < NUMBER_OF_STATIONS; i++) {
    if(fifoqueue_size(data->stations[i].buffer) > 0) {
      packets_waiting = 1;
      break;
    }
  }
  if(!data->arrivals_done || packets_waiting) {
    schedule_reservation_slot_event(simulation_run,
      now + data->reservation_slot_duration);
  } else if(!data->data_channel_busy) {
    data->simulation_done = 1;
  } else {
    schedule_reservation_slot_event(simulation_run,
      now + data->reservation_slot_duration);
  }
}

/*******************************************************************************/

long int
schedule_data_transmission_end_event(Simulation_Run_Ptr simulation_run,
             Time event_time,
             void * packet)
{
  Event event;

  event.description = "End Of Data Packet";
  event.function = data_transmission_end_event;
  event.attachment = packet;

  return simulation_run_schedule_event(simulation_run, event, event_time);
}

/*******************************************************************************/

void
data_transmission_end_event(Simulation_Run_Ptr simulation_run, void * packet)
{
  Packet_Ptr completed_packet, next_packet;
  Simulation_Run_Data_Ptr data;
  Time now;

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);
  now = simulation_run_get_time(simulation_run);
  completed_packet = (Packet_Ptr) fifoqueue_get(data->data_buffer);
  data->number_of_packets_processed++;
  if(now <= SIMULATION_DURATION) {
    data->number_completed_in_window++;
  }
  data->accumulated_delay += now - completed_packet->arrive_time;
  free(completed_packet);
  output_blip_to_screen(simulation_run);

  if(fifoqueue_size(data->data_buffer) > 0) {
    next_packet = (Packet_Ptr) fifoqueue_see_front(data->data_buffer);
    schedule_data_transmission_end_event(simulation_run,
      now + next_packet->service_time, next_packet);
  } else {
    data->data_channel_busy = 0;
  }

}



