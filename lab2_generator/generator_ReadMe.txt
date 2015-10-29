
The Generator Program is impleted by using Half Sync Half Aync Pattern, where the
main thread runs the parser class routines to synchronously hand off work to async 
threads that are responsible for writing data to disk

The primary class in the generator is the parser class (contained within parser.h), 
which is tasked with the bulk job of parsing a script file to split into different 
part fragments along with their respective configuration files. 

The parser class is implemented as a state machine, which stores and changes its 
current state (which is an enum identified by the current_state member variable),
as it encounters the start and end of different parts of the script it is parsing.

The parser::parse method, which is responsible for doing the parsing of a script,
first checks if the title of the play is available in the required format and if
so processes the rest of script by first checking if new characters are being
introduced or existing characters are exiting, by checking the next line against
a regular expression and extracting the names of the characters entering / exiting.
Once it detects that a change is in progress, it internally increments the act number
and sets its internal state to CHANGE, and continues to introduce / remove characters
in its internal storage and also posts enter() on the thread_pool active object.
Once it encounters a line that does not signify a change in the number of characters
on screen, it detects that a change in its internal state has occurred from CHANGE to 
STATELESS and writes out the new configuration file for the next act. Secondly, if 
the regex for change line fails, the parser tries to interpret the current line as a
line of the play (not an enter / exit line) and applies a regex match against for a 
play line, which if passes, increments the internal line count maintained by the 
parser and hands of the line of play to the appropriate character in the thread pool, if
the current character is defined. If the current character is not defined, it treats the
play line as the name of the character that is going to be delivering the lines following
the current one.

When the parser::parse member function reaches the end of the script, it passes calls
exit_all() on the thread_pool active object, which shuts down all of its managed 
threads by passing a poison pill line to the work queue of each one 