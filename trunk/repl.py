import zmq

context = zmq.Context()

socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:5555")

while True:
    try:
        s = raw_input("> ")
        socket.send(s.encode())
        print socket.recv()
    except EOFError:
        print
        break
