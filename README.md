start_detached
====================================

**start_detached** is a command line tool for Windows that serves a similar purpose as **nohup** on Linux.

It starts a command detached from the current [Console](http://msdn.microsoft.com/en-us/library/windows/desktop/ms682010%28v=vs.85%29.aspx) and in its own [Process Group](http://msdn.microsoft.com/en-us/library/windows/desktop/ms682083%28v=vs.85%29.aspx). This means that you can close the Command Prompt and the program will keep running in the background. The program will not receive events for Ctrl+C or Ctrl+Break from the Console where **start_detached** was used.

Usually, the started command also will not open a new Command Prompt, but I can't guarantee this. Should the command or any subprocess create a new Console, a new Command Prompt window will probably also pop up.

Ending your user session, usually by logging out, will terminate the command. If you want to keep the command running even after logging out, I'd suggest to register it as a Windows Service instead. Apparently [instsrv and srvany](http://support.microsoft.com/kb/137890) from some Windows Resource Kit can be used for this.

Usage
-----

    start_detached [application] [arguments...]

For example, `start_detached ping 8.8.8.8 -t` will keep a ping-command running in the background. Note that the first argument has to be an executable. To start a script, you have to explicitly call the interpreter.

TODO
----

  * There are currently no options to redirect STDIN, STDOUT and STDERR of the detached program.
  * **start_detached** returns as soon as the process is created. It can still fail to start after this, and **start_detached** will not report this. Maybe I could let it wait for the process to actually start.

Build
-----

A 32 bit build of **start_detached** is available in the Downloads section of the github repo page ([here](https://github.com/downloads/mooware/start_detached/start_detached.exe)).

The repository also contains Visual Studio 2010 solution and project files. These can either be opened with the Visual Studio 2010 IDE, or used for building on the command line, for example with `devenv start_detached.sln /Build`.

License
-------

**start_detached** is licensed under the MIT license. See the LICENSE file in the repository for its full text.