/*
 * spawn_rx1.c
 *
 *  Created on: Apr 28, 2012
 *      Author: ee469g12
 */
#include "usertraps.h"
#include "spawn.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  missile_code mc;         // Used to access missile codes from mailbox
  mbox_t h_mbox;           // Handle to the mailbox
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int h2o_mbox,h2_mbox,o2_mbox;

  if (argc != 5) {
    Printf("Usage: %s <handle_to_h2o_mailbox> <handle_to_h2_mailbox> <handle_to_o2_mailbox> <handle_to_page_mapped_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  h2o_mbox = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  h2_mbox = dstrtol(argv[1], NULL, 10);
  o2_mbox = dstrtol(argv[1], NULL, 10);
  s_procs_completed = dstrtol(argv[2], NULL, 10);

  // Open the mailbox
  if (mbox_open(h2o_mbox) == MBOX_FAIL) {
    Printf("spawn_rx1 (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }
  if (mbox_open(h2_mbox) == MBOX_FAIL) {
    Printf("spawn_rx1 (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }
  if (mbox_open(o2_mbox) == MBOX_FAIL) {
    Printf("spawn_rx1 (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }

  // Wait for a message from the mailbox
  if (mbox_recv(h2o_mbox, sizeof(mc), (void *)&mc) == MBOX_FAIL) {
    Printf("spawn_rx1 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }
  if (mbox_recv(h2o_mbox, sizeof(mc), (void *)&mc) == MBOX_FAIL) {
    Printf("spawn_rx1 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }
  if (mbox_send(h2_mbox, sizeof(mc), (void *)&mc) == MBOX_FAIL) {
    Printf("spawn_rx1 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }
  if (mbox_send(h2_mbox, sizeof(mc), (void *)&mc) == MBOX_FAIL) {
    Printf("spawn_rx1 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }
  if (mbox_send(o2_mbox, sizeof(mc), (void *)&mc) == MBOX_FAIL) {
    Printf("spawn_rx1 (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  // Now print a message to show that everything worked
  Printf("spawn_rx1 (%d): Reaction Completed\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("spawn_rx1 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("spawn_rx1 (%d): Done!\n", getpid());
}
