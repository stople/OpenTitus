#include "globals.h"

uint8 RESETLEVEL_FLAG;
bool GAMEOVER_FLAG; //triggers a game over
uint8 BAR_FLAG; //timer for health bar
bool X_FLAG; //true if left or right key is pressed
bool Y_FLAG; //true if up or down key is pressed
uint8 CHOC_FLAG; //headache timer
uint8 action; //player sprite array
uint8 KICK_FLAG; //hit/burn timer
bool GRANDBRULE_FLAG; //If set, player will be "burned" when hit (fireballs)
bool LADDER_FLAG; //True if in a ladder
bool PRIER_FLAG; //True if player is forced into kneestanding because of low ceiling
uint8 SAUT_FLAG; //6 if free fall or in the middle of a jump, decremented if on solid surface. Must be 0 to initiate a jump.
uint8 LAST_ORDER; //Last action (kneestand + jump = silent walk)
uint8 FURTIF_FLAG; //Silent walk timer
bool DROP_FLAG; //True if an object is throwed forward
bool DROPREADY_FLAG;
bool CARRY_FLAG; //true if carrying something (add 16 to player sprite)
bool POSEREADY_FLAG;
uint8 ACTION_TIMER; //Frames since last action change
//TITUS_sprite sprite; //Player sprite
//TITUS_sprite sprite2; //Secondary player sprite (throwed objects, "hit" when object hits an enemy, smoke when object hits the floor)
uint8 INVULNERABLE_FLAG; //When non-zero, boss is invulnerable
uint8 TAPISFLY_FLAG; //When non-zero, the flying carpet is flying
uint8 CROSS_FLAG; //When non-zero, fall through certain floors (after key down)
uint8 GRAVITY_FLAG; //When zero, skip object gravity function
uint8 FUME_FLAG; //Smoke when object hits the floor
const Uint8 *keystate; //Keyboard state
uint8 LIFE; //Lives
uint8 YFALL;
bool POCKET_FLAG;
bool PERMUT_FLAG; //If false, there are no animated tiles on the screen?
uint8 loop_cycle; //Increased every loop in game loop
uint8 tile_anim; //Current tile animation (0-1-2), changed every 4th game loop cycle
uint8 BITMAP_X; //Screen offset (X) in tiles
uint8 BITMAP_XM; //Point to the left tile in the tile screen (0 to 19)
uint8 BITMAP_Y; //Screen offset (Y) in tiles
uint8 BITMAP_YM; //Point to the top tile in the tile screen (0 to 11)
bool XSCROLL_CENTER; //If true, the screen will scroll in X
int16 XLIMIT_SCROLL; //If scrolling: scroll until player is in this tile (X)
int16 XLIMIT; //The engine will not scroll past this tile before the player have crossed the line (X)
bool YSCROLL_CENTER; //If true, the screen will scroll in Y
uint8 YLIMIT_SCROLL; //If scrolling: scroll until player is in this tile (Y)
uint8 ALTITUDE_ZERO; //The engine will not scroll below this tile before the player have gone below (Y)
int LAST_CLOCK; //Used for fixed framerate
uint16 IMAGE_COUNTER; //Increased every loop in game loop (0 to 0x0FFF)
int8 SENSX; //1: walk right, 0: stand still, -1: walk left, triggers the ACTION_TIMER if it changes
uint8 SAUT_COUNT; //Incremented from 0 to 3 when accelerating while jumping, stop acceleration upwards if >= 3
bool NOSCROLL_FLAG;
bool NEWLEVEL_FLAG; //Finish a level
uint8 BIGNMI_NBR; //Number of bosses that needs to be killed to finish
uint8 TAUPE_FLAG; //Used for enemies walking and popping up
uint8 TAPISWAIT_FLAG; //Flying carpet state
uint8 SEECHOC_FLAG; //Counter when hit
bool NFC_FLAG; //Skip NO_FAST_CPU
uint8 BIGNMI_POWER; //Lives of the boss
bool boss_alive; //True if the boss is alive
uint8 AUDIOMODE;

bool GODMODE; //If true, the player will not interfere with the enemies
bool NOCLIP; //If true, the player will move noclip
bool DISPLAYLOOPTIME; //If true, display loop time in milliseconds
uint8 LOOPTIME; //Loop time
uint8 SUBTIME[16]; //Sub time
uint16 FPS; //Frames pr second
uint16 FPS_LAST; //Frames pr second
uint16 LAST_CLOCK_CORR; //Correction to LAST_CLOCK


SPRITE sprites[256];

SPRITEDATA spritedata[256];

uint16 level_code[16];
