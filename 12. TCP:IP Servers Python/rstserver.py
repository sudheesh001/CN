"""Generates a TCP RST packet on client connect"""

__author__ = 'Sudheesh Singanamalla <ssingana@redhat.com>'

import struct
import socket
import optparse

BACKLOG = 5


def generate_rst(sock):
    l_onoff = 1 # cause RST to be sent on socket.close()
    l_linger = 0
    ling = struct.pack('ii', l_onoff, l_linger)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, ling)
    sock.close() # RST is sent


def serve_forever(host, port):
    # create, bind, listen
    listen_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # re-use the port
    listen_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    listen_sock.bind((host, port))
    listen_sock.listen(BACKLOG)

    print 'Listening on port %d ...' % port

    # iterative server
    while True:
        # block waiting for connection to handle
        try:
            conn, client_address = listen_sock.accept()
        except IOError as e:
            code, msg = e.args
            if code == errno.EINTR:
                continue
            else:
                raise

        # generate RST and off to accept another request
        print 'Sending RST to %s' % str(client_address)
        generate_rst(conn)


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
