# ArdR120
Arduino project to allow a Strand R120 remote to work with Eos over TCP using the ESP8266 chip - developement done using an Adafruit Huzzah board with a Max232 chip to shift the serial data to RS232 levels

Specifically targetted to work with the Stage Electrics version of the R120 which has fewer buttons and a smaller display, but is a much nicer box 

# user
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

[@ATT][Rec] = update

[@ATT][@] = Address (for Stage Electrics remotes without a Dim key)

[@ATT][+] = Park

#Differences to labelled buttoned

[ON] equates to eos [Level]

[DIM] equates to eos [Address]

[up arrow] equates to +% and -%

#current problems/TODO

- No / key so can't set addresses on other universes - maybe use . somehow?

- Need to make users work

- Need to make all network settings configurable

- Need to tidy up how screen displays information

- When recalling macros command line on remote doesn't show that you are typing a macro (as it doesn't appear on console command line)

- Similarly STOPBACK and GO don't display - maybe need a mechanism to show "special" messages for a short duration
"'

