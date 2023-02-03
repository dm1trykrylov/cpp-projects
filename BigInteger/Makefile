all:
	g++ -g -DIO -DINCR -DMINUS -DLIT -DCONV -DDIV -DMULT -DRAT ./test.cpp -o ./test
	./test
io:
	g++ -g -DIO ./test.cpp -o ./test
	./test
minus:
	g++ -g -fsanitize=address -DMINUS ./test.cpp -o ./test
	./test
div:
	g++ -g -DDIV ./test.cpp -o ./test
	./test < ./input.txt > ./testoutput.txt
	diff ./output.txt ./testoutput.txt
mult:
	g++ -g -DMULT -fsanitize=address ./test.cpp -o ./test
	./test < ./input.txt > ./testoutput.txt
	diff ./output.txt ./testoutput.txt
rat:
	g++ -g -DRAT ./test.cpp -o ./test
	./test < ./input.txt > ./testoutput.txt
	diff ./output.txt ./testoutput.txt
ratsan:
	g++ -g -DRAT -fsanitize=address ./test.cpp -o ./test
	./test < ./input.txt > ./testoutput.txt
	diff ./output.txt ./testoutput.txt
ratvg:
	g++ -g -DRAT ./test.cpp -o ./test
	valgrind ./test < ./input.txt > ./testoutput.txt
	diff ./output.txt ./testoutput.txt
minus:
	g++ -g -DMINUS ./test.cpp -o ./test
	./test
test: $(defs)
	g++ -g $(defs) ./test.cpp -o ./test
	./test

