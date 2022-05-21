# /usr/bin/env bash
#
#
red=`tput setaf 1`
green=`tput setaf 2`
reset_color(){ 
	tput sgr0 
}

# Tidy up function after tests execution
cleaning_up() {
	rm -f test_1.txt  
	rm -f  test_2.txt  
	rm -f test_3.txt
	rm -f test_4.txt
}

early_exit(){
	reset_color
	echo "Proceed to early exit"
	cleaning_up
	exit
}

CS_DIR="${1:-REPO}"

echo CS_DIR set to $CS_DIR

echo "## Running simple tests for mini_cs ##"
cd $CS_DIR


#
# TEST 1: Empty File creation
#
echo "Test1: file creation: touch "
rm -f test_1.txt
touch test_1.txt

if [ -f test_1.txt ]
then
	echo "${green} Test1 succeeded"
else
	echo "${red} Test1 failed"
	exit
fi
reset_color


#
# TEST 2: File append
#
echo "Test2: file append: writing data to empty file"
test_content="test2 done"
rm -f test_2.txt

echo -n $test_content  > test_2.txt
if [ `stat -c "%s" test_2.txt` -eq ${#test_content} ]
then
	echo "${green} Test2 succeeded"
else
	echo "${red} Test2 failed"
	early_exit
fi
reset_color

#
# TEST 3: Non-Empty File Creation
#
echo "Test3: file creation with data : writing data to empty file"
rm -f test_3.txt
test_content="test3 done"
echo -n $test_content > test_3.txt

if [ `stat -c "%s" test_3.txt` -eq ${#test_content} ]
then
	echo "${green} Test3 succeeded"
else
	echo "${red} Test3 failed"
	early_exit
fi
reset_color

#
# TEST 4: Remove file
#
echo "Test4: remove file"
test_content="test4 done"
echo -n $test_content > test_4.txt

if [ ! -f test_4.txt ]
then
	echo "${red} Test4 failed"
	early_exit
fi

rm -f test4.txt 
if [ $? -ne 0 ]
then
	echo "${red} Test4 failed"
	early_exit
else
	echo "${green} Test4 succeeded"
fi
reset_color

#
# TEST 5: Create an empty directory
#
echo "Test5: create directory"
mkdir TEST_DIR.$$
if [ -d TEST_DIR.$$ ]
then
	echo "${green} Test5 succeeded"
else
	echo "${red} Test5 failed"
	early_exit
fi
reset_color

#
# TEST 6: Populate an empty directory with a file
#
echo "Test6: create file in the newly created directory"
test_content="test6 done"
echo -n $test_content > ./TEST_DIR.$$/test_6.txt

if [ `stat -c "%s" ./TEST_DIR.$$/test_6.txt` -eq ${#test_content} ]
then
	echo "${green} Test6 succeeded"
else
	echo "${red} Test6 failed"
	early_exit
fi
reset_color

#
# TEST 7: Remove a non-empty directory
#
echo "Test7: remove directory"
rm -rf mkdir TEST_DIR.$$

if [ $? -ne 0 ]
then
	echo "${red} Test7 failed"
	early_exit
else
	echo "${green} Test7 succeeded"
fi
reset_color

cleaning_up


