# shelfun
Assorted includes with general purpose shell functions

Note: The unit tests for these functions rely on jshu in order to run (see http://github.com/Shadowfen/jshu). Please install the jshutest.inc file in /usr/local/include before trying to run the unittests.


## readconf.inc 
This collection of shell functions provides the capability to read a simple key=value configuration file, make the values easily accessible to the script, and to provide default values for keys which were not defined in the configuration file. The configuration keys are extensible (and ignore-able) - your script only uses the key values that you expect to get.

One reason for using this instead of simply including a shell script fragment that defines the variables you with to have is to avoid executing whatever random contents that the "shell config" file might have been tainted with. These functions read the file and ignore anything that does not look like a key=value pair.


