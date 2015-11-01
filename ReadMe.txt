CSE 532 Fall 2015 Lab 2

Team Member: 
    Tong Mu (mutong32@gmail.com)
    Jinxuan Zhao (jzhao32@wustl.edu)
    Zeneil Ambekar (ambekar@wustl.edu)

== Usage ==
=== Unpack ===

Unpack the zip file with 7zip.

The extracted folder contains a copy of this ReadMe.txt, a sample_result folder
containing sample output from the executables, and the solution folder
for this assignment.

> lab2\
> |- ReadMe.txt
> |- sample_result\
> |- lab2\
>    |- lab2\
>    |- lab2_generator\
>    |- common\
>    |- configs\
>    |- lab2.sln
>    |- ...

The solution folder consists of the following folders,

- lab2, for the VS project lab2.
- lab2_generator, for the VS project lab2_extra.
- common, headers and utility functions common across projects.  
- configs, sample configuration files for testing. 

=== Build ===

Open lab2.sln in the solution folder with VS 2013, and build the solution.
This solution consists of 2 VS projects, lab2 and lab2_generator,
and should generate two executable files in Debug or Release folder in the 
solution folder, depends on which profile is choosed.

=== Run ===

Both executables requires command line arguments to run properly; they will 
prompt usage information if no arguments are provided.

The usage for lab2.exe is

    lab2.exe <configuration_file_path> <minimum-#-of-player> [-override]

It reads and parses the configuration file, finds corresponding per-character 
files in the SAME DIRECTORY as the config file, and prints the play into
the standard output.

The usage for lab2_generator.exe is

    lab2_generator.exe <script-to-process> <output-scene-prefix>

It reads and parses the play file, and generates a config file accordingly.
It will also generate the scene files and per-character play files in the SAME 
DIRECTORY as the config file.

=== Example ===

After building the solution, open cmd.exe and cd to the directory for the 
executales.

Generate config files

    lab2_generator.exe ..\configs\partial_hamlet_act_ii_script.txt

And play it

    lab2.exe ..\configs\partial_hamlet_act_ii_script.txt

or

    lab2.exe ..\configs\partial_hamlet_act_ii_script.txt 10 -override

== Description ==
=== Overview ===

1: General structure

    We mainly applied Leader/Follower pattern to our program. Here's a brief 
    introduction to out structure:

    (1) The main function creates the director, which creates the play and 
        a number of players. Each of the players immediately spawns his working 
        thread, which

        (a) Continuously tries to "declare idle", but only one of them
            will make through and start to wait until a task is assigned.

        (b) Once the idle player is assigned a task, it will allow another 
            player (if exists) to declare idle.

        (c) Once a player finishes his task, it comes back to again try to 
            declare idle.

    (2) The main function continously elects a director, during which it:

        (a) Gets the next scene fragment from the play.

        (b) Grabs an idle player.

        (c) Assigns the play to the idle player as a leader task.

        (d) Returns to (a).

    (3) A player who is assigned a task and finds it a leader task will:

        (a) Leave out the first character for himself, and for each of the rest
            characters,

            (i) Grabs an idle player.

            (ii) Assigns the character to the player as a follower task.

        (b) Act as his own character.

    (4) A player who is assigned a task and finds it a follower task will:

        (a) Act as the assigned character.

2: The L/F pattern we used has most advantages mentioned in the class 
   over HSHA:

    (a) Less data passing overhead. Data for the next scene are directly 
        passed to the leader, instead of being stored temporarily in a queue,
        and thus eliminating the following overhead for that queue:
         (i) dispatching thread
        (ii) dynamic allocation
       (iii) synchronization and blocking
        (iv) context switching

    (b) Less latency. Data received are guaranteed to be processed 
        immediately, instead of being queued for a possibly long time.

    But it also suffers the following disadvantages:

    (a) Additional code complexity. It took us a long discussion on the 
        structure to apply L/F pattern onto this lab, as well as
        several large changes during the implementation. It also severly 
        increased the difficulty to find bugs and to write bug-free programs.
        A realistic problem which is originally complicated can make it even
        more severe.

    (b) Higher requirement of threads. Insufficient amount of threads can 
        lead to severe problem in L/F pattern, from incoming packet lost, to 
        deadlock between work threads.

        In this lab we pre-read all configuration files and knew the necessary 
        number of threads, which is impossible in realistic cases. In 
        those situations, we can spawn threads ad hoc, i.e. spawn a new one
        when an idle player is required but no one is available, .

=== Design details ===

1: Design of class Player:

    (1) Upon creation, the Player class no longer knows his character and 
        character file, which are assigned later; instead, he only knows the 
        play he belongs to. 

        We also pass the director to the player, since a play always 
        corresponds to one and only one director.

    (2) Each player is an active object whose working thread loops on 
        Player::_start() until told to end, during which the player tries to
        declare idle to the director. Once it made through, the player must have 
        been assigned a task, and depending on whether it's a leader task or
        a follower task, it executes Player::_doLeader() or Player::_doFollower()
        respectively.

        The task information are synchroized with Player::_idleMutex and 
        Player::_idleCv.

    (3) Assigning a task to the player, whether it's a leader task or a 
        follower task, consists of

        (a) acquiring the mutex in a lock guard,
        (b) storing the value inside the player,
        (c) and notifying the condition variable.

        There is yet another method Player::_assignFollowerSync(),
        which stores values without acquiring the mutex and notifying
        the condition variable. It is used for the director to synchronizedly
        finish its acting job.

    (4) An atomic boolean member Player::_ended is used to notify a waiting 
        player that the play has ended. It is used in the waiting condition of
        the condition variable. The working thread will check this boolean 
        value on every loop and ends when it turns to true. That allows 
        it to be joined by the destructor of Player.

