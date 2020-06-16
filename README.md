# Kernel Module (Character Device) to list active processes

## This kernel module lists all the active processes and their respective 
   states


### STEPS:
	- make
	- sudo insmod process_list.ko
	- sudo dmesg --> To get the MajorNumber
	- sudo rm /dev/process_list
	- sudo mknod /dev/process_list c MajorNumber 0
	- sudo ./userproc
	- sudo rmmod process_list.ko
	- sudo rm /dev/process_list
	- make clean
