all: reliable_receiver reliable_sender
reliable_receiver: receiver_main.c
	gcc -o reliable_receiver receiver_main.c -lpthread
reliable_sender: sender_main.c
	gcc -o reliable_sender sender_main.c -lpthread -lm
clean:
	rm -f reliable_receiver reliable_sender
