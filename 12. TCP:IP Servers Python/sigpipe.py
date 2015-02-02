__author__ = 'Sudheesh Singanamalla <ssingana@redhat.com>'

import socket

# connect to rstserver, port 2000
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('', 2000))

# first read gets an RST packet
try:
    s.recv(1024)
except socket.error as e:
    print e
    print

# write after getting the RST causes SIGPIPE signal
# to be sent to this process which causes a socket.error
# exception
s.send('hello')
