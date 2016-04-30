# ArdR120
Arduino project to allow a Strand R120 remote to work with Eos over TCP using the ESP8266 chip - developement done using an Adafruit Huzzah board with a Max232 chip to shift the serial data to RS232 levels - PCB design is in the PCB folder

Specifically targetted to work with the Stage Electrics version of the R120 which has fewer buttons and a smaller display, but is a much nicer box, so all the features can be accessed with the reduced number of buttons 

# User
The remote can be configured to send commands either as the connnected console's own user, or a specified user. Note that a limitation of the EOS OSC API currently means that all OSC devices share a user - which is a pain!

# Shift keys

@Att button is effectively shift, but has different behaviour to the shift button on the console in order to leverage additonal remote functionality

The following commands are accepted:

[@ATT][Clr] = clear command line

[@ATT][Go] = go to cue

[@ATT][Cue] = cue only

[@ATT][Rec] = update

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

[@ATT][@] = Address (for Stage Electrics remotes without a Dim key)

[@ATT][+] = Park

[@ATT][.] = /

#Differences to labelled buttoned

[ON] equates to eos [Level]

[DIM] equates to eos [Address]

[up arrow] equates to +% and -%

#Setup

Press and hold the ESP's on board button (pin 0) for 2 seconds to do a full factory reset, note that once restarted you will be in bootloader mode, so press the reset button to run normally

Setup commands are accessed by pressing [@ATT]  [@ATT] (press @ATT twice), followed by the option below, press [Rem Dim] at any time while in setup to exit without saving

Press [*] to confirm settings
[up] and [down] will move left/right where applicable

CHANGED THE BELOW....

[1] = Display current settings of device, cursor up/down to scroll through 

[2] = Set console user (0 > 254), 0 will revert to the connected console's user (meaning commands will appear on that console's command line and vice-versa

[3] = Set console IP (in future implement OpenSLP so you can discver the console on the network..)

[4] = Select DHCP/static IP, if Static you will be prompted for subnet mask and gateway

[5] = Select Wifi network (use up and down to cycle through), pressing [*] will take you to the password entry. 

1, A, B, C, a, b, c

2, D, E, F, d, e, f

3, G, H, I, g, h, i

4, J, K, L, j, k, l

5, M, N, O, m, n, o

6, P, Q, R, p, q, r

7, S, T, U, s, t, u

8, V, W, X, v, w, x

9, Y, Z, y, z

0, (TO BE CHECKED)

. special chars (NOT YET WORKING)

[6] enable remote setup - the device will turn into a Wireless Access Point named "ArdR120-xxx" with the password "12345678" - navigate to 192.168.4.1 in your browser. The AP will run until saved - alternatively you can press REMDIM on the remote and the AP will quit and device restart..

[7] enable webserver - this allows you to configure the device by typing the IP address into a browser on a device on the same wireless network. 1=ON, 2=OFF

[9] = Enter bootloader mode (needs arduino IDE for now) - TODO enable web upload somehow.. Maybe through Wifi Manager, this is for 'engineer' use only and will probably eventually be removed..



#current problems/TODO

Need to tidy up how screen displays information, and have option for SLX (3 line) or Strand (4? line) remotes 

When recalling macros command line on remote doesn't show that you are typing a macro (as it doesn't appear on console command line), maybe replace top line with this sort of thing, could also display the SHIFT mode, and the below stop/go stuff..

Similarly STOPBACK and GO don't display - maybe need a mechanism to show "special" messages for a short duration

special chars not yet working in on device wifi config..

Check that user saves/loads properly everywhere

An interrupt in the wifiManager so that if you press a key on the remote wifiManager will quit..

Check what happens if things don't connect, and that device can recover OK from all problems..

