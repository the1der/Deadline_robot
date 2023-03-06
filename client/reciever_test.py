import socket
import os
import socket
import json

with open('client_config.json') as f:
    config = json.load(f)

server_address = (config['ip'],config['port']) 

# set up TCP socket
HOST = 'localhost'
PORT = 9999
BUFFER_SIZE = 1024

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()

    print(f"Listening on {HOST}:{PORT}")
    conn, addr = s.accept()

    with conn:
        print(f"Connected by {addr}")

        # receive images in a loop
        count = 0
        while True:
            # receive image size
            size_bytes = conn.recv(4)
            if not size_bytes:
                break
            size = int.from_bytes(size_bytes, byteorder='big')

            # receive image data
            data = b''
            while len(data) < size:
                packet = conn.recv(BUFFER_SIZE)
                if not packet:
                    break
                data += packet

            # save image to file
            filename = f"received_image_{count}.jpg"
            with open(filename, "wb") as f:
                f.write(data)

            print(f"Image {count} saved to {filename}")
            count += 1

    print("All images received and saved.")
