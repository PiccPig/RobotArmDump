#include "dynamixel.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>


//stores a piles location and height
struct Pile 
{
    int id; //left = 0 middle = 1 right = 2
    int height; //0 = empty, 3 = full
    int position; //value for motor 1 to rotate to
};

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


//moves arm to above highest possible pile
void lift_up (int connection)
{
	move_to_location(connection,2,0x01,0x60);
	move_to_location(connection,3,0x01,0x80);
	move_to_location(connection,4,0x00,0xdf);
    usleep(300000);
}

//rotates arm to specified pile's rotation, waits based on how many piles it moves
void rotate_to_pile (int pile, int pilesMoved, int connection)
{
    switch (pile)
    {
        case 0: //left pile
            move_to_location(connection,1,0x01,0x61);
            break;
        case 1: //middle pile
            move_to_location(connection,1,0x01,0xb0);
            break;
        case 2: //right pile
            move_to_location(connection,1,0x01,0xff);
            break;
    }
    usleep(800000 * pilesMoved);
}


//moves arm to specified level's height
void move_to_level (int level, int connection)
{
    switch (level)
    {
        case 1: //bottom block
            move_to_location(connection,2,0x01,0x27);
            move_to_location(connection,3,0x01,0x4c);
            move_to_location(connection,4,0x01,0x35);
            break;
        case 2: //middle block
            move_to_location(connection,2,0x01,0x3f);
            move_to_location(connection,3,0x01,0x4f);
            move_to_location(connection,4,0x01,0x15);
            break;
        case 3: //top block
            move_to_location(connection,2,0x01,0x52);
            move_to_location(connection,3,0x01,0x52);
            move_to_location(connection,4,0x01,0x00);
            break;
    }
}

//closes claw
void grab (int connection)
{
    usleep (300000);
    move_to_location(connection,5,0x01,0x40);
	wait_until_done(connection,5);
}

//opens claw
void let_go (int connection)
{
    usleep(900000);
    move_to_location(connection,5,0x01,0x96);
	usleep(500000);
}

//moves arm to start position
void setup (int connection)
{
    let_go(connection);
    lift_up(connection);
    rotate_to_pile(0, 2, connection);
}

//moves a block from one pile to another
void takeFromTo (struct Pile fromPile, struct Pile toPile, int connection)
{
    
    rotate_to_pile(fromPile.id, 2, connection);
    move_to_level(fromPile.height,connection);
    grab(connection);
    lift_up(connection);
    int pilesMoved = abs(fromPile.id - toPile.id); //used to scale the amount of time the arm needs to wait after rotating
    rotate_to_pile(toPile.id, pilesMoved, connection);
    move_to_level(toPile.height+1,connection);
    let_go(connection);
    lift_up(connection);
}

int main(int argc, char* argv[]) {

    struct Pile lPile;
    lPile.id = 0;
    lPile.height = 3;
    lPile.position = 0x61;

    struct Pile mPile;
    mPile.id = 1;
    mPile.height = 0;
    mPile.position = 0xb0;

    struct Pile rPile;
    rPile.id = 2;
    rPile.height = 0;
    rPile.position = 0xff;

	int connection = open_connection("/dev/ttyUSB0",B1000000);

    setup(connection);
    usleep(2000000);

    int currentPile = 0;
    
    takeFromTo(lPile, rPile, connection);
    lPile.height--; rPile.height++;
    takeFromTo(lPile, mPile, connection);
    lPile.height--; mPile.height++;
    takeFromTo(rPile, mPile, connection);
    rPile.height--; mPile.height++;
    takeFromTo(lPile, rPile, connection);
    lPile.height--; rPile.height++;
    takeFromTo(mPile, lPile, connection);
    mPile.height--; lPile.height++;
    takeFromTo(mPile, rPile, connection);
    mPile.height--; rPile.height++;
    takeFromTo(lPile, rPile, connection);
    lPile.height--; rPile.height++;
    
	return 0;

}