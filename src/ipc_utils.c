#define _GNU_SOURCE

#include <string.h>
#include "header/ipc_utils.h"

/* Data structures */
struct data_general *general = NULL;
struct data_ship *ships = NULL;
struct data_port *ports = NULL;
struct data_cargo *cargo = NULL;

/* Shared memory ids */
int id_shm_general;
int id_shm_ship;
int id_shm_port;
int id_shm_cargo;

/* Semaphore ids */
int id_sem_start;

void initialize_shm(struct data_general *data)
{
	id_sem_start = sem_create(SEM_START_KEY, 1);

	id_shm_general = shm_create(SHM_DATA_GENERAL_KEY, sizeof(*general));
	general = shm_attach(id_shm_general);
	memcpy(general, data, sizeof(*data));

	id_shm_ship = shm_create(SHM_DATA_SHIPS_KEY, (sizeof(*ships) * general->so_navi));
	id_shm_port = shm_create(SHM_DATA_PORTS_KEY, (sizeof(*ports) * general->so_porti));
	id_shm_cargo = shm_create(SHM_DATA_CARGO_KEY, (sizeof(*cargo) * general->so_merci));

	ships = shm_attach(id_shm_ship);
	ports = shm_attach(id_shm_port);
	cargo = shm_attach(id_shm_cargo);
}

void attach_process_to_shm()
{
	id_sem_start = sem_create(SEM_START_KEY, 1);
	sem_execute_semop(id_sem_start, 0, 0, 0);

	/* Attaching to general for simulation constants */
	id_shm_general = shm_create(SHM_DATA_GENERAL_KEY, sizeof(*general));
	general = shm_attach(id_shm_general);

	/* Attaching to other segments */
	id_shm_ship = shm_create(SHM_DATA_SHIPS_KEY, (sizeof(*ships) * general->so_navi));
	id_shm_port = shm_create(SHM_DATA_PORTS_KEY, (sizeof(*ports) * general->so_porti));
	id_shm_cargo = shm_create(SHM_DATA_CARGO_KEY, (sizeof(*cargo) * general->so_merci));

	ships = shm_attach(id_shm_ship);
	ports = shm_attach(id_shm_port);
	cargo = shm_attach(id_shm_cargo);
}

/* Getters */
int get_general_shm_id(){return id_shm_general;}
int get_ship_shm_id(){return id_shm_ship;}
int get_cargo_shm_id(){return id_shm_cargo;}

pid_t get_ship_pid(int ship_id){return ships[ship_id].pid;};
struct coordinates get_ship_coords(int ship_id){return ships[ship_id].coord;};


/* Setters */
void set_ship_pid(int ship_id, pid_t ship_pid){ships[ship_id].pid = ship_pid;}
void set_ship_coords(int ship_id, struct coordinates coords){ships[ship_id].coord = coords;}

double get_constant(int const_num)
{
	switch (const_num % 16) {
	case 0  :return general->so_lato;
	case 1 : return general->so_days;
	case 2 : return general->so_navi;
	case 3 : return general->so_speed;
	case 4 : return general->so_capacity;
	case 5 : return general->so_porti;
	case 6 : return general->so_banchine;
	case 7 : return general->so_fill;
	case 8 : return general->so_loadspeed;
	case 9 : return general->so_merci;
	case 10: return general->so_size;
	case 11 : return general->so_min_vita;
	case 12 : return general->so_max_vita;
	case 13 : return general->so_storm_duration;
	case 14 : return general->so_swell_duration;
	case 15 : return general->so_maelstrom;
	}
}

void start_simulation(){sem_setval(id_sem_start, 0, 0);}

void delete_all_shm()
{
	shm_delete(id_shm_general);
	shm_delete(id_shm_ship);
	shm_delete(id_shm_port);
	shm_delete(id_shm_cargo);
	sem_delete(id_sem_start);
}
