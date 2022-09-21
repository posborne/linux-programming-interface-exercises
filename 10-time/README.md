Chapter 10: Time
================

Exercise 10-1
-------------

**Question**

Assume a system where the value returned by the call
sysconf(_SC_CLK_TCK) is 100.  Assuming that the clock_t value returned
by times()  is a signed 32-bit integer, how long will it take before
this value cycles so that it restarts at 0?  Perform the same
calculation for the CLOCKS_PER_SEC value returned by clock().

**Answer**

sysconf(_SC_CLK_TCK) tells us the number of ticks per second and
times() tell us the number of ticks that have elapsed.  Given that the
maximum value of a signed 32-bit integer is 2147483647, and the elapsed 
time in units of clock ticks is 2147483647 - 0 + 1 = 2147483648. We can
calculate out the max time to be:

    >>> secs = 2147483648 / 100.0
    >>> print secs
    21474836.47
    >>> minutes = secs / 60
    >>> hours = minutes / 60
    >>> days = hours / 24
    >>> print days
    248.55134814814812
    >>> years = days / 365
    >>> print years
    0.6809625976661592

At 1 million, as is the case on my machine we get:

    >>> secs = 2147483648 / 1000000.0
    >>> print secs
    2147.483648
    >>> hours = secs / 60.0 / 60.0
    >>> print hours
    0.5965232355555555
