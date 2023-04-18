import socket
import json
import time
import sys
import platform
import os

HEADERSIZE=10

with open('client_config.json') as f:
    config = json.load(f)

server_address = (config['ip'],config['port']) 

def send_message(message):
    print("sending message: " + message)

    client_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM) 
    client_socket.connect(server_address) 
    message = (message).encode()
    message_size = len(message)
    client_socket.send(message_size.to_bytes(4, 'little'))
    client_socket.send(message)
    client_socket.close()

def get_logs():
    client_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM) 
    client_socket.connect(server_address) 
    message = b'logs'
    message_size = len(message)
    client_socket.send(message_size.to_bytes(4, 'little'))
    client_socket.send(message)
    data = client_socket.recv(4096 * 8)
    print(data.decode())
    client_socket.close()

def send_image(image):
    print("sending image: " + image)

    client_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM) 
    client_socket.connect(server_address) 
    message = ('image='+image).encode()
    message_size = len(message)
    client_socket.send(message_size.to_bytes(4, 'little'))
    client_socket.send(message)
    with open(image, 'rb') as f:
        image_data = f.read()
        image_data_size = len(image_data)

    client_socket.send(image_data_size.to_bytes(4, 'little'))
    client_socket.send(image_data)  
    client_socket.close()

def send_message_m(msg, sw):
    # s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    header= f'{len(msg):< 10}'
    f_msg=header+str(msg)
    # s.connect((HOST,PORT))
    sw.send(bytes(f_msg,"utf-8"))
    # sw.recv(1)

def read_msg(s, decode):
    msg=s.recv(HEADERSIZE)
    msg_l=int(msg)
    f_msg= b''
    print(f'New message with {msg_l} length')   
    # msg=s.recv(msg_l)

    while(len(f_msg)<msg_l):
        msg=s.recv(1)
        f_msg+=msg
    print(len(f_msg))
    s.send(bytes("b","utf-8"))
    if decode==True:
        return f_msg.decode('utf-8')
    else:
        return f_msg
        
def get_images(nbr_of_images,s):
    for i in range(nbr_of_images):
        message= read_msg(s,True)
        print(message)
        # image = read_msg(s, False)
        msg=s.recv(HEADERSIZE)
        msg_l=int(msg)
        f_msg= b''
        print(f'New image with {msg_l} length')   
        # msg=s.recv(msg_l)
        while len(f_msg)< msg_l:
            msg=s.recv(1024)
            f_msg+=msg
            if(msg_l-len(f_msg)<1024):
                f_msg+=s.recv(msg_l-len(f_msg))
                break
        if not os.path.exists('images'):
            os.mkdir('images')
        with open("images/" + message, 'wb') as f:
            f.write(f_msg)

    

def help():
    if platform.system() == 'Linux':
        print("python3 client.py stop   :  to stop the robot")
        print("python3 client.py start  :  to start the robot")
        print("python3 client.py config :  to configure the robot from config.json file")
        print("python3 client.py delete  :  to delete all images that the robot can recognize")
        print("python3 client.py getpictures   :  to get pictures from the robot")
        print("python3 client.py help   :  for this help message")

    else:
        print("py client.py stop    :  to stop the robot")
        print("python client.py start   :  to start the robot")
        print("python client.py config  :  to configure the robot from config.json file")
        print("python client.py delete  :  to delete all images that the robot can recognize")
        print("python client.py getpictures    :  to get pictures from the robot")
        print("python client.py help    :  for this help message")

    

if __name__ == '__main__':
    argc = len(sys.argv)
    if argc != 2:
        print("make sure to have 1 argument only")
        help()
    else:
        if sys.argv[1] == 'help':
            help()
        
        elif sys.argv[1] == 'start':
            send_message('start')

        elif sys.argv[1] == 'stop':
            send_message('stop')

        elif sys.argv[1] == 'config':
            send_message('stop')
            config_str = json.dumps(config['config'])
            send_message("{\"config\":" + config_str +"}")
            for image in config['images']:
                send_image(image)

        elif sys.argv[1] == 'delete':
            send_message('delete')
        
        elif sys.argv[1] == 'getpictures':
            # send_message('getpics')
            s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
            s.connect(server_address)
            send_message_m('getpics',s)
            nbr_of_images= int(read_msg(s,True))
            print(f'Receiving {nbr_of_images} images.')
            get_images(nbr_of_images, s)            
            s.close()
            send_message('donepics')



        elif sys.argv[1] == 'logs':
            send_message('logs')
            get_logs()
        else:
            print("unknown argument")
            help()
