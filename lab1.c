#include "dynamixel.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void move_to_location(int connection, unsigned char id,
			unsigned char loc_h, unsigned char loc_l) {

	unsigned char cs = ~ ( id + 0x07 + 0x03 + 0x1e + loc_l + loc_h +
				0x30 + 0x00);

	unsigned char arr[] = { 0xff, 0xff, id, 0x07, 0x03, 0x1e, loc_l,
                                       loc_h, 0x30, 0x00, cs };

	int buff_len = 100;
	unsigned char buff[buff_len];

	int bytes_read = write_to_connection(connection,arr,11,buff,buff_len);

}

void wait_until_done(int connection, unsigned char id) {
	usleep(2000000);
}

void lift_up (int connection)
{
	move_to_location(connection,2,0x01,0x60);
	wait_until_done(connection,2);
	move_to_location(connection,3,0x01,0x80);
    wait_until_done(connection,3);
	move_to_location(connection,4,0x00,0xdf);
	wait_until_done(connection,4);
}

void rotate_to_pile (int pile, int connection)
{
    switch (pile)
    {
        case 1:
            move_to_location(connection,1,0x01,0x61);
            wait_until_done(connection,1);
            break;
        case 2:
            move_to_location(connection,1,0x01,0xb0);
            wait_until_done(connection,1);
            break;
        case 3:
            move_to_location(connection,1,0x01,0xff);
            wait_until_done(connection,1);
            break;
    }
}

void move_to_level (int level, int connection)
{
    switch (level)
    {
        case 1:
            move_to_location(connection,2,0x01,0x1d);
            wait_until_done(connection,2);
            move_to_location(connection,3,0x01,0x4a);
            wait_until_done(connection,3);
            move_to_location(connection,4,0x01,0x3a);
            wait_until_done(connection,4);
            break;
        case 2:
            move_to_location(connection,2,0x01,0x36);
            wait_until_done(connection,2);
            move_to_location(connection,3,0x01,0x4c);
            wait_until_done(connection,3);
            move_to_location(connection,4,0x01,0x1d);
            wait_until_done(connection,4);
            break;
        case 3:
            move_to_location(connection,2,0x01,0x4f);
            wait_until_done(connection,2);
            move_to_location(connection,3,0x01,0x4f);
            wait_until_done(connection,3);
            move_to_location(connection,4,0x01,0x00);
            wait_until_done(connection,4);
            break;
    }
}

void grab (int connection)
{
    move_to_location(connection,5,0x01,0x40);
	wait_until_done(connection,5);
}

void let_go (int connection)
{
    move_to_location(connection,5,0x01,0xff);
	wait_until_done(connection,5);
}

int main(int argc, char* argv[]) {

	int connection = open_connection("/dev/ttyUSB0",B1000000);

    let_go(connection);
	rotate_to_pile(1,connection);
    move_to_level(3,connection);
    grab(connection);
    lift_up(connection);
    rotate_to_pile(3,connection);
    move_to_level(1,connection);
    let_go(connection);
    lift_up(connection);
    rotate_to_pile(1,connection);
    move_to_level(2,connection);
    grab(connection);
    lift_up(connection);
    rotate_to_pile(2,connection);
    move_to_level(1,connection);
    let_go(connection);
    lift_up(connection);
    rotate_to_pile(3,connection);
    move_to_level(1,connection);
    grab(connection);
    lift_up(connection);
    rotate_to_pile(2,connection);
    move_to_level(2,connection);
    let_go(connection);
    lift_up(connection);
    rotate_to_pile(1,connection);
    move_to_level(1,connection);
    grab(connection);
    lift_up(connection);
    rotate_to_pile(3,connection);
    move_to_level(1,connection);
    let_go(connection);
    lift_up(connection);
    rotate_to_pile(2,connection);
    move_to_level(2,connection);
    grab(connection);
    lift_up(connection);
    rotate_to_pile(1,connection);
    move_to_level(1,connection);
    let_go(connection);
    lift_up(connection);
    rotate_to_pile(2,connection);
    move_to_level(1,connection);
    grab(connection);
    lift_up(connection);
    rotate_to_pile(3,connection);
    move_to_level(2,connection);
    let_go(connection);
    lift_up(connection);
    rotate_to_pile(1,connection);
    move_to_level(1,connection);
    grab(connection);
    lift_up(connection);
    rotate_to_pile(3,connection);
    move_to_level(3,connection);
    let_go(connection);
    lift_up(connection);

	return 0;

}




