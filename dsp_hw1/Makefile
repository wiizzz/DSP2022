.PHONY: all clean run loop
CC=g++
CFLAGS= -std=c++11 -O2
LDFLAGS=-lm
TARGET=train test
TRAIN_ITER=100

all: $(TARGET)

train: src/train.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

test: src/test.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

clean:
	rm -f $(TARGET)

acc: src/acc.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

run:
	./train $(TRAIN_ITER) model_init.txt data/train_seq_01.txt model_01.txt
	./train $(TRAIN_ITER) model_init.txt data/train_seq_02.txt model_02.txt
	./train $(TRAIN_ITER) model_init.txt data/train_seq_03.txt model_03.txt
	./train $(TRAIN_ITER) model_init.txt data/train_seq_04.txt model_04.txt
	./train $(TRAIN_ITER) model_init.txt data/train_seq_05.txt model_05.txt
	./test modellist.txt data/test_seq.txt result.txt
	./acc  result.txt data/test_lbl.txt acc.txt

loop:
	number=0 ; while [[ $$number -le $(TRAIN_ITER) ]] ; do \
		./train $$number model_init.txt data/train_seq_01.txt model_01.txt ; \
		./train $$number model_init.txt data/train_seq_02.txt model_02.txt ; \
		./train $$number model_init.txt data/train_seq_03.txt model_03.txt ; \
		./train $$number model_init.txt data/train_seq_04.txt model_04.txt ; \
		./train $$number model_init.txt data/train_seq_05.txt model_05.txt ; \
		./test modellist.txt data/test_seq.txt result.txt ; \
		./acc  result.txt data/test_lbl.txt acc.txt ; \
        ((number = number + 1)) ; \
    done


