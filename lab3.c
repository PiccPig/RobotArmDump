#include "dynamixel.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

const int CALIBRATE = 0x03;

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
    usleep(600000 * pilesMoved);
}


//moves arm to specified level's height
void move_to_level (int level, int connection)
{
    switch (level)
    {
        case 1: //bottom block
            move_to_location(connection,2,0x01,0x21 + CALIBRATE);
            move_to_location(connection,4,0x01,0x35);
            usleep(200000);
            move_to_location(connection,3,0x01,0x4c + CALIBRATE);
            break;
        case 2: //middle block
            move_to_location(connection,2,0x01,0x3a + CALIBRATE);
            move_to_location(connection,4,0x01,0x15);
            usleep(200000);
            move_to_location(connection,3,0x01,0x4f + CALIBRATE);
            break;
        case 3: //top block
            move_to_location(connection,2,0x01,0x52 + CALIBRATE);
            move_to_location(connection,4,0x01,0x00);
            usleep(200000);
            move_to_location(connection,3,0x01,0x52 + CALIBRATE);
            break;
    }
}

//closes claw
void grab (int connection)
{
    usleep (400000);
    move_to_location(connection,5,0x01,0x40);
	usleep(500000);
}

//opens claw
void let_go (int connection)
{
    usleep(300000);
    move_to_location(connection,5,0x01,0x8a);
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
int takeFromTo (struct Pile fromPile, struct Pile toPile, int connection, int currentPile)
{
    int pilesMoved = abs(currentPile - fromPile.id);
    rotate_to_pile(fromPile.id, pilesMoved, connection);
    move_to_level(fromPile.height,connection);
    grab(connection);
    lift_up(connection);
    pilesMoved = abs(fromPile.id - toPile.id); //used to scale the amount of time the arm needs to wait after rotating
    rotate_to_pile(toPile.id, pilesMoved, connection);
    move_to_level(toPile.height+1,connection);
    let_go(connection);
    lift_up(connection);
    
    return toPile.id;
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
    while(1)
    {
        currentPile = takeFromTo(lPile, rPile, connection, currentPile);  // ab      c  
        lPile.height--; rPile.height++;
        currentPile = takeFromTo(lPile, mPile, connection, currentPile);  // a   b   c
        lPile.height--; mPile.height++;
        currentPile = takeFromTo(rPile, mPile, connection, currentPile);  // a   bc
        rPile.height--; mPile.height++;
        currentPile = takeFromTo(lPile, rPile, connection, currentPile);  //     bc  a
        lPile.height--; rPile.height++;
        currentPile = takeFromTo(mPile, lPile, connection, currentPile);  // c   b   a
        mPile.height--; lPile.height++;
        currentPile = takeFromTo(mPile, rPile, connection, currentPile);  // c       ab
        mPile.height--; rPile.height++;
        currentPile = takeFromTo(lPile, rPile, connection, currentPile);  //         abc
        lPile.height--; rPile.height++;

        //in reverse
        currentPile = takeFromTo(rPile, lPile, connection, currentPile);  // c       ab
        rPile.height--; lPile.height++;
        currentPile = takeFromTo(rPile, mPile, connection, currentPile);  // c   b   a
        rPile.height--; mPile.height++;
        currentPile = takeFromTo(lPile, mPile, connection, currentPile);  //     bc  a
        lPile.height--; mPile.height++;
        currentPile = takeFromTo(rPile, lPile, connection, currentPile);  // a   bc
        rPile.height--; lPile.height++;
        currentPile = takeFromTo(mPile, rPile, connection, currentPile);  // a   b   c
        mPile.height--; rPile.height++;
        currentPile = takeFromTo(mPile, lPile, connection, currentPile);  // ab      c
        mPile.height--; lPile.height++;
        currentPile = takeFromTo(rPile, lPile, connection, currentPile);  // abc
        rPile.height--; lPile.height++;
    }
	return 0;

}