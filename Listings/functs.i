# 1 "functs.cpp"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 448 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "functs.cpp" 2
# 1 ".\\functs.h" 1



void delay();
# 2 "functs.cpp" 2

void delay(){
 volatile int i;
 for (i = 0; i < 100000; i++);
}
