"""
Illustrates a problem with a race condition that might happen if a process
needs to monitor several descriptors for I/O and wait for the delivery of
a signal.
"""

__author__ = 'Sudheesh Singanamalla <ssingana@redhat.com>'

import os
import sys
import time
import errno
import select
import signal

GOT_SIGNAL = False


def handler(signum, frame):
    global GOT_SIGNAL
    GOT_SIGNAL = True


def main():
    print 'PID: %s' % os.getpid()

    signal.signal(signal.SIGUSR1, handler)

    # read, write, exception lists with descriptors to poll
    rlist, wlist, elist = [sys.stdin.fileno()], [], []

    print 'Sleep for 10 secs'
    time.sleep(10)
    print 'Wake up and block in "select"'

    #
    # Nasty racing can happen at this point if the signal arrives before
    # the call to 'select' - the call won't be interrupted and 'select'
    # will block
    #

    # block in select
    try:
        readables, writables, exceptions = select.select(rlist, wlist, elist)
    except select.error as e:
        code, msg = e.args
        if code == errno.EINTR:
            if GOT_SIGNAL:
                print 'Got signal'
        else:
            raise


if __name__ == '__main__':
    main()
