import socket, sys
import time

class MiniClient:
    h = ''
    p = ''
    m = ''
    
    def __init__(self, host, port, mode):
        self.h = host
        self.p = int(port)
        self.m = mode
    def tcpC4(self):
        tcpT4Client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print "Done........"
        tcpT4Client.connect((self.h, self.p))
        print "TCP IPv4 TCP mode connecting..."
        while True:
            time.sleep(1)
            tcpT4Client.send('hello')
            print "hello send to Server"

    def udpC4(self):
        udpT4Client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        print "UDP TCP IPv4 Mode connecting..."
        while True:
            time.sleep(1)
            udpT4Client.sendto("hello", (self.h, self.p))
            print "Hello Send to " , self.h , ' Use ', self.p, 'Port'
                
    def tcpC6(self):
        tcpT4Client = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        print "Done........"
        tcpT4Client.connect((self.h, self.p))
        print "TCP IPv6 TCP mode connecting..."
        while True:
            time.sleep(1)
            tcpT4Client.send('hello')
            print "hello send to Server"        
        
    def udpC6(self):
        udpU6Client = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
        print "UDP TCP IPv4 Mode connecting..."
        while True:
            time.sleep(1)
            udpU6Client.sendto("hello", (self.h, self.p))
            print "Hello Send to " , self.h , ' Use ', self.p, 'Port'
            
if __name__ == "__main__":
    x = MiniClient(sys.argv[1], sys.argv[2], sys.argv[3])
    if x.m == 't4':
        x.tcpC4()
    elif x.m == 't6':
        x.tcpC6()
    elif x.m == 'u4':
        x.udpC4()
    else:
        x.udpC6()