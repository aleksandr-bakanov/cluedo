import socket

HOST = 'localhost'  # The remote host
PORT = 7003         # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

data = s.recv(1024);
print 'Received', repr(data)

er = u'\x03\x00\x02\x00\x03'
s.send(er)

data = s.recv(1024);
print 'Received', repr(data)

er = u'\x03\x00\x06\x00\x01'
s.send(er)

while True:
    data = s.recv(1024);
    print 'Received', repr(data)

s.close()
