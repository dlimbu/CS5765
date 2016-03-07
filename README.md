#####Compile server and client

$ make sns snc

output: two binaries will be created sns(simple network server) and snc (simple network client)

#####Running Server:

./sns <ipaddress> <port>

#####Server supported commands:

- ls
- cd
- cd..
- pwd
- get
- exit

#####Running client:

./snc <ipaddress> <port>

#####Client supported commands:

- lls
- lpwd
- lcd
- bye
- help
- example:

```
$: help

Simple Network Client commands:
Remote:
ls: list files
pwd: server current directory
cd: change server directory
get:download a file

Local:
lls: list local files
lpwd: local current directory
cd: change local directory
get:download a file
exit/bye: quit snc app
help: list all available commands

$: ls
cs5765 files 

$: lls
files lol myFiles picture test.txt text1.txt text2.txt tms 

$: cd test.txt

$: ls
cs5765 files 

$: lls
files lol myFiles picture test.txt text1.txt text2.txt tms 

$: lpwd
/clientStorage

$: pwd
/serverStorage

$: get test.txt
Download Complete

$: lcd myFiles

$: lpwd
/clientStorage/myFiles
```