#!/bin/sh

if [ ! -e /usr/local/include/jshutest.inc ]; then
	echo "This unit test relies on the jshu project file jshutest.inc "
	echo " being installed. Please install it in /usr/local/include."
	exit 1
fi
. /usr/local/include/jshutest.inc

if [ -z ${WORKSPACE+x} ]; then
	echo "This unit test requires that the environment variable"
	echo "WORKSPACE be defined and set to the path of the shelfun directory."
	exit 1
fi

# get my including script's installed directory
if [ -z ${INCDIR+x} ] ; then
    installed_dir=${WORKSPACE}/include
else
    installed_dir=$INCDIR
fi

. ${installed_dir}/readconfig.inc

#---------------------------------------------------

setCfgValueTest() {
    setCfgValue MYTEST "value1"
    val=$(getCfgValue MYTEST)
    if [ $? -ne 0 ] ; then
    	return ${jshuFAIL}
    fi
    if [ "$val" != "value1" ]; then
    	jshu_errmsg="MYTEST key value was not set to 'value1'"
    	return ${jshuFAIL}
    fi
    return ${jshuPASS}
}

clearCfgTest() {
	# make sure that the config array is not empty
    setCfgValue MYCLRTEST "clrvalue1"
    
    clearCfg
    if [ ${#config[@]} -ne 0 ]; then
    	jshu_errmsg="config array was not empty ${#config[@]}"
    	return ${jshuFAIL}
    fi
    return ${jshuPASS}
}

setDefaultCfgUnassignedTest() {
    clearCfg
    setDefaultCfgValue MYDEFTEST "defvalue1" 
    val=$(getCfgValue MYDEFTEST)
    if [ "$val" != "defvalue1" ]; then
    	jshu_errmsg="Failed to assign default value to MYDEFTEST"
    	return ${jshuFAIL}
    fi
    return ${jshuPASS}
}

setDefaultCfgAssignedTest() {
    clearCfg
    setCfgValue MYDEFTEST "customvalue1" 
    setDefaultCfgValue MYDEFTEST "defvalue1" 
    val=$(getCfgValue MYDEFTEST)
    if [ "$val" == "defvalue1" ]; then
    	jshu_errmsg="Failed to assign default value to MYDEFTEST"
    	return ${jshuFAIL}
    fi
    if [ "$val" != "customvalue1" ]; then
    	jshu_errmsg="Unexpected value in MYDEFTEST ${val}"
    	return ${jshuFAIL}
    fi
    return ${jshuPASS}
}

hasCfgKeyTest() {
	clearCfg
	if hasCfgKey MYTEST; then
		jshu_error="Unexpectedly found key in array."
		return ${jshuFAIL}
	fi
	setCfgValue MYTEST "hasvalue1"
	if ! hasCfgKey MYTEST; then
		jshu_error="Could not find key in array."
		return ${jshuFAIL}
	fi
    
    return ${jshuPASS}
}

removeCfgKeyTest() {
	clearCfg
	setCfgValue MYRMTEST "rmvalue1"
    setCfgValue YourRMTEST "rmvalue2"
    
    removeCfgKey MYRMTEST
    if [ ${#config[@]} -eq 0 ]; then
    	jshu_errmsg="config array was empty"
    	return ${jshuFAIL}
    fi
    
    return ${jshuPASS}
}


##############################################################
# main
##############################################################
# initialize testsuite
jshuInit

# run unit tests on script
jshuRunTests

# result summary
jshuFinalize

echo Done.
echo
let tot=failed+errors
exit $tot