2: Design of class Play:

    The Play class generally handles two similar but different tasks: 
    distributing scene fragments, and supervising reciting. They are similar to
    two iterators on a same container of scene fragments, but since distributing 
    tasks and reciting are done in an asynchronized fashion, they must be 
    maintained seperately.

    The distribution part:

    (1) The Play class stores an integer Play::_sceneFragDistributed, which 
        represents the number of fragments that are given to the director. 
        It is increased in Play::getNextTask(), which returns this increased 
        value and the character list of that corresponding fragment scece.

    (2) The status of fragment distribution can be queried by 
        Play::distributeEnded(). 

    The reciting part:

    (1) The Play class stores another integer Play::_sceneFragCounter, which
        represents the id of of the fragment that is currently being played by 
        the players. 
        
    (2) Play::enter() and Play::exit() are used to keep track of the status 
        of the fragment currently being played by maintaining Play::_onStage. 
        When this value reaches 0, it's time to move on to the next scene 
        fragment, and thus increasing Play::_sceneFragCounter.

    (3) The Play::recite() method uses Play::_sceneFragCounter and 
        Play::_lineCounter to keep track of the next line the players ought to
        play. 

    (4) The status of acting can be queried by Play::actEnded(). 

    (5) We removed the iterator to the fragment container that the lab 
        requirement asked us to add to the Play class, because we found that 
        the iterator always increases at the same time as Play::_sceneFragCounter,
        and its task can be completely fulfilled by the latter one if the 
        container we use is a std::vector.

    (6) We used a same pair of mutex and condition variable for Play::exit()
        and for Play::recite(). The implementation would be simpler than 
        using a second pair of them, but the downside is that the mutex for 
        Play::exit() will suffer more spurious wakeups.

        On another hand, using a second pair of locks is not that complicated.
        Since the cases that the one for Play::exit() should wake up are a 
        strict subset to the one for Play::recite(), they can be implemented
        by hierarchical mutexes.

3: Design of class Director:
    
    The class Director generally serves as a maintainer to players.

    (1) Upon construction, it reads the play script file and storing inside of 
        itself a list of scene fragments and the list of characters of each of 
        them. It then decides the number of players needed and creates them.

    (2) It has a method Director::declareIdle() for players to declare themselves
        idle, which sets Director::_idler to this player if and only if it is 
        previously NULL. 

    (3) Director::electDirector() is designed to be called by the main thread
        to pick an idler as a director and assign it a leader task.
        Director::cue() is designed to be called by the director in the player
        thread to pick several idlers as followers and assign them corresponding
        follower tasks.

        These two functions pick iders by acquiring the value stored in 
        Director::_ider if it is previously not NULL, and setting it to NULL.

    (4) The two modules above ((2) and (3)) are here implemented in a lock free 
        fashion by defining Director::_ider as an atomic pointer variable and 
        continuously querying and testing it using atomic operations. If the test 
        fails, the thread yields to try again later. 

        These operations can also be implemented using mutexes and condition
        variables. They offer a better notifying mechanism and prevents the
        excessive power comsumption of continuous yielding. But on the other 
        hand, implementation with mutexes and condition variables are severely
        more complicated than atomic operations. Actually we first implemented
        it using mutexes and CVs but was soon scared as the number of agents 
        that touches this mutex grows.

== Insights, Observations and Questions encountered == 

	One insight that was gained from a very long session of debugging and the observation of 
	unpredictable state of member variables is that it is very crucial to ensure that
	for active objects, the thread associated with the object should not be allowed
	to start before all the member variables of the object have been initialized, if the
	method that is called by that thread references any of the potentially uninitialized variables
	of that active object

== Extra-Credit ==
== Generator Program ==

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
	threads by passing a poison pill line to the work queue of each one. 

== "-override" argument observations ==

	The "-override" command line argument forces the Director class to strictly spawn only
	the number of characters that are specified by the preceding command line argument.
	It can be observed that when the number of threads to be used (player active objects) 
	is less than the maximum difference in the number of consecutive fragments, the program
	always deadlocks at some point in the recital.

	It can be inferred why this happens as follows:

	(1) The leader player active object thread spinwaits till it can get atleast one
		idle thread. After it gets such a thread, it hands of the current player
		fragment to that thread

	(2) Once a follower player active object thread receives a task, it will at some point
		call recite on the shared reference to a play object, which blocks if the calls
		happen out of order of the line numbers

	(3) At some point during the program execution, (since the number of threads are limited)
		all of the threads will be blocked in the recite method, as some players will be 
		handed their lines to recite out of order.

	(4) At this point, the leader thread is the only active thread, but it will be livelocked
		around spinwaiting for a condition (idle = true) which will never be satisfied because
		no follower thread will become unblocked as the play object will be waiting for the  
		line number (by a part that is not yet handed to any thread) that breaks the deadlock
		between the waiting followers.
	
== Acknowledgement ==

This project refers to no code from previous assignment by students outside of
this team.
