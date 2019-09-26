#!/usr/bin/env bash

rm my-router
rm data_injection
rm routing-outputA.txt
rm routing-outputB.txt
rm routing-outputC.txt
rm routing-outputD.txt
rm routing-outputE.txt
rm routing-outputF.txt
rm routing-outputG.txt
rm routing-outputH.txt
rm routing-outputI.txt
rm routing-outputJ.txt
rm routing-outputK.txt
rm routing-outputL.txt
rm routing-outputM.txt
rm routing-outputN.txt
rm routing-outputO.txt
rm routing-outputP.txt
rm routing-outputQ.txt
rm routing-outputR.txt
rm routing-outputS.txt
rm routing-outputT.txt
rm routing-outputU.txt
rm routing-outputV.txt
rm routing-outputW.txt
rm routing-outputX.txt
rm routing-outputY.txt
rm routing-outputZ.txt

rm forwarding-output.txt
make

echo 'gnome-terminal --geometry=40x20 --title="A" --command="./my-router A" & sleep 1
gnome-terminal --geometry=40x20 --title="B" --command="./my-router B" & sleep 1
gnome-terminal --geometry=40x20 --title="C" --command="./my-router C" & sleep 1
gnome-terminal --geometry=40x20 --title="D" --command="./my-router D" & sleep 1
gnome-terminal --geometry=40x20 --title="E" --command="./my-router E" & sleep 1
gnome-terminal --geometry=40x20 --title="F" --command="./my-router F" & sleep 1' | shuf > exec
chmod 777 exec
./exec
rm exec

#echo 'xterm -title "Router A" -hold -e "./my-router A" & sleep 1
#xterm -title "Router B" -hold -e "./my-router B" & sleep 1
#xterm -title "Router C" -hold -e "./my-router C" & sleep 1
#xterm -title "Router D" -hold -e "./my-router D" & sleep 1
#xterm -title "Router E" -hold -e "./my-router E" & sleep 1
#xterm -title "Router F" -hold -e "./my-router F" & sleep 1' | shuf > exec
#chmod 777 exec
#./exec
#rm exec

#this is a shell script that will delete the executable and text files and then make the project.
#it will then procceed to run 6 routers in individual terminals (A->F) in a random order.

#you type "chmod +x compile.sh" once only, or use sh compile.sh
#from then on you just type "sh compile.sh" instead of "make"