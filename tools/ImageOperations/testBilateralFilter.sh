#!/bin/bash

STARTDIR=`pwd`  
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd "$DIR" 

./imageopsutility ../../media/samples/test.jpg output0.jpg --contrast 2.4 --bilateral 0.4 0.4 
./imageopsutility ../../media/samples/test.jpg output1.jpg --contrast 2.4 --bilateral 1.0 1.0 
./imageopsutility ../../media/samples/test.jpg output2.jpg --contrast 2.4 --bilateral 5.0 6.0 
./imageopsutility ../../media/samples/test.jpg output3.jpg --contrast 2.4 --bilateral 25.0 26.0 
./imageopsutility ../../media/samples/test.jpg output4.jpg --contrast 2.4 --bilateral 125.0 126.0 

timeout 15 gpicview output0.jpg output1.jpg output2.jpg output3.jpg output4.jpg

exit 0