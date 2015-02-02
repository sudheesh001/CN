"""
TCP Concurrent Server, I/O Multiplexing (select).

Single server process to handle any number of clients.
"""

__author__ = 'Sudheesh Singanamalla <ssingana@redhat.com>'

import os
import sys
import errno
import select
import socket
import optparse

BACKLOG = 5


def serve_forever(host, port):
    # create, bind. listen
    lstsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # re-use the port
    lstsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # put listening socket into non-blocking mode
    lstsock.setblocking(0)

    lstsock.bind((host, port))
    lstsock.listen(BACKLOG)

    print 'Listening on port %d ...' % port

    # read, write, exception lists with sockets to poll
    rlist, wlist, elist = [lstsock], [], []

    while True:
        # block in select
        readables, writables, exceptions = select.select(rlist, wlist, elist)

        for sock in readables:
            if sock is lstsock: # new client connection, we can accept now
                try:
                    conn, client_address = lstsock.accept()
                except IOError as e:
                    code, msg = e.args
                    if code == errno.EINTR:
                        continue
                    else:
                        raise
                # add the new connection to the 'read' list to poll
                # in the next loop cycle
                rlist.append(conn)
            else:
                # read a line that tells us how many bytes to write
                bytes = sock.recv(1024)
                if not bytes: # connection closed by client
                    sock.close()
                    rlist.remove(sock)
                else:
                    print ('Got request to send %s bytes. '
                           'Sending them all...' % bytes)
                    # send them all
                    # XXX: this is cheating, we should use 'select' and wlist
                    # to determine whether socket is ready to be written to
                    data = os.urandom(int(bytes))
                    sock.sendall(data)


def main():
    parser = optparse.OptionParser()
    parser.add_option(
        '-i', '--host', dest='host', default='0.0.0.0',
        help='Hostname or IP address. Default is 0.0.0.0'
        )

    parser.add_option(
        '-p', '--port', dest='port', type='int', default=2000,
        help='Port. Default is 2000')

    options, args = parser.parse_args()

    serve_forever(options.host, options.port)

if __name__ == '__main__':
    main()


