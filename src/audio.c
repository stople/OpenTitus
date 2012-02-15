/*   
 * Copyright (C) 2010 Eirik Stople
 *
 * OpenTitus is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 3  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

//Probably not the best way, but it works...
#define HAVE_CONFIG_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef AUDIO_ENABLED


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_audio.h>
#include "audio.h"
#include "globals.h"
#include "settings.h"
#include "definitions.h"
#include "tituserror.h"
#include "opl.h"
#include "common.h"


#define ADLIB_DATA_COUNT 10
#define ADLIB_INSTRUMENT_COUNT 19
#define ADLIB_SFX_COUNT 14

#define FREQ_RATE 44100
#define BUF_SIZE 2048

#define MUS_OFFSET 0
#define INS_OFFSET 352
#define SFX_OFFSET 1950

#define ADLIB_PORT 0x388

char playing;
int rate;

unsigned char opera[] = {0,0,1,2,3,4,5,8,9,0xA,0xB,0xC,0xD,0x10,0x11,0x12,0x13,0x14,0x15};
unsigned char voxp[] = {1,2,3,7,8,9,13,17,15,18,14};
unsigned int gamme[] = {343,363,385,408,432,458,485,514,544,577,611,647,0};
unsigned int song_type[] = {0,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0};

FILE* gifp;

uint8 last_song;

int debug_test[20];

int debug_counter;

uint16 seg_reduction;

unsigned int lastnote, tick; //debug

typedef struct {
    unsigned char op[2][5]; //Two operators and five data settings 
    unsigned char fb_alg;
    unsigned char vox; //(only for perc instruments, 0xFE if this is melodic instrument, 0xFF if this instrument is disabled)
} ADLIB_INSTR;

typedef struct {
    unsigned char duration[ADLIB_DATA_COUNT];
    unsigned char volume[ADLIB_DATA_COUNT];
    unsigned char tempo[ADLIB_DATA_COUNT];
    unsigned char triple_duration[ADLIB_DATA_COUNT];
    unsigned char lie[ADLIB_DATA_COUNT];
    unsigned char vox[ADLIB_DATA_COUNT]; //(range: 0-10)
    ADLIB_INSTR *instrument[ADLIB_DATA_COUNT];
    ADLIB_INSTR sfx[ADLIB_SFX_COUNT];

    unsigned char delay_counter[ADLIB_DATA_COUNT];
    unsigned char freq[ADLIB_DATA_COUNT];
    unsigned char octave[ADLIB_DATA_COUNT];
    unsigned char *return_point[ADLIB_DATA_COUNT];
    unsigned char loop_counter[ADLIB_DATA_COUNT];
    unsigned char *pointer[ADLIB_DATA_COUNT];
    unsigned char lie_late[ADLIB_DATA_COUNT];

    unsigned char perc_stat;

    unsigned char skip_delay;
    unsigned char skip_delay_counter;

    signed int cutsong; //Contains the number of active music channels

    unsigned char *data;
    int data_size;

    ADLIB_INSTR instrument_data[ADLIB_INSTRUMENT_COUNT];

    SDL_AudioSpec spec;
} ADLIB_DATA;

typedef struct {
    unsigned char sampsize;
    int playing;
    SDL_AudioSpec spec;
    ADLIB_DATA aad;
} SDL_PLAYER;

bool FX_ON;
uint16 FX_TIME;
uint8 AUDIOTIMING;
int lastaudiotick;
uint8 audiodelay;

SDL_PLAYER sdl_player_data;

void updatechip(int reg, int val);
int fillchip(ADLIB_DATA *aad);
void insmaker(unsigned char *insdata, int channel);
int load_file(char *filename, unsigned char **raw_data);
int SELECT_MUSIC(int song_number);
void all_vox_zero();
void TimerCallback(void *data);

int FX_DRIVER();
int FX_STOP();

void updatechip(int reg, int val)
{
    OPL_WriteRegister(reg, val);
}

int fillchip(ADLIB_DATA *aad)
{
    int i;
    unsigned char byte;
    unsigned char oct;  //.xxx....
    unsigned char freq; //....xxxx
    unsigned int tmp1, tmp2;
    unsigned char tmpC;

	FX_DRIVER();
	
    aad->skip_delay_counter--;
    if (aad->skip_delay_counter == 0) {
        aad->skip_delay_counter = aad->skip_delay;
        return (aad->cutsong); //Skip (for modifying tempo)
    }
    for (i = 0; i < ADLIB_DATA_COUNT; i++) {
        if (aad->pointer[i] == NULL) continue;
        if (aad->delay_counter[i] > 1) {
            aad->delay_counter[i]--;
            continue;
        }

        do {
        byte = *aad->pointer[i];
        aad->pointer[i]++;
        oct = (byte >> 4) & 0x07;
        freq = byte & 0x0F;
        if (byte & 0x80) { //Escape)
            switch (oct) {
            case 0: //Change duration
                aad->duration[i] = freq;
                break;

            case 1: //Change volume
                aad->volume[i] = freq;
                tmpC = aad->instrument[i]->op[0][2];
                tmpC = (tmpC & 0x3F) - 63;

                tmp1 = (((256 - tmpC) << 4) & 0x0FF0) * (freq + 1);
                tmp1 = 63 - ((tmp1 >> 8) & 0xFF);
                tmp2 = voxp[aad->vox[i]];
                if (tmp2 <= 13)
                    tmp2 += 3;
                tmp2 = opera[tmp2];
                updatechip(0x40 + tmp2, tmp1);
                break;

            case 2: //Change tempo
                aad->tempo[i] = freq;
                break;

            case 3: //Change triple_duration
                aad->triple_duration[i] = freq;
                break;

            case 4: //Change lie
                aad->lie[i] = freq;
                break;

            case 5: //Change vox (channel)
                aad->vox[i] = freq;
                break;

            case 6: //Change instrument
                if (freq == 1) { //Not melodic
                    aad->instrument[i] = &(aad->instrument_data[aad->octave[i] + 15]); //(1st perc instrument is the 16th instrument)
                    aad->vox[i] = aad->instrument[i]->vox;
                    aad->perc_stat = aad->perc_stat | (0x10 >> ((signed int)aad->vox[i] - 6)); //set a bit in perc_stat
                } else {
                    if (freq > 1)
                        freq--;
                    aad->instrument[i] = &(aad->instrument_data[freq]);
                    if (((signed int)aad->vox[i] - 6) >= 0) {
                        tmp1 = (0x10 << ((signed int)aad->vox[i] - 6)) & 0xFF; //
                        aad->perc_stat = aad->perc_stat | tmp1;                //   clear a bit from perc_stat
                        tmp1 = ~(tmp1) & 0xFF;                                 //
                        aad->perc_stat = aad->perc_stat & tmp1;                //
                        updatechip(0xBD, aad->perc_stat); //update perc_stat
                    }
                }
                tmp2 = voxp[aad->vox[i]];
                if (aad->vox[i] <= 6)
                    tmp2 += 3;
                tmp2 = opera[tmp2]; //Adlib channel
                insmaker(aad->instrument[i]->op[0], tmp2);
                if (aad->vox[i] < 7) {
                    insmaker(aad->instrument[i]->op[1], tmp2 - 3);
                    updatechip(0xC0 + aad->vox[i], aad->instrument[i]->fb_alg);
                }
                break;

            case 7: //Extra functions
                switch (freq) {
                case 0: //Call a sub
                    aad->return_point[i] = aad->pointer[i] + 2;
                    tmp1 = ((unsigned int)aad->pointer[i][1] << 8) & 0xFF00;
                    tmp1 += (unsigned int)aad->pointer[i][0] & 0xFF;
                    aad->pointer[i] = aad->data + tmp1 - seg_reduction;
                    break;

                case 1: //Update loop counter
                    aad->loop_counter[i] = *(aad->pointer[i]);
                    aad->pointer[i]++;
                    break;

                case 2: //Loop
                    if (aad->loop_counter[i] > 1) {
                        aad->loop_counter[i]--;
                        tmp1 = ((unsigned int)aad->pointer[i][1] << 8) & 0xFF00;
                        tmp1 += (unsigned int)aad->pointer[i][0] & 0xFF;
                        aad->pointer[i] = aad->data + tmp1 - seg_reduction;
                    } else {
                        aad->pointer[i] += 2;
                    }
                    break;

                case 3: //Return from sub
                    aad->pointer[i] = aad->return_point[i];
                    break;

                case 4: //Jump
                    tmp1 = ((unsigned int)aad->pointer[i][1] << 8) & 0xFF00;
                    tmp1 += (unsigned int)aad->pointer[i][0] & 0xFF;
                    aad->pointer[i] = aad->data + tmp1 - seg_reduction;
                    break;

                case 15: //Finish
                    aad->pointer[i] = NULL;
                    aad->cutsong--;
                    break;

                }
                break;
            }
        }

        } while ((byte & 0x80) && aad->pointer[i]);
        if (aad->pointer[i] == NULL) continue;

        aad->octave[i] = oct;
        aad->freq[i] = freq;

        //Play note
        if (gamme[aad->freq[i]] != 0) {
            if (aad->instrument[i]->vox == 0xFE) { //Play a frequence
                updatechip(0xA0 + aad->vox[i], (unsigned char)(gamme[aad->freq[i]] & 0xFF)); //Output lower 8 bits of frequence
                if (aad->lie_late[i] != 1) {
                    updatechip(0xB0 + aad->vox[i], 0); //Silence the channel
                }
                tmp1 = (aad->octave[i] + 2) & 0x07; //Octave (3 bits)
                tmp2 = (unsigned char)((gamme[aad->freq[i]] >> 8) & 0x03); //Frequency (higher 2 bits)
                updatechip(0xB0 + aad->vox[i], 0x20 + (tmp1 << 2) + tmp2); //Voices the channel, and output octave and last bits of frequency
                aad->lie_late[i] = aad->lie[i];

            } else { //Play a perc instrument
                if (aad->instrument[i] != &(aad->instrument_data[aad->octave[i] + 15])) { //New instrument

                    //Similar to escape, oct = 6 (change instrument)
                    aad->instrument[i] = &(aad->instrument_data[aad->octave[i] + 15]); //(1st perc instrument is the 16th instrument)
                    aad->vox[i] = aad->instrument[i]->vox;
                    aad->perc_stat = aad->perc_stat | (0x10 >> ((signed int)aad->vox[i] - 6)); //set a bit in perc_stat
                    tmp2 = voxp[aad->vox[i]];
                    if (aad->vox[i] <= 6)
                        tmp2 += 3;
                    tmp2 = opera[tmp2]; //Adlib channel
                    insmaker(aad->instrument[i]->op[0], tmp2);
                    if (aad->vox[i] < 7) {
                        insmaker(aad->instrument[i]->op[1], tmp2 - 3);
                        updatechip(0xC0 + aad->vox[i], aad->instrument[i]->fb_alg);
                    }

                    //Similar to escape, oct = 1 (change volume)
                    tmpC = aad->instrument[i]->op[0][2];
                    tmpC = (tmpC & 0x3F) - 63;
                    tmp1 = (((256 - tmpC) << 4) & 0x0FF0) * (aad->volume[i] + 1);
                    tmp1 = 63 - ((tmp1 >> 8) & 0xFF);
                    tmp2 = voxp[aad->vox[i]];
                    if (tmp2 <= 13)
                        tmp2 += 3;
                    tmp2 = opera[tmp2];
                    updatechip(0x40 + tmp2, tmp1);
                }
                tmpC = 0x10 >> ((signed int)aad->vox[i] - 6);
                updatechip(0xBD, aad->perc_stat & ~tmpC); //Output perc_stat with one bit removed
                if (aad->vox[i] == 6) {
                    updatechip(0xA6, 0x57); //
                    updatechip(0xB6, 0);    // Output the perc sound
                    updatechip(0xB6, 5);    //
                }
                updatechip(0xBD, aad->perc_stat); //Output perc_stat
            }
        } else {
            aad->lie_late[i] = aad->lie[i];
        }

        if (aad->duration[i] == 7)
            aad->delay_counter[i] = 0x40 >> aad->triple_duration[i];
        else
            aad->delay_counter[i] = 0x60 >> aad->duration[i];
    }
    return (aad->cutsong);
}

void insmaker(unsigned char *insdata, int channel)
{
    updatechip(0x60 + channel, insdata[0]); //Attack Rate / Decay Rate
    updatechip(0x80 + channel, insdata[1]); //Sustain Level / Release Rate
    updatechip(0x40 + channel, insdata[2]); //Key scaling level / Operator output level
    updatechip(0x20 + channel, insdata[3]); //Amp Mod / Vibrato / EG type / Key Scaling / Multiple
    updatechip(0xE0 + channel, insdata[4]); //Wave type
}


int load_file(char *filename, unsigned char **raw_data)
{
    int size = 0, i;

    FILE *ifp;

    ifp = fopen(filename, "rb");

    if (ifp == NULL) {
        fprintf(stderr, "Can't open input file: %s!\n", filename);
        return(-1);
    }

    fseek(ifp, 0L, SEEK_END);
    size = ftell(ifp);
    fseek(ifp, 0, SEEK_SET);
    *raw_data = (unsigned char *)malloc(size + 1);
    if (&raw_data == NULL) {
        fprintf(stderr, "Not enough memory to load file: %s!\n", filename);
        fclose(ifp);
        return -4;
    }

    if ((i = fread(*raw_data, 1, size, ifp)) != size) {
        fprintf(stderr, "Reading error: read %d bytes, should have been %d, in file %s!\n", i, size, filename);
        free (*raw_data);
        fclose(ifp);
        return -5;
    }
    fclose(ifp);
    return size;
}

int SELECT_MUSIC(int song_number)
{
    int i; //Index
    int j; //Offset to the current offset
    int k;
    unsigned int tmp1; //Source offset
    unsigned int tmp2; //Next offset
    ADLIB_DATA *aad = &(sdl_player_data.aad);
    unsigned char *raw_data = aad->data;
    if (song_type[song_number] == 0) { //0: level music, 1: bonus
        last_song = song_number;
    }
    aad->perc_stat = 0x20;

    //Load instruments
    j = INS_OFFSET;
	SDL_LockAudio();

    for (i = 0; i < song_number; i++) {
        do {
            j += 2;
            tmp1 = ((unsigned int)raw_data[j] & 0xFF) + (((unsigned int)raw_data[j + 1] << 8) & 0xFF00);
        } while (tmp1 != 0xFFFF);
        j += 2;
    }
    all_vox_zero();

    tmp2 = ((unsigned int)raw_data[j] & 0xFF) + (((unsigned int)raw_data[j + 1] << 8) & 0xFF00);

    for (i = 0; i < ADLIB_INSTRUMENT_COUNT + 1; i++)
        aad->instrument_data[i].vox = 0xFF; //Init; instrument not in use

    for (i = 0; (i < ADLIB_INSTRUMENT_COUNT + 1) && ((j + 2) < aad->data_size); i++) {
        tmp1 = tmp2;
        tmp2 = ((unsigned int)raw_data[j + 2] & 0xFF) + (((unsigned int)raw_data[j + 3] << 8) & 0xFF00);
        j += 2;

        if (tmp2 == 0xFFFF) //Terminate for loop
            break;

        if (tmp1 == 0) //Instrument not in use
            continue;

        if (i > 14) //Perc instrument (15-18) have an extra byte, melodic (0-14) have not
            aad->instrument_data[i].vox = raw_data[(tmp1++) - seg_reduction];
        else
            aad->instrument_data[i].vox = 0xFE;

        for (k = 0; k < 5; k++)
            aad->instrument_data[i].op[0][k] = raw_data[(tmp1++) - seg_reduction];

        for (k = 0; k < 5; k++)
            aad->instrument_data[i].op[1][k] = raw_data[(tmp1++) - seg_reduction];

        aad->instrument_data[i].fb_alg = raw_data[tmp1 - seg_reduction];

    }

    //Set skip delay
    aad->skip_delay = tmp1;
    aad->skip_delay_counter = tmp1;

    //Load music
    j = MUS_OFFSET;

    for (i = 0; i < song_number; i++) {
        do {
            j += 2;
            tmp1 = ((unsigned int)raw_data[j] & 0xFF) + (((unsigned int)raw_data[j + 1] << 8) & 0xFF00);
        } while (tmp1 != 0xFFFF);
        j += 2;
    }

    aad->cutsong = -1;
    for (i = 0; (i < ADLIB_DATA_COUNT + 1) && (j < aad->data_size); i++) {
        tmp1 = ((unsigned int)raw_data[j] & 0xFF) + (((unsigned int)raw_data[j + 1] << 8) & 0xFF00);
        aad->cutsong++;
        if (tmp1 == 0xFFFF) //Terminate for loop
            break;

        aad->duration[i] = 0;
        aad->volume[i] = 0;
        aad->tempo[i] = 0;
        aad->triple_duration[i] = 0;
        aad->lie[i] = 0;
        aad->vox[i] = (unsigned char)i;
        aad->instrument[i] = NULL;
        //aad->instrument[i] = &(aad->instrument_data[0]);
        aad->delay_counter[i] = 0;
        aad->freq[i] = 0;
        aad->octave[i] = 0;
        aad->return_point[i] = NULL;
        aad->loop_counter[i] = 0;
        aad->pointer[i] = aad->data + tmp1 - seg_reduction;
        aad->lie_late[i] = 0;
        j += 2;
    }
	SDL_UnlockAudio();
    SDL_PauseAudio(0); //perhaps unneccessary

}

void all_vox_zero()
{
    int i;
    for (i = 0xB0; i < 0xB9; i++)
        updatechip(i, 0); //Clear voice, octave and upper bits of frequence
    for (i = 0xA0; i < 0xB9; i++)
        updatechip(i, 0); //Clear lower byte of frequence

    updatechip(0x08, 0x00);
    updatechip(0xBD, 0x00);
    updatechip(0x40, 0x3F);
    updatechip(0x41, 0x3F);
    updatechip(0x42, 0x3F);
    updatechip(0x43, 0x3F);
    updatechip(0x44, 0x3F);
    updatechip(0x45, 0x3F);
    updatechip(0x48, 0x3F);
    updatechip(0x49, 0x3F);
    updatechip(0x4A, 0x3F);
    updatechip(0x4B, 0x3F);
    updatechip(0x4C, 0x3F);
    updatechip(0x4D, 0x3F);
    updatechip(0x50, 0x3F);
    updatechip(0x51, 0x3F);
    updatechip(0x52, 0x3F);
    updatechip(0x53, 0x3F);
    updatechip(0x54, 0x3F);
    updatechip(0x55, 0x3F);
}


void TimerCallback(void *data)
{

    if (AUDIOMODE != 1)
    {
        return;
    }
    SDL_PLAYER	*sdlp = (SDL_PLAYER *)data;
	//Delay is original 13.75 ms
    int delay = 14;
	AUDIOTIMING++;
	if (AUDIOTIMING > 3) {
		AUDIOTIMING = 0;
		delay--;
	}

    // Read data until we must make a delay.

    sdlp->playing = fillchip(&(sdlp->aad));


    // Schedule the next timer callback.

    OPL_SetCallback(delay, TimerCallback, sdlp);

}

int startmusic() {
    OPL_SetCallback(0, TimerCallback, &sdl_player_data);
	return 0;
}

int refreshaudio() {
    if (AUDIOMODE != 1)
    {
        return;
    }
    int tick = SDL_GetTicks();
	if (tick - lastaudiotick < audiodelay) {
		return 0;
	}
	//Update the chip!
	lastaudiotick += audiodelay;
    audiodelay = 14;
	AUDIOTIMING++;
	if (AUDIOTIMING > 3) {
		AUDIOTIMING = 0;
		audiodelay--;
	}
    sdl_player_data.playing = fillchip(&(sdl_player_data.aad));
	return 0;
}

int initaudio(){
    AUDIOMODE = 1;
	AUDIOTIMING = 0;
    lastaudiotick = SDL_GetTicks();
	audiodelay = 14;
    last_song = 0;
    int in_len;
    FILE *ifp;

    if (game == 0) { //Titus
		seg_reduction = 1301;
		ifp = fopen("music.bin", "rb");
		if (ifp == NULL) {
			sprintf(lasterror, "Error: music.bin not found!\n");
			return (TITUS_ERROR_FILE_NOT_FOUND);
		} else {
			fseek(ifp, 0L, SEEK_END);
			in_len = ftell(ifp);
			if (in_len != 18749) {
				sprintf(lasterror, "Error: music.bin is invalid!\n");
				fclose (ifp);
				return (TITUS_ERROR_INVALID_FILE);
			}
			fclose (ifp);
		}
    } else if (game == 1) { //Moktar
		seg_reduction = 1345;
		ifp = fopen("music_mok.bin", "rb");
		if (ifp == NULL) {
			sprintf(lasterror, "Error: music_mok.bin not found!\n");
			return (TITUS_ERROR_FILE_NOT_FOUND);
		} else {
			fseek(ifp, 0L, SEEK_END);
			in_len = ftell(ifp);
			if (in_len != 18184) {
				sprintf(lasterror, "Error: music_mok.bin is invalid!\n");
				fclose (ifp);
				return (TITUS_ERROR_INVALID_FILE);
			}
			fclose (ifp);
		}
    }


    memset(&(sdl_player_data.spec), 0x00, sizeof(SDL_AudioSpec));

    OPL_SetSampleRate(FREQ_RATE);

    if (!OPL_Init(ADLIB_PORT))
    {
        fprintf(stderr, "Unable to initialise OPL layer\n");
        exit(-1);
    }

    if (game == 0) { //Titus
		sdl_player_data.aad.data_size = load_file("music.bin", &(sdl_player_data.aad.data));
    } else if (game == 1) { //Moktar
		sdl_player_data.aad.data_size = load_file("music_mok.bin", &(sdl_player_data.aad.data));
	}
    if (sdl_player_data.aad.data_size < 0) {
        freeaudio();
        return 0;
    }
	
	initsfx();

    OPL_SetCallback(0, TimerCallback, &sdl_player_data);

	return 0;
}

int freeaudio(){
    free (sdl_player_data.aad.data);

    OPL_Shutdown();

    if(!SDL_WasInit(SDL_INIT_AUDIO)) return;

    SDL_CloseAudio();

    return 0;
}

int initsfx() {
    ADLIB_DATA *aad = &(sdl_player_data.aad);
    unsigned char *raw_data = aad->data;
	FX_ON = false;
	FX_TIME = 0;
	uint16 tmp1;
	int i, k;

	tmp1 = SFX_OFFSET;

    for (i = 0; i < ADLIB_SFX_COUNT; i++) {
        for (k = 0; k < 5; k++)
            aad->sfx[i].op[0][k] = raw_data[tmp1++];

        for (k = 0; k < 5; k++)
            aad->sfx[i].op[1][k] = raw_data[tmp1++];

        aad->sfx[i].fb_alg = raw_data[tmp1++];
	}

    return 0;
}

int WAIT_SONG(){
    SDL_Event event;
    bool waiting = true;
    if (AUDIOMODE == 0) {
        return 0;
    }
    do {
        titus_sleep();
        keystate = SDL_GetKeyState(NULL);
        while(SDL_PollEvent(&event)) { //Check all events
            if (event.type == SDL_QUIT) {
                return TITUS_ERROR_QUIT;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == KEY_ESC) {
                    return TITUS_ERROR_QUIT;
                } else if (event.key.keysym.sym == KEY_MUSIC) {
					AUDIOMODE++;
					if (AUDIOMODE > 1) {
						AUDIOMODE = 0;
						waiting = false;
					}
					if (AUDIOMODE == 1) {
						startmusic();
					}
                }
            }
        }
        if (sdl_player_data.aad.cutsong == 0) {
            waiting = false;
        }
    } while (waiting);
}

int FX_START(int fx_number){
    ADLIB_DATA *aad = &(sdl_player_data.aad);
	FX_TIME = 15;
	FX_ON = true;
	SDL_LockAudio();
	insmaker(aad->sfx[fx_number].op[0], 0x13); //Channel 6 operator 1
	insmaker(aad->sfx[fx_number].op[1], 0x10); //Channel 6 operator 2
    updatechip(0xC6, aad->sfx[fx_number].fb_alg); //Channel 6 (Feedback/Algorithm)
	SDL_UnlockAudio();
    return 0;
}

int FX_DRIVER() {
    ADLIB_DATA *aad = &(sdl_player_data.aad);
	if (!FX_ON) return;
	updatechip(0xBD, 0xEF & aad->perc_stat);
	updatechip(0xA6, 0x57);
	updatechip(0xB6, 1);
	updatechip(0xB6, 5);
	updatechip(0xBD, 0x10 | aad->perc_stat);
	FX_TIME--;
	if (FX_TIME == 0) {
		FX_STOP();
	}
	return 0;
}

int FX_STOP() {
	unsigned char tmpins1[] = {0xF5, 0x7F, 0x00, 0x11, 0x00};
	unsigned char tmpins2[] = {0xF8, 0xFF, 0x04, 0x30, 0x00};
	SDL_LockAudio();
	updatechip(0xB6, 32);
	insmaker(tmpins1, 0x13); //Channel 6 operator 1
	insmaker(tmpins2, 0x10); //Channel 6 operator 2
    updatechip(0xC6, 0x08); //Channel 6 (Feedback/Algorithm)
	SDL_UnlockAudio();
	FX_ON = false;
}

int RETURN_MUSIC(){
    if (AUDIOMODE == 1) {
        if (sdl_player_data.aad.cutsong == 0) {
            SELECT_MUSIC(last_song);
        }
    }
}

#endif //AUDIO_ENABLED

