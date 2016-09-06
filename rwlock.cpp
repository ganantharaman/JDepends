#include <rwlock.h>

typedef struct rwlock_t {
	int readers;

	lock_t read_lock;
	lock_t write_lock;

	(*reader_read)();
	(*writer_write)();
	(*writer_done)();
	(*reader_done)();
}
`	

reader_read() {
	acquire_lock(read_lock);
	readers++;
	release_lock(read_lock);
}

reader_done() {
    acquire_lock(read_lock);
	readers--;
	release_lock(read_lock);
}

writer_write() {    
	int check_count = 0;
	int num_checks = 2;

	do {
	acquire_lock(read_lock);
	if(readers > 0 ) {
		// be nice and wait until the reader_finishes
		release_lock(&readlock);
		sleep(2000);
		check_count ++;
	} else {
       // we hold still the read_lock
       break; 
	}
	} while(check_count < num_checks);

	acquire_lock(read_lock);
	acquire_lock(write_lock);
	if(writers == 0) {
	   writers++;
	   
	}
	writers++;
}

writer_done() {
  writers--;
  release_lock(write_lock);
  release_lock(read_lock);
}




    
}