(echo hello) || (echo hi) && (echo no) && (echo hi);  #Subshells with AND and OR

(echo one) | wc -w; #Subshells with pipe

echo two || (echo three); #subshell to the right of binary

(echo left) && echo right; #subshell to the left of binary

echo text to file > text #simple command with output redirect

echo one | wc -w #regular command with pipe

(echo a; echo b; echo c; echo d) #sequence in subshell

(echo show && echo show || echo hide) #AND and OR inside subshell

echo hello > in.txt
(cat) < in.txt 





