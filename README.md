# Project 1: System Inspector


According to the man pages, top "provides a dynamic real-time view of a running system" (man pages). This system inspector is a simpler version of top, providing a view of my KVM.

This is done by reading through /proc, which holds info on every (nearly every?) process running the KVM.

This program has command-line options, given through the following:
<pre>
-a: Show all
-h: help
-l: Task list
-p new_dir: Reads from new_dir instead of /proc
-r: Hardware info
-s: System info
-t: Task info
</pre>

Commandline arg info is based off the sample readme (https://www.cs.usfca.edu/~mmalensek/cs326/schedule/materials/kylie-readme.html).


To compile and run:

```bash
make
./inspector
```

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'
```


# Program Output

Here is the output from running the following code on my KVM:
<pre>

Input: ./inspector

Output:

Hostname: KevinM | Kernel Version: 5.10.11
CPU: AMD EPYC Processor (with IBPB), Processing Units: 2
Uptime: 28 days, 21 hours, 0 minutes, 39 seconds

Load Average (1/5/15 min): 0.00 0.00 0.00
CPU Usage:    [--------------------] 0.9%
Memory Usage: [###-----------------] 17.2% (0.2 / 1.4 GB)

Tasks: 111 total
1 running, 0 waiting, 110 sleeping, 0 stopped, 0 zombie

      PID |                 Task Name |        State |            User
----------+---------------------------+--------------+-----------------
   117487 |                 inspector |      running |        jrreduta

</pre>