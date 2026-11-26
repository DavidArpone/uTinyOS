#include "uTaskManager.h"
#include "freertos/idf_additions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uTaskList_t *head;

uTaskManagerError uTask_init(){
	
	head = malloc(sizeof(uTaskList_t));
	
	return uTASK_OK;
}

uTaskManagerError uTask_create(uTaskConfig_t taskConfig){
	
	
	BaseType_t xReturned = xTaskCreatePinnedToCore(
		taskConfig.entry,
		taskConfig.name,
		taskConfig.stackSize,
		taskConfig.parameters,
		taskConfig.priority,
		taskConfig.handle,
		taskConfig.coreID
	);
	
	if(xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY){
		
		return uTASK_ERROR;
		
	}
	
	uTaskInfo_t task;
	
	memcpy(task.name, taskConfig.name, sizeof(taskConfig.name));
	task.owner = taskConfig.owner;
	task.coreID = taskConfig.coreID;
	task.priority = taskConfig.priority;

	return uTASK_OK; 
	//uTask_registry(&task);
}

uTaskManagerError uTask_registry(uTaskInfo_t *node){
	
	uTaskList_t *newNode = malloc(sizeof(uTaskList_t));
	
	if(newNode == NULL){
		
		return uTASK_ERROR;
			
	}
	
	uTaskList_t *prev = NULL; 
	uTaskList_t *aux = head;
	
	(*newNode).next = NULL;
	
	memcpy(&(*newNode).info, node, sizeof(uTaskInfo_t));
	
	while( ((*newNode).info.owner < (*aux).info.owner) || (*aux).next == NULL){
		
		prev = aux;
		aux = (*aux).next;
	
	}
	
	if(prev == NULL){
		
		head = newNode;
		
	}else{
	
		(*prev).next = newNode;
	
	}
	
	(*newNode).next = aux;
	
	return uTASK_OK;
	
}