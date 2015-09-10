#!/bin/bash
#========================================================================
# readconfig.inc
#     version 1.0.2-01
#
# This collection of shell functions provides the capability to read a
# simple key=value configuration file, make the values easily accessible
# to the script, and to provide default values for keys which were not
# defined in the configuration file. The configuration keys are extensible
# (and ignore-able) - your script only uses the key values that you expect
# to get.
# 
# One reason for using this instead of simply including a shell script 
# fragment that defines the variables you with to have is to avoid 
# executing whatever random contents that the "shell config" file might 
# have been tainted with. 
# These functions read the file and ignore anything that does not look 
# like a key=value pair.
#
# Include this file into your shell script using
#        . /usr/local/include/readconfig.inc
# (or whatever path is appropriate to find this file).
#
# If you wish to provide fall-back default values for your configuration,
# you should create a "setDefaultValues" (or whatever you want to name it)
# function where you use the "SetDefaultCfgValue" function to set a value
# for a particular key - for instance,
#  
#    setMyDefaults() {
#        setDefaultCfgValue MYKEY1 "key1-value"
#        setDefaultCfgValue ANOTHER_KEY "This is a test"
#    }
#
# Then in the "main" part of your shell script, you would call
#
#    loadCfg "myconfig.cfg"
# followed by
#    setMyDefaults
#
# In this way, the setMyDefaults can add in config values that were not 
# provided in the "myconfig.cfg" configuration file. If "myconfig.cfg" 
# already defines MYKEY1="something-different", it WILL NOT be overwritten
# by the setDefaultCfgValue call.
#
# Configuration values get loaded into an associative array named "config".
# To access your config values, you can use the bash array syntax yourself,
#   echo ${config["MYKEY1"]}
# or, you can use the getCfgKeyValue function
#   echo $(getCfgKeyValue MYKEY1)
# and not really have to remember how bash array syntax works.
#
# Provided functions:
#    clearCfg
#        empties out the config array
#    loadCfg filename
#        loads the contents of the config file into the config array
#    printCfg
#        prints out the keys and values in the config array to stdout
#    getCfgValue keyname
#        echos the value assigned to that key in the config array.
#        returns 1 if an error occurred (missing parameter)
#    setDefaultCfgValue keyname value
#        if the specified key does not already exist in the config array,
#        then add it to the array with the specified value.
#    removeCfgKey keyname
#        removes the specified key from the config array if it exists.
#        returns 1 if the named key does not exist in the array.
#    hasCfgKey keyname
#        returns 0 if the key exists in the config array
#        returns 1 if the key does not exist in the config array

#========================================================================
# License: Simplified BSD:
#
# Copyright (c) 2015, Dolores Scott
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#========================================================================

# if config array already exists, this does not change it!
typeset -A config # init array

clearCfg() {
    config=()
}

loadCfg() {
    config_file="$1"
    if [ -z "$config_file" ] ; then
        echo "loadCfg: ERROR: You must specify a configuration file."
        return 1
    fi
    if [ ! -e "$config_file" ] ; then
        $LOG $LOG_OPTS "loadCfg: WARNING: Configuration file ($config_file) does not exist."
        return 0
    fi
    while read line
    do
        # be sure to skip commented-out lines
        if ! echo $line | grep -e '^[:space:]*#' &>/dev/null ; then
            # only process lines with '=' in them
            if echo $line | grep -F = &>/dev/null ; then
                varname=$(echo "$line" | cut -d '=' -f 1)
                config[$varname]=$(echo "$line" | cut -d '=' -f 2-)
            fi
        fi
    done < $config_file
    return 0
}

printCfg() {
    for key in "${!config[@]}"; do 
        echo "key: $key, value: ${config[$key]}"
    done
}

getCfgValue() {
    key="$1"
    if [ -z "$key" ] ; then
        echo "getCfgValue: ERROR: You must specify a field name."
        return 1
    fi
    echo ${config[$key]}
}

setCfgValue() {
    key="$1"
    shift
    val="$@"
    if [ -z "$key" ] ; then
        echo "setCfgValue: ERROR: You must specify a field name."
        return 1
    fi
    config[$key]=$val
}

setDefaultCfgValue() {
    key="$1"
    shift
    val="$@"
    if [ -z "$key" ] ; then
        echo "setDefaultCfgValue: ERROR: You must specify a field name."
        return 1
    fi
    # only create key:value if key does not exist already
    if ! hasCfgKey "$key" ; then
        config[$key]=$val
    fi
}

removeCfgKey() {
    key="$1"
    if [ -z "$key" ] ; then
        echo "removeCfgKey: ERROR: You must specify a field name."
        return 1
    fi
    # only remove key:value if key exists
    if [ "${config[$key]+isset}" ] ; then
        unset config["$key"]
    fi
}

# returns 0 if the key exists in the array (value can be null)
# returns 1 if the key does not exist in the array
hasCfgKey() {
    key="$1"
    if [ -z "${config[$key]+isset}" ] ; then
        return 1
    fi
    return 0
}


