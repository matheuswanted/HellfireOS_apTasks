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
		int err = hf_spawn(task, 0, 3, 0, "task ap", 2048);
    }
}

void app_main(void){
	hf_spawn(ap_producer, 0, 0, 0, "task producer", 2048);
}
