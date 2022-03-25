// Module to implement the ground controller application-layer protocol.

// The protocol is fully defined in the README file. This module
// includes functions to parse and perform commands sent by an
// airplane (the docommand function), and has functions to send
// responses to ensure proper and consistent formatting of these
// messages.

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "airplane.h"
#include "airs_protocol.h"
#include "planelist.h"
#include "que.h"

/************************************************************************
 * Call this response function if a command was accepted
 */
void send_ok(airplane *plane) {
    fprintf(plane->fp_send, "OK\n");
}

/************************************************************************
 * Call this response function if an error can be described by a simple
 * string.
 */
void send_err(airplane *plane, char *desc) {
    fprintf(plane->fp_send, "ERR %s\n", desc);
}

/************************************************************************
 * Call this response function if you want to embed a specific string
 * argument (sarg) into an error reply (which is now a format string).
 */
void send_err_sarg(airplane *plane, char *fmtstring, char *sarg) {
    fprintf(plane->fp_send, "ERR ");
    fprintf(plane->fp_send, fmtstring, sarg);
    fprintf(plane->fp_send, "\n");
}

/************************************************************************
 * Handle the "REG" command.
 */
static void cmd_reg(airplane *plane, char *rest) {
    if (plane->state != PLANE_UNREG) {
        send_err_sarg(plane, "Already registered as %s", plane->id);
        return;
    }

    if (rest == NULL) {
        send_err(plane, "REG missing flightid");
        return;
    }

    char *cp = rest;
    while (*cp != '\0') {
        if (!isalnum(*cp)) {
            send_err(plane, "Invalid flight id -- only alphanumeric characters allowed");
            return;
        }
        cp++;
    }

    if (strlen(rest) > PLANE_MAXID) {
        send_err(plane, "Invalid flight id -- too long");
        return;
    }

    // Check for a duplicate flight number

    if (planelist_find(rest) != NULL) {
        send_err(plane, "Duplicate flight id");
        return;
    }

    // Using a "planelist" function to change id for an atomic update
    planelist_changeid(plane, rest);
    plane->state = PLANE_ATTERMINAL;

    send_ok(plane);
}

/************************************************************************
 * Handle the "REQTAXI" command.
 */
static void cmd_reqtaxi(airplane *plane, char *rest) {
    if (plane->state == PLANE_UNREG) {
        send_err(plane, "Unregistered plane -- cannot process request");
        return;
    }

    //send_err(plane, "REQTAXI command not yet implemented");
    plane->state =3;
    que_add(plane, plane->id);
}

/************************************************************************
 * Handle the "REQPOS" command.
 */
static void cmd_reqpos(airplane *plane, char *rest) {
    if (plane->state == PLANE_UNREG) {
        send_err(plane, "Unregistered plane -- cannot process request");
        return;
    }
    if(plane->state == 4 || plane->state == 3){
        que_pos(plane, plane->id);
    }else
        send_err(plane, "REQPOS can only be used when the plane is taxiing");
}

/************************************************************************
 * Handle the "REQAHEAD" command.
 */
static void cmd_reqahead(airplane *plane, char *rest) {
    if (plane->state == PLANE_UNREG) {
        send_err(plane, "Unregistered plane -- cannot process request");
        return;
    }

    if(plane->state == 4 || plane->state == 3){
        que_print(plane, plane->id);
        fprintf(plane->fp_send,"\n");
    }else
        send_err(plane, "REQAHEAD can only be used when the plane is taxiing");
}


/************************************************************************
 * Handle the "INAIR" command.
 */
static void cmd_inair(airplane *plane, char *rest) {
    if (plane->state == PLANE_UNREG) {
        send_err(plane, "Unregistered plane -- cannot process request");
        return;
    }

    //send_err(plane, "INAIR command not yet implemented");
    if (que_index(plane->id) == 0){
    que_remove(plane->id);
    plane->state = 5;
    send_ok(plane);
    fprintf(plane->fp_send, "NOTICE Disconnecting from ground control - please connect to air control\n");
    } else{
        fprintf(plane->fp_send, "Plane is not cleared for takeoff\n");
        fflush(plane->fp_send);
    }

}

/************************************************************************
 * Handle the "BYE" command.
 */
static void cmd_bye(airplane *plane, char *rest) {
    que_remove(plane->id);
    plane->state = PLANE_DONE;
}

/************************************************************************
 * Parses and performs the actions in the line of text (command and
 * optionally arguments) passed in as "command".
 */
void docommand(airplane *plane, char *command) {
    char *saveptr;
    char *cmd = strtok_r(command, " \t\r\n", &saveptr);
    if (cmd == NULL) {  // Empty line (no command) -- just ignore line
        return;
    }

    // Get arguments (everything after command, trimmed)
    char *args = strtok_r(NULL, "\r\n", &saveptr);
    if (args != NULL) {
        args = trim(args);
    }

    if (strcmp(cmd, "REG") == 0) {
        cmd_reg(plane, args);
    } else if (strcmp(cmd, "REQTAXI") == 0) {
        cmd_reqtaxi(plane, args);
    } else if (strcmp(cmd, "REQPOS") == 0) {
        cmd_reqpos(plane, args);
    } else if (strcmp(cmd, "REQAHEAD") == 0) {
        cmd_reqahead(plane, args);
    } else if (strcmp(cmd, "INAIR") == 0) {
        cmd_inair(plane, args);
    } else if (strcmp(cmd, "BYE") == 0) {
        cmd_bye(plane, args);
    } else {
        send_err(plane, "Unknown command");
    }
}
