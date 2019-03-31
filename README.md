## SHA256 Algorithm in C
You must write a program in the C programming language that calculates the SHA-256 hecksum of an input. The algorithm is speciﬁed in the Secure Hash Standard
document supplied by the (United States) National Institute of Standards and Technology. The only pre-requisite is that your program performs the lgorithm — you are free to decide what input the algorithm should be performed on. I suggest you allow the user to input some free text or a ﬁlename via the
command line.

## Compiling
For creating this program I used the Google Cloud platform and created an
Instance on my machine to create the algorithm.

If you wish to compile and run thus program you must do the following:
- get a clone of this repo to your desired destination
- cd Theory-of-algorithms
- In order to compile this program the GCC compiler must be installed on your
  device which is why we opted to use an instance on Google Cloud
- Run the command "gcc -o sha256 sha256.c" in the terminal(or CMD) within your
  project folder.

## Running
After compiling your program you can then run it from terminal using the
following command:
```
./sha256 test.txt
```

## Additional Features
- Added to the program a feature that will display out on the output screen the
  contents of the file.

## Research/References
https://www.geeksforgeeks.org/little-and-big-endian-mystery/
https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm

