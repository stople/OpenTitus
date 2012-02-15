/*   
 * Copyright (C) 2008 - 2011 The OpenTitus team
 *
 * Authors:
 * Eirik Stople
 *
 * "Titus the Fox: To Marrakech and Back" (1992) and
 * "Lagaf': Les Aventures de Moktar - Vol 1: La Zoubida" (1991)
 * was developed by, and is probably copyrighted by Titus Software,
 * which, according to Wikipedia, stopped buisness in 2005.
 *
 * OpenTitus is not affiliated with Titus Software.
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

/* settings.c
 * Handles settings loaded from titus.conf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"

//Probably not the best way, but it works...
#define HAVE_CONFIG_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

int readconfig(char *configfile) {
    char line[300], tmp[256];
    int retval, i, j, tmpcount = 0;
    levelcount = 0;
    spritefile[0] = 0;
    devmode = 0;
    FILE *ifp = fopen (configfile, "rt");
    if (ifp == NULL) {
        fprintf(stderr, "Error: Can't open config file: %s!\n", configfile);
        return(-1);
    }

    while(fgets(line, 299, ifp) != NULL)
    {
        if (sscanf (line, "%50s", tmp) == EOF)
            continue;

        if ((line[0] == 0) || (tmp[0] == *"#"))
            continue;

        else if (strcmp (tmp, "sprites") == 0)
            sscanf (line, "%*s %255s", spritefile);

        else if (strcmp (tmp, "levelcount") == 0) {
            if (tmpcount > 0) {
                printf("Error: You may only specify one 'levelcount', check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            sscanf (line, "%*s %255d", &levelcount);
            if ((levelcount < 1) || (levelcount > 16)) {
                printf("Error: 'levelcount' (%d) must be between 1 and 16, check config file: %s!\n", levelcount, configfile);
                fclose(ifp);
                return(-1);
            }
        }

        else if (strcmp (tmp, "level") == 0) {
            if (levelcount == 0) {
                printf("Error: 'levelcount' must be set before level files, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            if (sscanf (line, "%*s %2d", &i) <= 0) {
                printf("Error: Invalid numbering on the individual levels, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            if ((retval = sscanf (line, "%*s %*2d %255s", tmp)) <= 0) {
                printf("Error: You have not specified level file number %d, check config file: %s!\n", i, configfile);
                fclose(ifp);
                return(-1);
            }
            if ((i < 1) || (i > levelcount) || (tmpcount >= levelcount)) {
                printf("Error: Invalid numbering on the individual levels, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            strcpy (levelfiles[i - 1], tmp);
            tmpcount++;
        } else if (strcmp (tmp, "reswidth") == 0)
            sscanf (line, "%*s %255d", &reswidth);
        else if (strcmp (tmp, "resheight") == 0)
            sscanf (line, "%*s %255d", &resheight);
        else if (strcmp (tmp, "devmode") == 0)
            sscanf (line, "%*s %255d", &devmode);
        else if (strcmp (tmp, "bitdepth") == 0)
            sscanf (line, "%*s %255d", &bitdepth);
        else if (strcmp (tmp, "ingamewidth") == 0)
            sscanf (line, "%*s %255d", &ingamewidth);
        else if (strcmp (tmp, "ingameheight") == 0)
            sscanf (line, "%*s %255d", &ingameheight);
        else if (strcmp (tmp, "videomode") == 0)
            sscanf (line, "%*s %255d", &videomode);
        else if (strcmp (tmp, "game") == 0)
            sscanf (line, "%*s %255d", &game);
        else if (strcmp (tmp, "logo") == 0)
            sscanf (line, "%*s %255s", tituslogofile);
        else if (strcmp (tmp, "logoformat") == 0)
            sscanf (line, "%*s %255d", &tituslogoformat);
        else if (strcmp (tmp, "intro") == 0)
            sscanf (line, "%*s %255s", titusintrofile);
        else if (strcmp (tmp, "introformat") == 0)
            sscanf (line, "%*s %255d", &titusintroformat);
        else if (strcmp (tmp, "menu") == 0)
            sscanf (line, "%*s %255s", titusmenufile);
        else if (strcmp (tmp, "menuformat") == 0)
            sscanf (line, "%*s %255d", &titusmenuformat);
        else if (strcmp (tmp, "finish") == 0)
            sscanf (line, "%*s %255s", titusfinishfile);
        else if (strcmp (tmp, "finishformat") == 0)
            sscanf (line, "%*s %255d", &titusfinishformat);
        else if (strcmp (tmp, "font") == 0)
            sscanf (line, "%*s %255s", fontfile);
        else if (strcmp (tmp, "moduleintro") == 0)
            sscanf (line, "%*s %255s", moduleintrofile);
        else if (strcmp (tmp, "moduleintroloop") == 0)
            sscanf (line, "%*s %255d", &moduleintrofileloop);
        else if (strcmp (tmp, "moduleprelevel") == 0)
            sscanf (line, "%*s %255s", moduleprelevelfile);
        else if (strcmp (tmp, "moduleprelevelloop") == 0)
            sscanf (line, "%*s %255d", &moduleprelevelfileloop);
        else if (strcmp (tmp, "modulegameover") == 0)
            sscanf (line, "%*s %255s", modulegameoverfile);
        else if (strcmp (tmp, "modulegameoverloop") == 0)
            sscanf (line, "%*s %255d", &modulegameoverfileloop);
        else if (strcmp (tmp, "modulelevelfile") == 0) {
            if (sscanf (line, "%*s %2d", &i) <= 0) {
                printf("Error: Invalid numbering on the module level files, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            if ((retval = sscanf (line, "%*s %*2d %255s", tmp)) <= 0) {
                printf("Error: You have not specified module level file number %d, check config file: %s!\n", i, configfile);
                fclose(ifp);
                return(-1);
            }
            if ((i < 1) || (i > 6)) {
                printf("Error: Invalid numbering on the individual module level files, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            strcpy (modulelevelfile[i - 1], tmp);
        } else if (strcmp (tmp, "modulelevelfileloop") == 0) {
            if (sscanf (line, "%*s %2d", &i) <= 0) {
                printf("Error: Invalid numbering on the module level files loop, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            if ((retval = sscanf (line, "%*s %*2d %d", &j)) <= 0) {
                printf("Error: You have not specified module level file loop number %d, check config file: %s!\n", i, configfile);
                fclose(ifp);
                return(-1);
            }
            if ((i < 1) || (i > 6)) {
                printf("Error: Invalid numbering on the individual module levels loop, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            modulelevelfileloop[i - 1] = j;
        } else if (strcmp (tmp, "modulelevel") == 0) {
            if (sscanf (line, "%*s %2d", &i) <= 0) {
                printf("Error: Invalid numbering on the module levels, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            if ((retval = sscanf (line, "%*s %*2d %d", &j)) <= 0) {
                printf("Error: You have not specified module level number %d, check config file: %s!\n", i, configfile);
                fclose(ifp);
                return(-1);
            }
            if ((i < 1) || (i > levelcount)) {
                printf("Error: Invalid numbering on the individual module levels, check config file: %s!\n", configfile);
                fclose(ifp);
                return(-1);
            }
            modulelevel[i - 1] = j;
        }

        else
            printf("Warning: undefined command '%s' in titus.conf\n", tmp);

    }
    fclose(ifp);

    if (tmpcount == 0) { // No levels
        printf("Error: You must specify at least one level, check config file: %s!\n", configfile);
        return(-1);
    }

    if (tmpcount < levelcount) { // No levels
        printf("Error: 'levelcount' (%d) and the number of specified levels (%d) does not match, check config file: %s!\n", levelcount, tmpcount, configfile);
        return(-1);
    }

    for (i = 1; i <= levelcount; i++) {
        if (levelfiles[i - 1][0] == 0) {
            fprintf(stderr, "Error: You have not specified level file number %d, check config file: %s!\n", i, configfile);
            return(-1);
        }
        if ((ifp = fopen (levelfiles[i - 1], "r")) != NULL )
            fclose(ifp);
        else {
            fprintf(stderr, "Error: Level file number %d (%s) does not exist, check config file: %s!\n", i, levelfiles[i - 1], configfile);
            return(-1);
        }
    }

    if ((ifp = fopen (spritefile, "r")) != NULL )
        fclose(ifp);
    else {
        fprintf(stderr, "Error: Sprite file does not exist, check config file: %s!\n", configfile);
        return(-1);
    }

    return 0;
}

int initcodes() {
    strcpy (levelcode[0], "EFE8");
    strcpy (levelcode[1], "5165");
    strcpy (levelcode[2], "67D4");
    strcpy (levelcode[3], "2BDA");
    strcpy (levelcode[4], "11E5");
    strcpy (levelcode[5], "86EE");
    strcpy (levelcode[6], "4275");
    strcpy (levelcode[7], "A0B9");
    strcpy (levelcode[8], "501C");
    strcpy (levelcode[9], "E9ED");
    strcpy (levelcode[10], "D4E6");
    strcpy (levelcode[11], "A531");
    strcpy (levelcode[12], "CE96");
    strcpy (levelcode[13], "B1A4");
    strcpy (levelcode[14], "EBEA");
    strcpy (levelcode[15], "3B9C");
    return 0;
}

int initleveltitles() {
    if (game == 0) { //Titus
        strcpy (leveltitle[0], "           ON THE FOXY TRAIL");
        strcpy (leveltitle[1], "           LOOKING FOR CLUES");
        strcpy (leveltitle[2], "           ROAD WORKS AHEAD");
        strcpy (leveltitle[3], "           GOING UNDERGROUND");
        strcpy (leveltitle[4], "          FLAMING CATACOMBES");
        strcpy (leveltitle[5], "            COMING TO TOWN");
        strcpy (leveltitle[6], "               FOXYS DEN");
        strcpy (leveltitle[7], "       ON THE ROAD TO MARRAKESH");
        strcpy (leveltitle[8], "         HOME OF THE PHARAOHS");
        strcpy (leveltitle[9], "           DESERT EXPERIENCE");
        strcpy (leveltitle[10], "             WALLS OF SAND");
        strcpy (leveltitle[11], "           A BEACON OF HOPE");
        strcpy (leveltitle[12], "             A PIPE DREAM");
        strcpy (leveltitle[13], "              GOING HOME");
        strcpy (leveltitle[14], "             JUST MARRIED");
    } else if (game == 1) { //Moktar
        strcpy (leveltitle[0], "     A LA RECHERCHE DE LA ZOUBIDA");
        strcpy (leveltitle[1], "          LES QUARTIERS CHICS");
        strcpy (leveltitle[2], "           ATTENTION TRAVAUX");
        strcpy (leveltitle[3], "         LES COULOIRS DU METRO");
        strcpy (leveltitle[4], "       LES CATACOMBES INFERNALES");
        strcpy (leveltitle[5], "         ARRIVEE DANS LA CITE");
        strcpy (leveltitle[6], "       L IMMEUBLE DE LA ZOUBIDA");
        strcpy (leveltitle[7], "      SOUS LE CHEMIN DE MARRAKECH");
        strcpy (leveltitle[8], "            LA CITE ENFOUIE");
        strcpy (leveltitle[9], "             DESERT PRIVE");
        strcpy (leveltitle[10], "          LA VILLE DES SABLES");
        strcpy (leveltitle[11], "            LE PHARE OUEST");
        strcpy (leveltitle[12], "             UN BON TUYAU");
        strcpy (leveltitle[13], "           DE RETOUR AU PAYS");
        strcpy (leveltitle[14], "           DIRECTION BARBES");
        strcpy (leveltitle[15], "              BIG BISOUS");
    }
    return 0;
}
