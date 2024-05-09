--------------------------------------
Author
--------------------------------------
Mike Pryor
4/29/2024

--------------------------------------
About MusicStreamLoggerCodingChallenge
--------------------------------------
This project is in response to the coding challenge to monitor a music stream
service. The project was compiled with the Makefile developed on
Ubuntu 22.04.4 LTS.

--------------------------------------
Dependencies
--------------------------------------
Install g++ 11.4.0:

<code>sudo apt-get install g++</code>

This project includes the example header-only libraries provied by Josh.ai for
HTTP and JSON support as well as a command line parser:

C++11 header-only HTTP/HTTPS client library (httplib.h)
JSON for Modern C++ (nlohmann/json.hpp)
Simple C++ command line parser (cmdparser.hpp)


In addition to those, a test framework library supported testing:

Acutest: Simple header-only C/C++ Unit Testing Facility (acutest.h)

--------------------------------------
Project files
--------------------------------------
The project includes the following source files and output files to run the
coding challenge C++ application:

IObserver.h
Logger.h
Logger.cpp
MusicStreamMonitor.h
MusicStreamMonitor.cpp
MusicStreamLogger.h
MusicStreamLogger.cpp
main.cpp

Output
${INSTALL_DIR}/bin/MusicStreamLogger

--------------------------------------
Test files
--------------------------------------
TestMusicStream.cpp

Output
${INSTALL_DIR}/tests/TestMusicStream

--------------------------------------
MusicStreamLogger
--------------------------------------
The MusicStreamLogger application is the solution to the coding
challenge. To compile the application from the provided source code, run

<code>make install</code>

from the terminal. If you would like to choose a different install directory
before installing, export INSTALL_DIR, e.g.

<code>export INSTALL_DIR=$HOME/install</code>

The Makefile targets will set up the appropriate directory structure.

To run MusicStreamLogger, a run.bash script provides examples of running the
application with optional arguments for the MusicStreamMonitor to monitor the 
music stream simulator for changes in the stream.

<code>$INSTALL_DIR/bin/MusicStreamLogger --help</code>

provides the help dialog for more information.

------------------------------
Standard Includes
------------------------------

The project includes 
<code>iostream</code> for <code>std::cout</code> and <code>std::endl</code>,
<code>map</code> for hash map key-value pairs,
<code>memory</code> for <code>std::unique_ptr</code>,
<code>set</code> for <code>std::set</code>,
<code>string</code> for <code>std::string</code>,
<code>thread</code> for <code>std::thread</code>, and
<code>vector</code> for the <code>std::vector</code> data structure.

