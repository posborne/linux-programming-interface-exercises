/*
Assingment 12-1

Copyright 2012, Paul Osborne

Write a program that lists the porcess ID and command name for all
processes being run by the user named in the program's command-line
argument.  (You may find the userIdFromName() function from Listing
8-1, on page 159, useful.)  This can be done by inspecting the Name:
and Uid: lines of all of the /proc/PID/status files on the system.
Walking through all of the /proc/PID directories on the system
requires the use of readdir(3), which is described in Section 18.8.
Make sure your porgram correctly handles the possibility that a
/proc/PID directory disappears between the time that the porgram
determines that the directory exists and the time that it tries to
open the corresponding /proc/PID/status file.
*/

int main(int argc, char **argv)
{
	return 0;
}
