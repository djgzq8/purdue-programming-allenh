/*
 * spawn_sulfate.c
 *
 *  Created on: Apr 28, 2012
 *      Author: ee469g19
 */
#include "spawn.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  message m;         // Used to access missile codes from mailbox
  //mbox_t h_mbox;           // Handle to the mailbox
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int so4_mbox;

  if (argc != 3) {
    Printf("\tUsage: %s <handle_to_so4_mailbox> <handle_to_page_mapped_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  so4_mbox = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);


  // Open the mailbox
  if (mbox_open(so4_mbox) == MBOX_FAIL) {
    Printf("\tspawn_sulfate (%d): Could not open the mailbox!\n", getpid());
    Exit();
  }

  // Wait for a message from the mailbox
  if (mbox_send(so4_mbox, sizeof(m), (void *)&m) == MBOX_FAIL) {
    Printf("\tspawn_sulfate (%d): Could not map the virtual address to the memory!\n", getpid());
    Exit();
  }

  // Now print a message to show that everything worked
  Printf("\tspawn_sulfate (%d): SO4 molcule created. Injecting it into atmosphere\n", getpid());

  Printf("\tspawn_sulfate (%d): Done!\n\n", getpid());
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("\tspawn_sulfate (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }


}
