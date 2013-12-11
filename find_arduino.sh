#/usr/bin/env sh
ls /dev/ -l > ~/nokey
echo Connect the Arduino and press [ENTER]
read
ls /dev/ -l > ~/key
echo Difference:
diff ~/nokey ~/key
