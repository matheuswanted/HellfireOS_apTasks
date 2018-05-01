#include <hellfire.h>

void task(void){
	int32_t jobs, id;
	
	id = hf_selfid();
	for(;;){
		jobs = hf_jobs(id);
		printf("\n%s (%d)[%d][%d]", hf_selfname(), id, hf_jobs(id), hf_dlm(id));
		while (jobs == hf_jobs(id));
	}
}

void ap_producer(void){
	while(1){
		delay_ms(random() % 450 + 50);
		hf_spawn(task, 0, 10, 0, "task aperiodic", 2048);
	}
}

void app_main(void){
	hf_spawn(task, 4, 1, 4, "task a", 2048);
	hf_spawn(task, 8, 2, 8, "task b", 2048);
	hf_spawn(task, 12, 3, 12, "task c", 2048);
	hf_spawn(ap_producer, 0, 0, 0, "task c", 2048);
}
