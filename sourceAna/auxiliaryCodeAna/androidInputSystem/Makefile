TARGET:=inotifyTest

$(TARGET):$(TARGET).o
	gcc	-o $@	$<

%.c:%.o
	gcc -c $@

clean:
	rm $(TARGET) $(TARGET).o
