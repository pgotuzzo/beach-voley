#ifndef BEACH_VOLEY_CONSTANTS_H
#define BEACH_VOLEY_CONSTANTS_H

// SHARED MEMORIES
const char *const SHARED_MEMORY_PIDS_VECTOR = "tmp/shared_memory_pids_vector";
const char *const SHARED_MEMORY_POINTS_VECTOR = "tmp/shared_memory_points_vector";
// FIFOS
const char *const FIFO_FILE_MANAGER_RECEIVE_TASK = "tmp/receive_task";
const char *const FIFO_FILE_PARTNER_RESPONSE = "tmp/partner_response";
// SEMAPHORE
const char *const SEM_FILE_FIELD_ENTRANCE = "tmp/stadium_entrance";
const char *const SEM_FILE_FIELD_EXIT = "tmp/stadium_exit";
const char *const SEM_TURNSTILE = "tmp/turnstile";
// LOCKS
const char *const LOCK_FILE_PARTNER_REQUEST = "tmp/lock_partner_request";
const char *const LOCK_FILE_SHARED_VECTORS = "tmp/lock_shared_vectors";

#endif //BEACH_VOLEY_CONSTANTS_H
