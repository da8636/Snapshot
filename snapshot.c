/**
 * comp2129 - assignment 2
 * Deanna Arora
 * daro9707
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <ctype.h>

#include "snapshot.h"

entry* entry_head = NULL;
entry* entry_tail = NULL;

snapshot* snapshot_head = NULL;
snapshot* snapshot_tail = NULL;
snapshot* snapshot_current = NULL;



char line[MAX_LINE]; // do something about this
int getid = 1;

void delete_entry(entry* deleting_entry){
	//edge case 1: deleting the one and only entry
	if ((deleting_entry == entry_head) && (deleting_entry == entry_tail)){
		entry_head = NULL;
		entry_tail = NULL;

	}
	//edge case 2: deleting the entry at head
	else if (deleting_entry == entry_head){

		entry_head = deleting_entry->next;
		entry_head->prev = NULL;


	}
	//edge case 3: deleting the entry at tail
	else if (deleting_entry == entry_tail){

		entry_tail = deleting_entry->prev;
		entry_tail->next = NULL;

	}
	//main case: deleting entry otherwise
	else{
		entry* temporary_prev = deleting_entry->prev;
		entry* temporary_next = deleting_entry->next;

		deleting_entry->prev->next = temporary_next;
		temporary_next->prev = temporary_prev;
	}

	//reassign length and free the memory of the entry and values
	deleting_entry->length = 0;
	free(deleting_entry->values);
	free(deleting_entry);
}

//exits the program on command of "bye"
void bye_command() {

	//entries and values of current state.
	entry* entry_current = entry_head;

	while (entry_current != NULL){
		entry* entry_todelete = entry_current;
		entry_current = entry_current->next;
		delete_entry(entry_todelete);
	}

	snapshot* working_snapshot = snapshot_head;

	while (working_snapshot != NULL){
		entry_current = working_snapshot->entries;
		while (entry_current != NULL){
			entry* entry_todelete = entry_current;
			entry_current = entry_current->next;
			free(entry_todelete->values);
			free(entry_todelete);
		}
		snapshot* to_delete = working_snapshot;
		working_snapshot = working_snapshot->next;
		free(to_delete);
	}

	//snapshots

	printf("bye\n");
	exit(0);
}
//provides assistance on command of "help"
void command_help() {

		printf("%s\n", HELP);

}

//sets the value to the corresponding entry
void set_value(entry* entry, char* key){

	char* set_keyvalue = strtok(NULL, " ");
	int counter = 1;

	//allocates memory for the new values to be initialised
	entry->values = malloc(sizeof(int));

	//reallocate memory for all values
	while (set_keyvalue != NULL){
		int set_intvalue =	atoi(set_keyvalue);

		entry->values = realloc(entry->values, sizeof(int)*counter);
		entry->values[counter-1] = set_intvalue;

		counter++;
		set_keyvalue = strtok(NULL, " ");
	}

	//setting the length and getting a copy of the key.
	entry->length = counter-1;
	strcpy(entry->key, key);
	printf("ok\n");

}

//sets a entry to the current snapshot and calls
//set_value() to set the values of the entry.
void set_command() {

	char* set_key = strtok(NULL, " ");

	entry* entry_current = entry_head;
	//going through entries to find the entry corresponding to the key
	//and once found re-setting the values
	while (entry_current != NULL){

		if(strcmp(entry_current->key, set_key) == 0){
			free(entry_current->values);
			set_value(entry_current, set_key);
			return;
		}
		entry_current = entry_current->next;
	}


	//allocate memory to create a single entry
	entry* new_entry = malloc(sizeof(entry));

	//if the first entry being set, then set head and tail to entry
	if (entry_head == NULL){

		entry_head = new_entry;
		entry_head->prev = NULL;

		entry_tail = new_entry;
		entry_tail->next = NULL;

	}
	//if their are other exisiting entries and setting a new one
	//entry added to the end and connected so that it is the tail
	else{

		entry_tail->next = new_entry;
		new_entry->prev = entry_tail;

		entry_tail = new_entry;
		entry_tail->next = NULL;
	}
	//set the values again of the new entries
	set_value(new_entry, set_key);

}

entry* copy_entry(entry* entry_toCopy){
	entry* e = malloc(sizeof(entry));
	strcpy(e->key, entry_toCopy->key);

	e->values = malloc(sizeof(int)*entry_toCopy->length);
	memcpy(e->values, entry_toCopy->values, sizeof(int)*entry_toCopy->length);

	e->length = entry_toCopy->length;
	return e;
}


void snapshot_command(){
	snapshot* snapshot_current = malloc(sizeof(snapshot));
	snapshot_current->id = getid;
	getid++;
	//if no snapshots exist
	if (snapshot_head == NULL){
		snapshot_head = snapshot_current;
		snapshot_head->prev = NULL;
		snapshot_tail = snapshot_current;
		snapshot_tail->next = NULL;

	}
	else{

		//SNAPSHOT NOTATION
		snapshot_tail->next = snapshot_current;
		snapshot_current->prev = snapshot_tail;


		snapshot_tail = snapshot_current;
		snapshot_tail->next = NULL;

	}

	//SAVING THE STATE
	entry* entry_current = entry_head;
	entry* snapshot_prev_entry = copy_entry(entry_head);
	snapshot_current->entries = snapshot_prev_entry;
	entry_current = entry_current->next;


	while (entry_current != NULL){
		entry* snapshot_current_entry = copy_entry(entry_current);
		snapshot_prev_entry->next = snapshot_current_entry;
		snapshot_current_entry->prev = snapshot_prev_entry;

		snapshot_prev_entry = snapshot_current_entry;
		snapshot_current_entry = snapshot_current_entry->next;

		entry_current = entry_current->next;
	}

	snapshot_prev_entry->next = NULL;
 	printf("saved as snapshot %d\n", getid-1);

}


void list_snapshots(void){
	snapshot* snapshot_current = snapshot_tail;
	if (snapshot_current == NULL){
		printf("no snapshots\n");
		return;
	}
	while (snapshot_current != NULL){
		printf("%d\n", snapshot_current->id);
		snapshot_current = snapshot_current->prev;
	}

}

snapshot* get_snapshot(int id){
	snapshot* working_snapshot = snapshot_head;
	while (working_snapshot != NULL){
		if (working_snapshot->id == id){
			return working_snapshot;
		}
		working_snapshot = working_snapshot->next;
	}

	return NULL;
}

void checkout_command(void){
	char* set_id = strtok(NULL, " \n");
	int set_intid =	atoi(set_id);
	snapshot* working_snapshot = get_snapshot(set_intid);
	if (working_snapshot == NULL){
		printf("no such snapshot\n");
		return;
	}

	entry* entry_current = entry_head;

	//deletes the current state.
	while (entry_current != NULL){
		entry* entry_todelete = entry_current;
		entry_current = entry_current->next;
		delete_entry(entry_todelete);
	}
	if (working_snapshot->entries == NULL){
		printf("ok\n");
		return;
	}
	entry_head = copy_entry(working_snapshot->entries);
	//entry_current = entry_head->next;
	entry* entry_prev = entry_head;
	entry_head->prev = NULL;

	entry* snapshot_current_entry = working_snapshot->entries;
	snapshot_current_entry = snapshot_current_entry->next;
	entry_tail = entry_head;

	while (snapshot_current_entry != NULL){
		entry_current = copy_entry(snapshot_current_entry);
		entry_prev->next = entry_current;
		entry_current->prev = entry_prev;
		entry_tail = entry_current;

		//shifting over
		entry_prev = entry_current;
		entry_current = entry_current->next;
		snapshot_current_entry = snapshot_current_entry->next;
	}

	entry_tail->next = NULL;
	printf("ok\n");

}

void rollback_command(void){
	char* set_id = strtok(NULL, " \n");
	int set_intid =	atoi(set_id);
	snapshot* working_snapshot = get_snapshot(set_intid);
	if (working_snapshot == NULL){
		printf("no such snapshot\n");
		return;
	}

	entry* entry_current = entry_head;

	while (entry_current != NULL){
		entry* entry_todelete = entry_current;
		entry_current = entry_current->next;
		delete_entry(entry_todelete);
	}

	entry_head = copy_entry(working_snapshot->entries);
	//entry_current = entry_head->next;
	entry* entry_prev = entry_head;
	entry_head->prev = NULL;
	entry* snapshot_current_entry = working_snapshot->entries->next;
	entry_tail = entry_head;

	//checking out to snapshot
	while (snapshot_current_entry != NULL){
		entry_current = copy_entry(snapshot_current_entry);
		entry_prev->next = entry_current;
		entry_current->prev = entry_prev;
		entry_tail = entry_current;

		//shifting over
		entry_prev = entry_current;
		entry_current = entry_current->next;
		snapshot_current_entry = snapshot_current_entry->next;
	}
	entry_tail->next = NULL;

	//snapshot_tail is the one that has stuff deleted afterwards
	snapshot_tail = working_snapshot;
	working_snapshot = working_snapshot->next;
	snapshot_tail->next = NULL;
	//entry_current = working_snapshot->entries;

	while (working_snapshot != NULL){
		entry_current = working_snapshot->entries;
		while (entry_current != NULL){
			entry* entry_todelete = entry_current;
			entry_current = entry_current->next;
			free(entry_todelete->values);
			free(entry_todelete);
		}
		snapshot* to_delete = working_snapshot;
		working_snapshot = working_snapshot->next;
		free(to_delete);
	}


	printf("ok\n");

}


void drop_command(void){
	char* set_id = strtok(NULL, " \n");
	int set_intid =	atoi(set_id);
	snapshot* working_snapshot = get_snapshot(set_intid);

	if (working_snapshot == NULL){
		printf("no such snapshot\n");
		return;
	}
	//edge case 1: deleting the one and only entry
	if ((working_snapshot == snapshot_head) && (working_snapshot == snapshot_tail)){
		snapshot_head = NULL;
		snapshot_tail = NULL;
	}
	//edge case 2: deleting the entry at head
	else if (working_snapshot == snapshot_head){

		snapshot_head = working_snapshot->next;
		snapshot_head->prev = NULL;
	}
	//edge case 3: deleting the entry at tail
	else if (working_snapshot == snapshot_tail){

		snapshot_tail = working_snapshot->prev;
		snapshot_tail->next = NULL;

	}
	//main case: deleting entry otherwise
	else{
		snapshot* temporary_prev = working_snapshot->prev;
		snapshot* temporary_next = working_snapshot->next;

		working_snapshot->prev->next = temporary_next;
		temporary_next->prev = temporary_prev;
	}
	entry* entry_current = working_snapshot->entries;
	entry* entry_todelete = entry_current;

	while (entry_current != NULL){
		entry_current = entry_current->next;
		free(entry_todelete->values);
		free(entry_todelete);
		entry_todelete = entry_current;
	}
	free(working_snapshot);
	printf("ok\n");
}

//called to supply entry with key input without duplication of code
entry* get_entry(char* key){
	entry* entry_current = entry_tail;

	//goes through the entries
	while (entry_current != NULL){
		//locates the corresponding entry to the key and returns it
		if (strcmp(entry_current->key, key) == 0){
			return entry_current;
		}
		entry_current = entry_current->prev;
	}
	//if entry not found
	return NULL;
}

void purge_command(){
	char* set_key = strtok(NULL, " \n");
	entry* entry_current = get_entry(set_key);
	if (entry_current != NULL){
		delete_entry(entry_current);
	}
	if (snapshot_head == NULL){
		printf("ok\n");
		return;
	}
	snapshot* working_snapshot = snapshot_head;


	while (working_snapshot != NULL){
		entry_current = working_snapshot->entries;
		while (entry_current != NULL){
			if (strcmp(entry_current->key, set_key) == 0){
				if ((entry_current->prev == NULL) && (entry_current->next == NULL)){
					free(working_snapshot->entries->values);
					free(working_snapshot->entries);
					working_snapshot->entries = NULL;
				}
				else if ((entry_current->prev == NULL) && (entry_current->next != NULL)){
					entry* temp_entry = entry_current;
					entry_current = entry_current->next;
					entry_current->prev = NULL;
					free(temp_entry->values);
					free(temp_entry);
				}
				else if ((entry_current->prev != NULL) && (entry_current->next != NULL)){
					entry* temp_entry = entry_current;
					temp_entry->prev->next = temp_entry->next;
					temp_entry->next->prev = temp_entry->prev;
					free(temp_entry->values);
					free(temp_entry);
				}
				else if ((entry_current->prev != NULL) && (entry_current->next == NULL)){
					entry* temp_entry = entry_current;
					temp_entry->prev->next = NULL;
					free(temp_entry->values);
					free(temp_entry);
				}
				break;
			}
			entry_current = entry_current->next;
		}

		working_snapshot = working_snapshot->next;
	}
	printf("ok\n");

}

//prints values in an entry in the correct form
void print_values(entry* entry){
	//checking for NULL variable
	if (entry == NULL){

		printf("no such entry" );
		return;
	}

	//printing values
	printf("[");

	//going through values of entry
	for(int i = 0; i<(entry->length); i++){

		//if the last value then print without following space
		if (i == ((entry->length)-1)){
			printf("%d", entry->values[i]);
		}
		//print with following space
		else{
			printf("%d ",entry->values[i]);
		}
	}

	printf("]\n");

}


//displays the values with the corresponding key
void get_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}
	//prints the entry values
	print_values(working_entry);

}

//prints all the keys.
void list_keys(){

	//checking for NULL variable
	if (entry_tail == NULL){
		printf("no keys\n");
	}
	entry* entry_current = entry_tail;

	//prints all the keys
	while (entry_current != NULL){
		printf("%s\n", entry_current->key);
		entry_current = entry_current->prev;
	}
}


//prints all the entries of keys and values
void list_entries(){

	//checking for NULL variable
	if (entry_head == NULL){

		printf("no entries\n");
	}
	entry* entry_current = entry_tail;

	while (entry_current != NULL){

		//prints the keys and corresponding values
		printf("%s ", entry_current->key);
		print_values(entry_current);
		entry_current = entry_current->prev;
	}
}


//adds a value at the end of the entry values
void append_command(){

	 //recieves key and retrieves corresponding entry
	 char* get_key = strtok(NULL, " ");
	 entry* working_entry = get_entry(get_key);

	 //checking for NULL variable
	 if (working_entry == NULL){
		 printf("no such key\n");
		 return;
	 }
	 else{

		int counter = working_entry->length;
		char* set_keyvalue = strtok(NULL, " \n");

		//reallocates memory to allow for the additional value
		//sets the last memory spot as the new value and
		//reassigns the current length of entry values
	 	while (set_keyvalue != NULL){
	 		int set_intvalue =	atoi(set_keyvalue);

	 		working_entry->values = realloc(working_entry->values, sizeof(int)*(counter+1));
	 		working_entry->values[counter] = set_intvalue;

	 		counter++;
	 		set_keyvalue = strtok(NULL, " \n");
	 	}
		working_entry->length = counter;

		//once task completed
		printf("ok\n");
	 }

 }

void push_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " ");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}
	else{
		int counter = working_entry->length;
		char* push_keyvalue = strtok(NULL, " \n");

		while (push_keyvalue != NULL){

			int push_intvalue =	atoi(push_keyvalue);
			working_entry->values = realloc(working_entry->values, sizeof(int)*(counter+1));
			// for (int i = 0; i< counter; i ++){
			//
			// }
			for (int i = counter; i > 0; i--){
				working_entry->values[i] = working_entry->values[i-1];
			}
			working_entry->values[0] = push_intvalue;
			counter += 1;
			working_entry->length = counter;

			push_keyvalue = strtok(NULL, " \n");
		}
		printf("ok\n");
		//printf("length: %zu\n",working_entry->length );
	}
}

//displays the value according to index
void display_value(char* key, int index){

	printf("%d\n", (get_entry(key)->values[index-1]));

}

//picks an index in a entry and displays it
void pick_command(){
	char* get_key = strtok(NULL, " ");
	entry* working_entry = get_entry(get_key);
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	char* get_index = strtok(NULL, " ");
	int get_intindex = atoi(get_index);

	//checks if invalid input
	if ((get_intindex == 0)|| (get_intindex > working_entry->length)){
		printf("index out of range\n");
		return;
	}

	//displays the picked value
	display_value(get_key, get_intindex);
}

//removes the first value and displays it
void pop_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}
	//checking if nothing to pop
	if (working_entry->length == 0){
		printf("nil\n");
		return;
	}

	//display value being popped
	display_value(get_key, 1);

	int counter = working_entry->length;

	//shifting everything over 1 to the left
	for (int i = 0; i < counter-1; i++){
		working_entry->values[i] = working_entry->values[i+1];
	}

	//reallocates the memory to reduce the memory by 1 and reassigns the length
	working_entry->values = realloc(working_entry->values, sizeof(int)*(counter-1));
	working_entry->length = counter-1;
}


//plucks the value at given index and reallocs memory
void pluck(entry* entry, int index){

	int counter = entry->length;

	//shifts all the values to cover the value we want to pluck
	for (int i = index-1; i < counter-1; i++){
		entry->values[i] = entry->values[i+1];
	}

	//reallocates the memory to reduce the memory by 1 and reassigns the length
	entry->values = realloc(entry->values, sizeof(int)*(counter-1));
	entry->length = counter-1;

}

//removes a value at given index and displays it
//by calling pluck() and display_value()
void pluck_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " ");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	char* get_index = strtok(NULL, " \n");
	int get_intindex = atoi(get_index);

	if ((get_intindex == 0)|| (get_intindex > working_entry->length)){
		printf("index out of range\n");
		return;
	}
	//delivers the functions needs
	display_value(get_key, get_intindex);
	pluck(working_entry, get_intindex);
}


//deletes the given entry and its values
void del_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* deleting_entry = get_entry(get_key);

	//checking for NULL variable
	if (entry_head == NULL){
		printf("no such key\n");
		return;
	}

	delete_entry(deleting_entry);



	//completes task
	printf("ok\n");
}

//comparsion function to utilise qsort()
int comparsionfunction (const void * x1, const void * x2)
{
   return ( *(int*)(x1) - *(int*)(x2) );
}
//sorts values in an entry permanently
void sort_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	//utilises the qsort function to sort
	qsort(working_entry->values, working_entry->length, sizeof(int), comparsionfunction);
	printf("ok\n");
}

//retrieves the min value
void min_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	//setting initial minvalue to first value for comparsion
	int minvalue = working_entry->values[0];

	//goes through values to find the min values
	for (int i = 1; i < working_entry->length; i++){

		if (working_entry->values[i] < minvalue)
		minvalue = working_entry->values[i];
	}

	//once minvalue found, printed
	printf("%d\n", minvalue);
}

void max_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}
	//setting initial maxvalue to first value for comparsion
	int maxvalue = working_entry->values[0];

	//goes through all values to find maxvalue
	for (int i = 1; i < working_entry->length; i++){

		if (working_entry->values[i] > maxvalue)
		maxvalue = working_entry->values[i];
	}

	//once maxvalue found, printed
	printf("%d\n", maxvalue);
}



//displays the sum of all the values in an entry
void sum_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	int sum = 0;

	//goes through and adds all the values
	for (int i = 0; i < working_entry->length; i++){
		sum += working_entry->values[i];
	}

	//once sum found, printed
	printf("%d\n", sum);
}


//displays the number of values in an entry
void len_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	//prints the length of the entry
	printf("%zu\n", working_entry->length);
}

//reverses the values in an entry permanently
void rev_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	int i = 0; //front
	int j = (working_entry->length) - 1; //back
	int temporary;
	//goes through and reverses the array of values
	while (i < j){
		temporary = working_entry->values[i];
		working_entry->values[i] = working_entry->values[j];
		working_entry->values[j] = temporary;
		i++;
		j--;
	}
	printf("ok\n");
}

//removes all unique values adjacent permanently in the entry
void uniq_command(){

	//recieves key and retrieves corresponding entry
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);

	//checking for NULL variable
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	//goes through and if a repeat is encountered it is plucked out (removed)
	for (int i = 0; i < working_entry->length-1; i++){

		if (working_entry->values[i] == working_entry->values[i+1]){
			pluck(working_entry, i+1);
			i--;
		}

	}
	printf("ok\n");
}

int* sort_unique(int* length, int* values){

	int* setvalues = malloc(sizeof(int)*(*length));

	memcpy(setvalues, values, sizeof(int)*(*length));

	qsort(setvalues, *length, sizeof(int), comparsionfunction);


	for (int i = 0; i < *length-1; i++){

		if (setvalues[i] == setvalues[i+1]){
			for (int j = i; j < *length-1; j++){
				setvalues[j] = setvalues[j+1];

			}
			setvalues = realloc(setvalues, sizeof(int)*(*length-1));
			*length = *length-1;
			i--;
		}

	}
	free(values);
	return setvalues;

}
void printsets(int length, int* values){
	printf("[");

	//going through values of entry
	for(int i = 0; i<(length); i++){

		//if the last value then print without following space
		if (i == ((length)-1)){
			printf("%d", values[i]);
		}
		//print with following space
		else{
			printf("%d ", values[i]);
		}
	}

	printf("]\n");
}

void union_command(void){
	char* get_key = strtok(NULL, " \n");

	int* unionvalues = malloc(sizeof(int));
	int unionlength = 0;


	//add all values into the array
	while (get_key != NULL){

		entry* working_entry = get_entry(get_key);
		if (working_entry == NULL){
			printf("no such key\n");
			free(unionvalues);
			return;
		}
		//printf("%s\n", get_key);
		unionlength += working_entry->length;
		unionvalues = realloc(unionvalues, sizeof(int)*unionlength);

		int* newStart = unionvalues + (unionlength-(working_entry->length));
		memcpy(newStart, working_entry->values, sizeof(int)*working_entry->length);

		get_key = strtok(NULL, " \n");
	}


	unionvalues = sort_unique(&unionlength, unionvalues);
	printsets(unionlength, unionvalues);
	free(unionvalues);

}

void intersect_command(void){
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);
	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}

	int* intersectvalues = malloc(sizeof(int)* working_entry->length);
	int intersectlength = working_entry->length;

	memcpy(intersectvalues, working_entry->values, sizeof(int)*working_entry->length);

	get_key = strtok(NULL, " \n");

	//add all values into the array
	while (get_key != NULL){

		entry* working_entry = get_entry(get_key);
		if (working_entry == NULL){
			printf("no such key\n");
			return;
		}

		bool toPluck = true;

		for (int i = 0; i < intersectlength; i++){
			for (int j = 0; j < working_entry->length; j++){
				if (intersectvalues[i] == working_entry->values[j]){
					toPluck = false;
				}
			}

			if (toPluck){

				//shifts all the values to cover the value we want to pluck
				for (int k = i; k < intersectlength-1; k++){
					intersectvalues[k] = intersectvalues[k+1];
				}

				//reallocates the memory to reduce the memory by 1 and reassigns the length
				intersectvalues = realloc(intersectvalues, sizeof(int)*(intersectlength-1));
				intersectlength = intersectlength-1;
				i--;
			}
			toPluck = true;
		}

		get_key = strtok(NULL, " \n");

	}
	intersectvalues = sort_unique(&intersectlength, intersectvalues);
	printsets(intersectlength, intersectvalues);

	free(intersectvalues);

}



void difference_command(void){
	char* get_key = strtok(NULL, " \n");
	entry* working_entry = get_entry(get_key);


	if (working_entry == NULL){
		printf("no such key\n");
		return;
	}
	int* unionvalues = malloc(sizeof(int)*working_entry->length);
	int unionlength = working_entry->length;

	memcpy(unionvalues, working_entry->values, sizeof(int)*working_entry->length);

	int* intersectvalues = malloc(sizeof(int)* working_entry->length);
	int intersectlength = working_entry->length;

	memcpy(intersectvalues, working_entry->values, sizeof(int)*working_entry->length);

	get_key = strtok(NULL, " \n");

	//add all values into the array
	while (get_key != NULL){

		entry* working_entry = get_entry(get_key);
		if (working_entry == NULL){
			printf("no such key\n");
			return;
		}

		bool toPluck = true;

		//union_command
		unionlength += working_entry->length;
		unionvalues = realloc(unionvalues, sizeof(int)*unionlength);

		int* newStart = unionvalues + (unionlength-(working_entry->length));
		memcpy(newStart, working_entry->values, sizeof(int)*working_entry->length);

		//intersect_command
		for (int i = 0; i < intersectlength; i++){
			for (int j = 0; j < working_entry->length; j++){
				if (intersectvalues[i] == working_entry->values[j]){
					toPluck = false;
				}
			}

			if (toPluck){

				//shifts all the values to cover the value we want to pluck
				for (int k = i; k < intersectlength-1; k++){
					intersectvalues[k] = intersectvalues[k+1];
				}

				//reallocates the memory to reduce the memory by 1 and reassigns the length
				intersectvalues = realloc(intersectvalues, sizeof(int)*(intersectlength-1));
				intersectlength = intersectlength-1;
				i--;
			}
			toPluck = true;
		}

		// does the DIFF
		for (int j = 0; j < intersectlength; j++){
			for (int i = 0; i < unionlength; i++){

				if (unionvalues[i] == intersectvalues[j]){
					for (int k = i; k < unionlength-1; k++){
						unionvalues[k] = unionvalues[k+1];
					}
					unionvalues = realloc(unionvalues, sizeof(int)*(unionlength-1));
					unionlength = unionlength-1;
					i--;
				}

			}
		}
		intersectvalues = realloc(intersectvalues, sizeof(int)*unionlength);
		intersectvalues = memcpy(intersectvalues, unionvalues, sizeof(int)*unionlength);
		intersectlength = unionlength;
		get_key = strtok(NULL, " \n");


	}

	unionvalues = sort_unique(&unionlength, unionvalues);
	printsets(unionlength, unionvalues);
	free(intersectvalues);
	free(unionvalues);

}










//changes all input to lowercase to handle case sensitivity
char* lowercase(char* line){

	int i = 0;

	while (line[i] != '\0'){
		line[i] = tolower(line[i]);
		i++;
	}

	return line;
}

int main(void) {
	//run a while loop to allow for multiple command
	while (true) {
		printf("> ");

		//if nothing is typed as command
		//exit program saying bye
		if (fgets(line, MAX_LINE, stdin) == NULL) {
			printf("bye\n");
			return 0;
		}else{
			//retrives the command
			char* command = strtok(line, " \n");
			//converts to lower for case sensitivity
			command = lowercase(command);
			//if any of the commands from below are called in command line, run functions.
			if (strcmp(command, "help") == 0){
			 	command_help();
			}
			else if (strcmp(command, "bye") == 0){
				bye_command();
			}
			else if (strcmp(command, "set") == 0){
				set_command();
			}
			else if (strcmp(command, "get") == 0){
				get_command();
			}
			else if (strcmp(command, "list") == 0){
				//retrieve the second parameter
				char* set_secondcommand = strtok(NULL, " ");
				set_secondcommand = lowercase(set_secondcommand);

				//run commands if second parameter is fulfilled
				if (strcmp(set_secondcommand, "keys\n") == 0){
					list_keys();

				}else if (strcmp(set_secondcommand, "entries\n") == 0){
					list_entries();

				}else if (strcmp(set_secondcommand, "snapshots\n") == 0){
					list_snapshots();
				}
			}
			else if (strcmp(command, "append") == 0){
				append_command();
			}
			else if (strcmp(command, "push") == 0){
				push_command();
			}
			else if (strcmp(command, "pick") == 0){
				pick_command();
			}
			else if (strcmp(command, "pop") == 0){
				pop_command();
			}
			else if (strcmp(command, "pluck") == 0){
				pluck_command();
			}
			else if (strcmp(command, "del") == 0){
				del_command();
			}
			else if (strcmp(command, "sort") == 0){
				sort_command();
			}
			else if (strcmp(command, "min") == 0){
				min_command();
			}
			else if (strcmp(command, "max") == 0){
				max_command();
			}
			else if (strcmp(command, "sum") == 0){
				sum_command();
			}
			else if (strcmp(command, "len") == 0){
				len_command();
			}
			else if (strcmp(command, "rev") == 0){
				rev_command();
			}
			else if (strcmp(command, "uniq") == 0){
				uniq_command();
			}
			else if (strcmp(command, "union") == 0){
				union_command();
			}
			else if (strcmp(command, "inter") == 0){
				intersect_command();
			}
			else if (strcmp(command, "diff") == 0){
				difference_command();
			}
			else if (strcmp(command, "snapshot") == 0){
				snapshot_command();
			}
			else if (strcmp(command, "checkout") == 0){
				checkout_command();
			}
			else if (strcmp(command, "rollback") == 0){
				rollback_command();
			}
			else if (strcmp(command, "drop") == 0){
				drop_command();
			}
			else if (strcmp(command, "purge") == 0){
				purge_command();
			}
			else{//expect for this
				printf("Invalid command\n");
			}


		}
		printf("\n");

  	}
	return 0;
}
