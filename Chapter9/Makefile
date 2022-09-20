smsh1: smsh1.o splitline.o execute.o
	gcc smsh1.o splitline.o execute.o -o smsh1
smsh2: smsh2.o splitline.o execute.o process.o controlflow.o 
	gcc smsh2.o splitline.o execute.o process.o controlflow.o \
	-o smsh2
smsh3: smsh2.o splitline.o execute.o process2.o controlflow.o \
       builtin.o varlib.o
	gcc smsh2.o splitline.o execute.o process2.o controlflow.o \
	builtin.o varlib.o -o smsh3
smsh4: smsh4.o splitline.o execute2.o process2.o controlflow.o \
       builtin.o varlib.o
	gcc smsh4.o splitline.o execute2.o process2.o controlflow.o \
	builtin.o varlib.o -o smsh4
clean: 
	rm *.o	

