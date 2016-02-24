# ArdR120
Arduino project to allow a Strand R120 remote to work with Eos over TCP using the ESP8266 chip - developement done using an Adafruit Huzzah board with a Max232 chip to shift the serial data to RS232 levels

Specifically targetted to work with the Stage Electrics version of the R120 which has fewer buttons and a smaller display, but is a much nicer box 

# User
The remote can be configures to send commands either as the connnected console's own user, or a specified user

# Shift keys

@Att button is effectively shift, but has different behaviour to the shift button on the console in order to leverage additonal remote functionality

The are accepted:

[@ATT][Clr] = clear command line

[@ATT][Go] = go to cue

[@ATT][0] = sneak

[@ATT][1] = preset

[@ATT][2] = intensity pallette

[@ATT][3] = Focus pallette

[@ATT][4] = Colour pallette

[@ATT][5] = Beam pallette

[@ATT][6] = Live

[@ATT][7] = Blind

[@ATT][8] = Patch

[@ATT][9] = Park

[@ATT][Rec] = update

[@ATT][@] = Address (for Stage Electrics remotes without a Dim key)

[@ATT][+] = Park

#Differences to labelled buttoned

[ON] equates to eos [Level]

[DIM] equates to eos [Address]

[up arrow] equates to +% and -%

#Setup

(NOT YET WORKING)

Setup commands are accessed by pressing [@ATT] - [@ATT] (press it twice), followed by the option below, press [Rem Dim] at any time while in setup to exit without saving

Press [*] to confirm settings
[up] and [down] will move left/right where applicable

[@ATT] [1] = Set user (0 > 200), [Clr] will revert to the connected console's user (meaning commands will appear on that console's command line and vice-versa

[@ATT] [2] = Set device IP, [Clr] will revert to DHCP, use [.] as applicable for segments

[@ATT] [3] = Set device subnet (not available if IP set to DHCP), [CLR] will revert to 255.255.255.0

[@ATT] [4] = Set Console IP

[@ATT] [5] = Select Wifi network (use up and down to cycle through), pressing [*] will take you to the password entry. 

1, A, B, C, a, b, c

2, D, E, F, d, e, f

3, G, H, I, g, h, i

4, J, K, L, j, k, l

5, M, N, O, m, n, o

6, P, Q, R, p, q, r

7, S, T, U, s, t, u

8, V, W, X, v, w, x

9, Y, Z, y, z

0

. special chars



#current problems/TODO

- No / key so can't set addresses on other universes - maybe use . somehow?

- Need to make users work

- Need to make all network settings configurable

- Need to tidy up how screen displays information

- When recalling macros command line on remote doesn't show that you are typing a macro (as it doesn't appear on console command line)

- Similarly STOPBACK and GO don't display - maybe need a mechanism to show "special" messages for a short duration

