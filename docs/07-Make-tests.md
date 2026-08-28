```text
{
	echo "=== burnout 3 fifo ==="
	./codexion 3 800 200 200 200 10 100 fifo

	echo "=== burnout 3 edf ==="
	./codexion 3 800 200 200 200 10 100 edf

	echo "=== burnout 5 fifo ==="
	./codexion 5 800 200 200 200 10 100 fifo

	echo "=== burnout 5 edf ==="
	./codexion 5 800 200 200 200 10 100 edf

	echo "=== cooldown fifo ==="
	./codexion 3 3000 200 200 200 5 500 fifo

	echo "=== cooldown edf ==="
	./codexion 3 3000 200 200 200 5 500 edf

	echo "=== small stress fifo ==="
	./codexion 5 1000 50 50 50 20 20 fifo

	echo "=== small stress edf ==="
	./codexion 5 1000 50 50 50 20 20 edf
} > codexion_tests.txt
```


```text
{
	echo "=== VALGRIND 1 FIFO ==="
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	./codexion 1 800 200 200 200 2 100 fifo

	echo "=== VALGRIND 5 FIFO ==="
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	./codexion 5 1500 200 200 200 5 100 fifo

	echo "=== VALGRIND 5 EDF ==="
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	./codexion 5 1500 200 200 200 5 100 edf

	echo "=== HELGRIND 3 FIFO ==="
	valgrind --tool=helgrind \
	./codexion 3 1500 200 200 200 5 100 fifo

	echo "=== HELGRIND 5 EDF ==="
	valgrind --tool=helgrind \
	./codexion 5 1500 200 200 200 5 100 edf

	echo "=== HELGRIND STRESS EDF ==="
	valgrind --tool=helgrind \
	./codexion 5 1000 50 50 50 20 20 edf

} > valgrind_helgrind_tests.txt 2>&1
```