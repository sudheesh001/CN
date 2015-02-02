"""
TCP Preforked Server, Children Call 'accept'

Pool of child processes handle client requests.
"""

__author__ = 'Sudheesh Singanamalla <ssingana@redhat.com>'


import os
import errno
import signal
import socket
import optparse

BACKLOG = 5

# stores pids of all preforked children
PIDS = []


def handle(sock):
    # read a line that tells us how many bytes to write back
    bytes = int(sock.recv(1024))
    # get our random bytes
    data = os.urandom(bytes)

    print 'Got request to send %d bytes. Sending them all...' % bytes
    # send them all
    sock.sendall(data)


def child_loop(index, listen_sock):
    """Main child loop."""
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

        handle(conn)

        # close handled socket connection and off to handle another request
        conn.close()


def create_child(index, listen_sock):
    pid = os.fork()
    if pid > 0: # parent
        return pid

    print 'Child started with PID: %s' % os.getpid()
    # child never returns
    child_loop(index, listen_sock)


def _cleanup(signum, frame):
    """SIGTERM signal handler"""
    # terminate all children
    for pid in PIDS:
        try:
            os.kill(pid, signal.SIGTERM)
        except:
            pass

    # wait for all children to finish
    while True:
        try:
            pid, status = os.wait()
        except OSError as e:
            if e.errno == errno.ECHILD:
                break
            else:
                raise

        if pid == 0:
            break

    os._exit(0)


def serve_forever(host, port, childnum):
    # create, bind, listen
    listen_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # re-use the port
    listen_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    listen_sock.bind((host, port))
    listen_sock.listen(BACKLOG)

    print 'Listening on port %d ...' % port

    # prefork children
    global PIDS
    PIDS = [create_child(index, listen_sock) for index in range(childnum)]

    # setup SIGTERM handler - in case the parent is killed
    signal.signal(signal.SIGTERM, _cleanup)

    # parent never calls 'accept' - children do all the work
    # all parent does is sleeping :)
    signal.pause()


def main():
    parser = optparse.OptionParser()
    parser.add_option(
        '-i', '--host', dest='host', default='0.0.0.0',
        help='Hostname or IP address. Default is 0.0.0.0'
        )

    parser.add_option(
        '-p', '--port', dest='port', type='int', default=2000,
        help='Port. Default is 2000')

    parser.add_option(
        '-n', '--child-num', dest='childnum', type='int', default=10,
        help='Number of children to prefork. Default is 10')

    options, args = parser.parse_args()

    serve_forever(options.host, options.port, options.childnum)

if __name__ == '__main__':
    main()
