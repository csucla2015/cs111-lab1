#! /bin/sh
sleep 5
sleep 6
(sleep 1 ; echo hello)
sleep 2
(sleep 2 ; echo hi)

(sleep 3 ; echo fail)  
(sleep 2 ; echo no)
