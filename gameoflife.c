#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <unistd.h>

#define WIDTH 50
#define HEIGHT 35
#define GENERATIONS 1000

// Data structures
typedef struct {int x; int y;} vec2;

typedef struct field_s {
	int *occupations;
	int *next_occupations;
} field_t;

// Prototypes
void DBG_print_field_content(field_t);
int rnd(int,int);
field_t field_init();
void add_bacteria(field_t*, vec2);
void kill_bacteria(field_t*, vec2);
vec2 get_free_position(field_t);
vec2 i2v2(int,int);
int v22i(vec2,int);
vec2 rnd_vec2(int,int);
int is_occupied(field_t, vec2);
void new_bacteria(field_t*);
void update(field_t*);
int clamp(int, int, int);
int surrounders(int*, vec2);
void field_free(field_t);
void new_bacteria_n(field_t*, uint);
void copy_field(field_t*);
	
// Main
int main(int argc, char *argv[]) {
	int current_generation = 0;
	field_t field = field_init();
	new_bacteria_n(&field, 50*10);
	
	while(current_generation < GENERATIONS)
	{
		clrscr();
		printf("Generation: %d\n\n", current_generation);
		DBG_print_field_content(field);
		update(&field);
		current_generation++;
		sleep(1);
	}
	
	field_free(field);
	return 0;
}

// Implementations
int clamp(int min, int val, int max) {
	if(val < min) return min;
	if(val > max) return max;
	return val;
}

void update(field_t* field) {
	int i;
	uint bacteria_neighboors;
	printf("\n");
	for(i=0;i<WIDTH*HEIGHT;i++)
	{
		bacteria_neighboors = surrounders(field->occupations, i2v2(i, WIDTH));
		//printf("%d", bacteria_neighboors);
		//if((i+1)%WIDTH==0) { printf("\n"); }
		
		switch(bacteria_neighboors)
		{
			case 0:
			case 1:
				if(is_occupied(*field, i2v2(i, WIDTH))) {
					field->next_occupations[i] = 0;
				}
			break;
			case 2:
			case 3:
				if((is_occupied(*field, i2v2(i, WIDTH))) || (!is_occupied(*field, i2v2(i, WIDTH)) && bacteria_neighboors == 3)) {
					field->next_occupations[i] = 1;
				}	
			break;
			default:
				if(is_occupied(*field, i2v2(i, WIDTH))) {
					field->next_occupations[i] = 0;
				}
			break;
		}
	}
	copy_field(field);
}

void copy_field(field_t* field) {
	int i;
	for(i=0;i<HEIGHT*WIDTH;i++)
	{
			field->occupations[i] = field->next_occupations[i];
	}
}

int surrounders(int* occ, vec2 pos) {
	int surrounders = 0;
	int i,j;
	vec2 _pos = pos;
	for(i=-1;i<2;i++)
	{
		for(j=-1;j<2;j++)
		{
			if(i==0 && j==0)
			{
				continue;
			}
			_pos.x = pos.x + i;
			_pos.y = pos.y + j;
			
			if(_pos.x<0 || _pos.y<0 || _pos.x>=HEIGHT || _pos.y>=WIDTH)
			{
				continue;
			}
			
			surrounders += occ[v22i(_pos, WIDTH)];
		}
	}
	return surrounders;
}

void new_bacteria_n(field_t* field, uint num) {
	for(;num>0;num--)
	{
		new_bacteria(field);
	}
}

void new_bacteria(field_t* field) {
	vec2 new_pos = get_free_position(*field);
	add_bacteria(field, new_pos);
}

int rnd(int min,int max) {
	return (int)rand()%(max-min)+min;
}

field_t field_init() {
	int i,j=0;
	field_t f;
	f.occupations = (int*) malloc(WIDTH*HEIGHT*sizeof(int));
	f.next_occupations = (int*) malloc(WIDTH*HEIGHT*sizeof(int));
	if(f.occupations == NULL || f.next_occupations == NULL)
	{
		printf("ERROR: Memory allocation failed");
		exit(1);
	}
	for(i=0;i<WIDTH*HEIGHT;i++)
	{
		f.occupations[i] = 0;
		f.next_occupations[i] = 0;
	}
	return f;
}

void field_free(field_t field) {
	free(field.occupations);
	free(field.next_occupations);
}

void add_bacteria(field_t* field, vec2 bac_position) {
	field->occupations[v22i(bac_position, WIDTH)] = 1;
}

void kill_bacteria(field_t* field, vec2 bac_position) {
	field->occupations[v22i(bac_position, WIDTH)] = 0;
}

vec2 get_free_position(field_t field) {
	while(1) {
		vec2 new_position = rnd_vec2(WIDTH, HEIGHT);
		if(is_occupied(field, new_position)) {continue;}
		return new_position;
	}
}

vec2 i2v2(int i, int C) {
	vec2 v2;
	v2.x = (int)i/C;
	v2.y = i%C;
	return v2;
}

int v22i(vec2 v, int C) {
	return v.x*C + v.y;
}

vec2 rnd_vec2(int width,int height) {
	vec2 v;
	v.x = rnd(0, height);
	v.y = rnd(0, width);
	return v;
}

int is_occupied(field_t field, vec2 pos) {
	return field.occupations[v22i(pos, WIDTH)];
}

void DBG_print_field_content(field_t field) {
	int i;
	vec2 vec2_coord;
	for(i=0;i<WIDTH*HEIGHT;i++)
	{
		vec2_coord = i2v2(i, WIDTH);
		if(is_occupied(field, vec2_coord)) { printf("X"); } 
		else { printf(" "); }
		if(i!=0 && (i+1)%WIDTH == 0) { printf("\n"); }
	}
}
